#include "OpenGLGeometryPool.h"
#include "OpenGLBatchRender.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cassert>

namespace OnYuu {

    // =========================================================================
    // OpenGLGeometryPool
    // =========================================================================

    OpenGLGeometryPool::OpenGLGeometryPool(GLsizeiptr initialVertexBytes,
        GLsizeiptr initialIndexBytes)
        : vertexBufferSize_(initialVertexBytes)
        , indexBufferSize_(initialIndexBytes)
    {
        // Crea VBO unificato (immutabile se GL 4.4, altrimenti DYNAMIC_DRAW)
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize_, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Crea IBO unificato
        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize_, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // VAO globale con layout fisso
        setupVAO();

        std::cout << "[OpenGLGeometryPool] Created VBO=" << initialVertexBytes
            << " IBO=" << initialIndexBytes << " bytes | stride=" << VERTEX_STRIDE << "\n";
    }

    OpenGLGeometryPool::~OpenGLGeometryPool() { shutdown(); }

    // -------------------------------------------------------------------------
    // setupVAO
    //   Layout fisso, identico a MeshGPUusage (pos3+col4+uv2+norm3).
    //   Binding al VBO e IBO correnti al momento della chiamata.
    // -------------------------------------------------------------------------
    void OpenGLGeometryPool::setupVAO() {
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        // Binda esplicitamente VBO e IBO: il VAO memorizzerà questi binding
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_); // IBO è nello state del VAO

        // attrib 0: position vec3
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(POS_OFFSET));
        // attrib 1: color vec4
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(COL_OFFSET));
        // attrib 2: texCoord vec2
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(UV_OFFSET));
        // attrib 3: normal vec3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(NORM_OFFSET));

        glBindVertexArray(0);
        // Unbind esplicito DOPO aver slegato il VAO — CRUCIALE:
        // GL_ARRAY_BUFFER può essere slegato subito, ma GL_ELEMENT_ARRAY_BUFFER
        // NON deve essere slegato mentre il VAO è bindato (verrebbe rimosso dallo state).
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Non fare glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) qui — è già ok dopo unbind VAO
    }

    void OpenGLGeometryPool::bind()   const { glBindVertexArray(vao_); }
    void OpenGLGeometryPool::unbind() const { glBindVertexArray(0); }

    // -------------------------------------------------------------------------
    // allocateVertexRegion
    //   Allinea size a VERTEX_STRIDE. USA allocMutex_ (non rientrante).
    //   grow* viene chiamato SENZA il lock, poi si riprende il lock.
    // -------------------------------------------------------------------------
    GLBufferRegion OpenGLGeometryPool::allocateVertexRegion(GLsizeiptr size) {
        // NOTA: VERTEX_STRIDE=48 NON è una potenza di 2, quindi l'allineamento
        // bitwise "&~(stride-1)" darebbe risultati sbagliati (es. 124848 → 124880
        // che non è multiplo di 48). Usiamo la divisione intera che è sempre corretta.
        size = ((size + VERTEX_STRIDE - 1) / VERTEX_STRIDE) * VERTEX_STRIDE;

        std::unique_lock<std::mutex> lock(allocMutex_);

        // First-fit nella free-list
        for (auto it = vertexFreeList_.begin(); it != vertexFreeList_.end(); ++it) {
            if (it->size >= size) {
                GLBufferRegion region{ vbo_, it->offset, size, false };
                it->offset += size;
                it->size -= size;
                if (it->size == 0) vertexFreeList_.erase(it);
                return region;
            }
        }

        // Cresce se necessario — rilascia il lock durante il grow (operazione GL costosa)
        if (vertexUsedSize_ + size > vertexBufferSize_) {
            GLsizeiptr newSize = std::max(vertexBufferSize_ * 2, vertexUsedSize_ + size);
            lock.unlock();
            growVertexBuffer(newSize);
            lock.lock();
        }

        GLBufferRegion region{ vbo_, vertexUsedSize_, size, false };
        vertexUsedSize_ += size;
        return region;
    }

    GLBufferRegion OpenGLGeometryPool::allocateIndexRegion(GLsizeiptr size) {
        // sizeof(uint32_t)=4 è potenza di 2, il bitwise sarebbe ok,
        // ma usiamo divisione intera per coerenza e leggibilità
        size = ((size + 3) / 4) * 4;

        std::unique_lock<std::mutex> lock(allocMutex_);

        for (auto it = indexFreeList_.begin(); it != indexFreeList_.end(); ++it) {
            if (it->size >= size) {
                GLBufferRegion region{ ibo_, it->offset, size, true };
                it->offset += size;
                it->size -= size;
                if (it->size == 0) indexFreeList_.erase(it);
                return region;
            }
        }

        if (indexUsedSize_ + size > indexBufferSize_) {
            GLsizeiptr newSize = std::max(indexBufferSize_ * 2, indexUsedSize_ + size);
            lock.unlock();
            growIndexBuffer(newSize);
            lock.lock();
        }

        GLBufferRegion region{ ibo_, indexUsedSize_, size, true };
        indexUsedSize_ += size;
        return region;
    }

    // -------------------------------------------------------------------------
    // upload — non servono lock (GL è single-thread)
    // -------------------------------------------------------------------------
    void OpenGLGeometryPool::uploadVertexData(const GLBufferRegion& region,
        const void* data, GLsizeiptr size) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, region.offset, size, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLGeometryPool::uploadIndexData(const GLBufferRegion& region,
        const void* data, GLsizeiptr size) {
        // Binda direttamente l'IBO (NON il VAO) per evitare di sporcare lo state
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, region.offset, size, data);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // -------------------------------------------------------------------------
    // freeRegion
    // -------------------------------------------------------------------------
    void OpenGLGeometryPool::freeRegion(const GLBufferRegion& region) {
        std::lock_guard<std::mutex> lock(allocMutex_);

        if (!region.isIndex) {
            vertexFreeList_.push_back({ region.offset, region.size });
            mergeFreeList(vertexFreeList_);
        }
        else {
            indexFreeList_.push_back({ region.offset, region.size });
            mergeFreeList(indexFreeList_);
        }
    }

    void OpenGLGeometryPool::mergeFreeList(std::vector<FreeBlock>& list) {
        std::sort(list.begin(), list.end(),
            [](const FreeBlock& a, const FreeBlock& b) { return a.offset < b.offset; });
        for (size_t i = 0; i + 1 < list.size(); ) {
            if (list[i].offset + list[i].size == list[i + 1].offset) {
                list[i].size += list[i + 1].size;
                list.erase(list.begin() + static_cast<ptrdiff_t>(i + 1));
            }
            else { ++i; }
        }
    }
    }

    // -------------------------------------------------------------------------
    // grow — ricrea il buffer con glBufferData (stesso handle)
    //         POI aggiorna il binding nel VAO per l'IBO.
    // -------------------------------------------------------------------------
    void OpenGLGeometryPool::growVertexBuffer(GLsizeiptr newSize) {
        std::cout << "[OpenGLGeometryPool] Grow VBO " << vertexBufferSize_
            << " -> " << newSize << " bytes\n";

        // Leggi dati esistenti
        std::vector<uint8_t> tmp(static_cast<size_t>(vertexUsedSize_));
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        if (vertexUsedSize_ > 0)
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexUsedSize_, tmp.data());

        // Ricrea con nuova dimensione (stesso handle vbo_)
        glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
        if (vertexUsedSize_ > 0)
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertexUsedSize_, tmp.data());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        vertexBufferSize_ = newSize;

        // Il VAO punta a vbo_ per GL_ARRAY_BUFFER via glVertexAttribPointer —
        // il binding è per handle, non per dati, quindi rimane valido.
        vertexBufferSize_ = newSize;
    }

    void OpenGLGeometryPool::growIndexBuffer(GLsizeiptr newSize) {
        std::cout << "[OpenGLGeometryPool] Grow IBO " << indexBufferSize_
            << " -> " << newSize << " bytes\n";

        std::vector<uint8_t> tmp(static_cast<size_t>(indexUsedSize_));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        if (indexUsedSize_ > 0)
            glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexUsedSize_, tmp.data());

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
        if (indexUsedSize_ > 0)
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexUsedSize_, tmp.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // CRITICO: il VAO memorizza il binding GL_ELEMENT_ARRAY_BUFFER al momento
        // di setupVAO. Poiché ibo_ è lo STESSO handle (glBufferData non cambia l'handle,
        // ricrea solo la storage), il VAO punta ancora correttamente a ibo_. OK.
        indexBufferSize_ = newSize;
    }

    // -------------------------------------------------------------------------
    // LRU tracking — usa trackerMutex_ separato
    // -------------------------------------------------------------------------
    void OpenGLGeometryPool::registerMesh(const std::shared_ptr<Mesh>& mesh,
        uint64_t currentFrame) {
        std::lock_guard<std::mutex> lock(trackerMutex_);
        auto& info = meshUsageTracker_[mesh];
        info.lastUsedFrame = currentFrame;
        info.refCount = 1;
        info.markedForDeletion = false;
    }

    void OpenGLGeometryPool::updateMeshUsage(const std::shared_ptr<Mesh>& mesh,
        uint64_t currentFrame) {
        std::lock_guard<std::mutex> lock(trackerMutex_);
        auto it = meshUsageTracker_.find(mesh);
        if (it != meshUsageTracker_.end()) {
            it->second.lastUsedFrame = currentFrame;
            it->second.refCount++;
        }
        // Se non trovata: verrà registrata da registerMesh subito dopo l'upload
    }

    void OpenGLGeometryPool::collectGarbage(uint64_t currentFrame,
        uint32_t framesToKeep,
        OpenGLBatchRender* renderer) {
        std::vector<std::shared_ptr<Mesh>> toDelete;

        {
            std::lock_guard<std::mutex> lock(trackerMutex_);
            for (auto& [mesh, info] : meshUsageTracker_) {
                if (!info.markedForDeletion &&
                    (currentFrame - info.lastUsedFrame) > framesToKeep) {
                    info.markedForDeletion = true;
                    toDelete.push_back(mesh);
                }
            }
        }

        if (!toDelete.empty()) {
            std::cout << "[OpenGLGeometryPool] GC: removing " << toDelete.size()
                << " unused meshes\n";
        for (const auto& mesh : toDelete) {
            renderer->removeCachedMesh(mesh);
            std::lock_guard<std::mutex> lock(trackerMutex_);
            meshUsageTracker_.erase(mesh);
        }
        if (!toDelete.empty())
            std::cout << "[OpenGLGeometryPool] GC removed " << toDelete.size() << " meshes\n";
    }

    void OpenGLGeometryPool::shutdown() {
        if (vao_) { glDeleteVertexArrays(1, &vao_); vao_ = 0; }
        if (vbo_) { glDeleteBuffers(1, &vbo_);      vbo_ = 0; }
        if (ibo_) { glDeleteBuffers(1, &ibo_);      ibo_ = 0; }
        meshUsageTracker_.clear();
    }

    // =========================================================================
    // PooledMeshGL
    // =========================================================================

    PooledMeshGL::PooledMeshGL(const Mesh& mesh, std::shared_ptr<OpenGLGeometryPool> pool)
        : mesh_(mesh), pool_(pool) {
    }

    PooledMeshGL::~PooledMeshGL() { shutdown(); }

    // -------------------------------------------------------------------------
    // uploadToGPU
    //   Costruisce il vertex buffer interlacciato con stride FISSO 48 byte
    //   (pos3 + col4 + uv2 + norm3), riempiendo i canali mancanti con default.
    //   Questo garantisce che getBaseVertex() sia sempre corretto indipendentemente
    //   dai dati presenti nella Mesh.
    //
    //   CONTROLLO colore: MeshGPUusage richiede che color.size() == position.size()
    //   (altrimenti abortisce). Noi facciamo lo stesso controllo e riempiamo con
    //   bianco se mancante, invece di abortire.
    // -------------------------------------------------------------------------
    void PooledMeshGL::uploadToGPU() {
        if (uploaded_ || mesh_.position.empty()) return;

        vertexCount_ = static_cast<uint32_t>(mesh_.position.size());
        indexCount_ = static_cast<uint32_t>(mesh_.indices.size());

        const bool hasColor = !mesh_.color.empty();
        const bool hasTexCoord = !mesh_.texCoord.empty();
        const bool hasNormal = !mesh_.normal.empty();

        // Avvisa se i canali presenti hanno size diversa dai vertici
        // (stesso comportamento di MeshGPUusage)
        if (hasColor && mesh_.color.size() != vertexCount_) {
            std::cerr << "[PooledMeshGL] color.size()=" << mesh_.color.size()
                << " != position.size()=" << vertexCount_ << " — using white\n";
        }

        // stride fisso: 12 float / vertice = 48 byte
        std::vector<float> vdata;
        vdata.reserve(vertexCount_ * 12);

        for (uint32_t i = 0; i < vertexCount_; ++i) {
            // pos (3)
            vdata.push_back(mesh_.position[i].x);
            vdata.push_back(mesh_.position[i].y);
            vdata.push_back(mesh_.position[i].z);

            // color (4) — bianco se mancante o size mismatch
            if (hasColor && i < static_cast<uint32_t>(mesh_.color.size())) {
                vdata.push_back(mesh_.color[i].r);
                vdata.push_back(mesh_.color[i].g);
                vdata.push_back(mesh_.color[i].b);
                vdata.push_back(mesh_.color[i].a);
            }
            else {
                vdata.push_back(1.f); vdata.push_back(1.f);
                vdata.push_back(1.f); vdata.push_back(1.f);
            }

            // texCoord (2)
            if (hasTexCoord && i < static_cast<uint32_t>(mesh_.texCoord.size())) {
                vdata.push_back(mesh_.texCoord[i].x);
                vdata.push_back(mesh_.texCoord[i].y);
            }
            else {
                vdata.push_back(0.f); vdata.push_back(0.f);
            }

            // normal (3)
            if (hasNormal && i < static_cast<uint32_t>(mesh_.normal.size())) {
                vdata.push_back(mesh_.normal[i].x);
                vdata.push_back(mesh_.normal[i].y);
                vdata.push_back(mesh_.normal[i].z);
            }
            else {
                vdata.push_back(0.f); vdata.push_back(1.f); vdata.push_back(0.f);
            }
        }

        // Verifica che il buffer abbia esattamente vertexCount_ * 12 float
        assert(vdata.size() == static_cast<size_t>(vertexCount_) * 12);

        GLsizeiptr vertexBytes = static_cast<GLsizeiptr>(vdata.size() * sizeof(float));
        GLsizeiptr indexBytes = static_cast<GLsizeiptr>(indexCount_ * sizeof(uint32_t));

        vertexRegion_ = pool_->allocateVertexRegion(vertexBytes);
        pool_->uploadVertexData(vertexRegion_, vdata.data(), vertexBytes);

        if (indexCount_ > 0) {
            indexRegion_ = pool_->allocateIndexRegion(indexBytes);
            pool_->uploadIndexData(indexRegion_, mesh_.indices.data(), indexBytes);
        }

        uploaded_ = true;
    }

        std::cout << "[PooledMeshGL] Uploaded " << vertexCount_ << " verts, "
            << indexCount_ << " idx | vOffset=" << vertexRegion_.offset
            << " iOffset=" << indexRegion_.offset
            << " baseVertex=" << getBaseVertex()
            << " firstIndex=" << getFirstIndex() << "\n";
    }

    // -------------------------------------------------------------------------
    // drawInstanced — draw diretta (es. skybox), richiede VAO bindato
    // -------------------------------------------------------------------------
    void PooledMeshGL::drawInstanced(uint32_t instanceCount) {
        if (!uploaded_) return;

        if (indexCount_ > 0) {
            if (instanceCount <= 1) {
                glDrawElementsBaseVertex(
                    GL_TRIANGLES,
                    static_cast<GLsizei>(indexCount_),
                    GL_UNSIGNED_INT,
                    // byte offset nel IBO dal INIZIO del buffer
                    reinterpret_cast<void*>(static_cast<uintptr_t>(indexRegion_.offset)),
                    getBaseVertex()
                );
            }
            else {
                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    static_cast<GLsizei>(indexCount_),
                    GL_UNSIGNED_INT,
                    reinterpret_cast<void*>(static_cast<uintptr_t>(indexRegion_.offset)),
                    static_cast<GLsizei>(instanceCount),
                    getBaseVertex(),
                    0 // baseInstance
                );
            }
        }
        else {
            glDrawArrays(GL_TRIANGLES,
                getBaseVertex(),
                static_cast<GLsizei>(vertexCount_));
        }
        (void)instanceCount; // TODO: istanze con glDrawElementsInstancedBaseVertex
    }

    void PooledMeshGL::shutdown() {
        if (uploaded_) {
            pool_->freeRegion(vertexRegion_);
            if (indexCount_ > 0) pool_->freeRegion(indexRegion_);
            uploaded_ = false;
        }
    }

} // namespace OnYuu