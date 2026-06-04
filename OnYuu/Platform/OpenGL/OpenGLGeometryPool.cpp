#include "OpenGLGeometryPool.h"
#include "OpenGLBatchRender.h"
#include <algorithm>
#include <iostream>
#include <cstring>

namespace OnYuu {

    // ============================================================================
    // Costanti layout vertex (devono combaciare con setupVAO)
    //   pos3 (12) + col4 (16) + uv2 (8) + norm3 (12) = 48 byte
    // ============================================================================
    static constexpr GLsizei VERTEX_STRIDE = 12 * sizeof(float); // 48 byte
    static constexpr GLintptr POS_OFFSET = 0;
    static constexpr GLintptr COL_OFFSET = 3 * sizeof(float);
    static constexpr GLintptr UV_OFFSET = 7 * sizeof(float);
    static constexpr GLintptr NORM_OFFSET = 9 * sizeof(float);

    // ============================================================================
    // OpenGLGeometryPool
    // ============================================================================

    OpenGLGeometryPool::OpenGLGeometryPool(GLsizeiptr initialVertexBytes,
        GLsizeiptr initialIndexBytes)
        : vertexBufferSize_(initialVertexBytes)
        , indexBufferSize_(initialIndexBytes)
    {
        // Crea VBO unificato (immutabile se GL 4.4, altrimenti DYNAMIC_DRAW)
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize_, nullptr, GL_DYNAMIC_DRAW);

        // Crea IBO unificato
        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize_, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // VAO globale con layout fisso
        setupVAO();

        std::cout << "[OpenGLGeometryPool] Created: VBO=" << vertexBufferSize_
            << " IBO=" << indexBufferSize_ << " bytes\n";
    }

    OpenGLGeometryPool::~OpenGLGeometryPool() {
        shutdown();
    }

    void OpenGLGeometryPool::setupVAO() {
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

        // attrib 0: position (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(POS_OFFSET));

        // attrib 1: color (vec4)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(COL_OFFSET));

        // attrib 2: texCoord (vec2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(UV_OFFSET));

        // attrib 3: normal (vec3)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
            reinterpret_cast<void*>(NORM_OFFSET));

        glBindVertexArray(0);
    }

    void OpenGLGeometryPool::bind() const {
        glBindVertexArray(vao_);
    }

    void OpenGLGeometryPool::unbind() const {
        glBindVertexArray(0);
    }

    // ------------------------------------------------------------------ alloca
    GLBufferRegion OpenGLGeometryPool::allocateVertexRegion(GLsizeiptr size) {
        // Allinea a VERTEX_STRIDE per sicurezza
        size = (size + VERTEX_STRIDE - 1) & ~(VERTEX_STRIDE - 1);

        std::lock_guard<std::mutex> lock(trackerMutex_);

        // Cerca nella free-list (first-fit)
        for (auto it = vertexFreeList_.begin(); it != vertexFreeList_.end(); ++it) {
            if (it->size >= size) {
                GLBufferRegion region{ vbo_, it->offset, size, false };
                it->offset += size;
                it->size -= size;
                if (it->size == 0) vertexFreeList_.erase(it);
                return region;
            }
        }

        // Alloca dalla fine
        if (vertexUsedSize_ + size > vertexBufferSize_) {
            growVertexBuffer(std::max(vertexBufferSize_ * 2, vertexUsedSize_ + size));
        }

        GLBufferRegion region{ vbo_, vertexUsedSize_, size, false };
        vertexUsedSize_ += size;
        return region;
    }

    GLBufferRegion OpenGLGeometryPool::allocateIndexRegion(GLsizeiptr size) {
        // Allinea a 4 byte (sizeof uint32_t)
        size = (size + 3) & ~3;

        std::lock_guard<std::mutex> lock(trackerMutex_);

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
            growIndexBuffer(std::max(indexBufferSize_ * 2, indexUsedSize_ + size));
        }

        GLBufferRegion region{ ibo_, indexUsedSize_, size, true };
        indexUsedSize_ += size;
        return region;
    }

    // ------------------------------------------------------------------ upload
    void OpenGLGeometryPool::uploadVertexData(const GLBufferRegion& region,
        const void* data, GLsizeiptr size) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, region.offset, size, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLGeometryPool::uploadIndexData(const GLBufferRegion& region,
        const void* data, GLsizeiptr size) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, region.offset, size, data);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // ------------------------------------------------------------------ free
    void OpenGLGeometryPool::freeRegion(const GLBufferRegion& region) {
        std::lock_guard<std::mutex> lock(trackerMutex_);

        if (!region.isIndex) {
            vertexFreeList_.push_back({ region.offset, region.size });
            std::sort(vertexFreeList_.begin(), vertexFreeList_.end(),
                [](const FreeBlock& a, const FreeBlock& b) { return a.offset < b.offset; });
            // Merge blocchi adiacenti
            for (size_t i = 0; i + 1 < vertexFreeList_.size(); ) {
                auto& cur = vertexFreeList_[i];
                auto& next = vertexFreeList_[i + 1];
                if (cur.offset + cur.size == next.offset) {
                    cur.size += next.size;
                    vertexFreeList_.erase(vertexFreeList_.begin() + static_cast<ptrdiff_t>(i) + 1);
                }
                else { ++i; }
            }
        }
        else {
            indexFreeList_.push_back({ region.offset, region.size });
            std::sort(indexFreeList_.begin(), indexFreeList_.end(),
                [](const FreeBlock& a, const FreeBlock& b) { return a.offset < b.offset; });
            for (size_t i = 0; i + 1 < indexFreeList_.size(); ) {
                auto& cur = indexFreeList_[i];
                auto& next = indexFreeList_[i + 1];
                if (cur.offset + cur.size == next.offset) {
                    cur.size += next.size;
                    indexFreeList_.erase(indexFreeList_.begin() + static_cast<ptrdiff_t>(i) + 1);
                }
                else { ++i; }
            }
        }
    }

    // ------------------------------------------------------------------ grow
    void OpenGLGeometryPool::growVertexBuffer(GLsizeiptr newSize) {
        std::cout << "[OpenGLGeometryPool] Growing VBO " << vertexBufferSize_
            << " -> " << newSize << " bytes\n";

        // Leggi dati esistenti, crea nuovo buffer più grande, ricopia
        std::vector<uint8_t> tmp(static_cast<size_t>(vertexUsedSize_));
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexUsedSize_, tmp.data());

        glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
        if (vertexUsedSize_ > 0)
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertexUsedSize_, tmp.data());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        vertexBufferSize_ = newSize;

        // Il VAO mantiene il binding al VBO, ma il puntatore glVertexAttribPointer
        // rimane valido perché il buffer handle non cambia.
    }

    void OpenGLGeometryPool::growIndexBuffer(GLsizeiptr newSize) {
        std::cout << "[OpenGLGeometryPool] Growing IBO " << indexBufferSize_
            << " -> " << newSize << " bytes\n";

        std::vector<uint8_t> tmp(static_cast<size_t>(indexUsedSize_));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexUsedSize_, tmp.data());

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
        if (indexUsedSize_ > 0)
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexUsedSize_, tmp.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        indexBufferSize_ = newSize;
    }

    // ------------------------------------------------------------------ LRU
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
        else {
            std::cerr << "[OpenGLGeometryPool] WARNING: Mesh used but not registered!\n";
            auto& info = meshUsageTracker_[mesh];
            info.lastUsedFrame = currentFrame;
            info.refCount = 1;
        }
    }

    void OpenGLGeometryPool::collectGarbage(uint64_t currentFrame,
        uint32_t framesToKeep,
        OpenGLBatchRender* renderer) {
        std::vector<std::shared_ptr<Mesh>> toDelete;

        {
            std::lock_guard<std::mutex> lock(trackerMutex_);
            for (auto& [mesh, info] : meshUsageTracker_) {
                uint64_t age = currentFrame - info.lastUsedFrame;
                if (age > framesToKeep && !info.markedForDeletion) {
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
        }
    }

    void OpenGLGeometryPool::shutdown() {
        if (vao_) { glDeleteVertexArrays(1, &vao_); vao_ = 0; }
        if (vbo_) { glDeleteBuffers(1, &vbo_);      vbo_ = 0; }
        if (ibo_) { glDeleteBuffers(1, &ibo_);      ibo_ = 0; }
        meshUsageTracker_.clear();
    }

    // ============================================================================
    // PooledMeshGL
    // ============================================================================

    PooledMeshGL::PooledMeshGL(Mesh& mesh, std::shared_ptr<OpenGLGeometryPool> pool)
        : mesh_(mesh), pool_(pool) {
    }

    PooledMeshGL::~PooledMeshGL() { shutdown(); }

    void PooledMeshGL::uploadToGPU() {
        if (uploaded_ || mesh_.empty()) return;

        vertexCount_ = static_cast<uint32_t>(mesh_.position.size());
        indexCount_ = static_cast<uint32_t>(mesh_.indices.size());

        // Prepara vertex data interlacciati (pos3 col4 uv2 norm3)
        std::vector<float> vertexData;
        vertexData.reserve(vertexCount_ * 12);

        for (size_t i = 0; i < vertexCount_; i++) {
            vertexData.push_back(mesh_.position[i].x);
            vertexData.push_back(mesh_.position[i].y);
            vertexData.push_back(mesh_.position[i].z);

            if (i < mesh_.color.size()) {
                vertexData.push_back(mesh_.color[i].r);
                vertexData.push_back(mesh_.color[i].g);
                vertexData.push_back(mesh_.color[i].b);
                vertexData.push_back(mesh_.color[i].a);
            }
            else {
                vertexData.insert(vertexData.end(), { 1.f, 1.f, 1.f, 1.f });
            }

            if (i < mesh_.texCoord.size()) {
                vertexData.push_back(mesh_.texCoord[i].x);
                vertexData.push_back(mesh_.texCoord[i].y);
            }
            else {
                vertexData.insert(vertexData.end(), { 0.f, 0.f });
            }

            if (i < mesh_.normal.size()) {
                vertexData.push_back(mesh_.normal[i].x);
                vertexData.push_back(mesh_.normal[i].y);
                vertexData.push_back(mesh_.normal[i].z);
            }
            else {
                vertexData.insert(vertexData.end(), { 0.f, 1.f, 0.f });
            }
        }

        GLsizeiptr vertexBytes = static_cast<GLsizeiptr>(vertexData.size() * sizeof(float));
        GLsizeiptr indexBytes = static_cast<GLsizeiptr>(indexCount_ * sizeof(uint32_t));

        vertexRegion_ = pool_->allocateVertexRegion(vertexBytes);
        pool_->uploadVertexData(vertexRegion_, vertexData.data(), vertexBytes);

        if (indexCount_ > 0) {
            indexRegion_ = pool_->allocateIndexRegion(indexBytes);
            pool_->uploadIndexData(indexRegion_, mesh_.indices.data(), indexBytes);
        }

        uploaded_ = true;
    }

    PooledMeshGL::DrawCommand PooledMeshGL::buildDrawCommand(uint32_t instanceCount,
        uint32_t baseInstance) const {
        return DrawCommand{
            indexCount_,
            instanceCount,
            getFirstIndex(),
            getBaseVertex(),
            baseInstance
        };
    }

    void PooledMeshGL::drawInstanced(uint32_t instanceCount, uint32_t /*baseInstance*/) {
        if (!uploaded_) return;

        if (indexCount_ > 0) {
            // glDrawElementsBaseVertex usa byte offset per gli indici
            glDrawElementsBaseVertex(
                GL_TRIANGLES,
                static_cast<GLsizei>(indexCount_),
                GL_UNSIGNED_INT,
                reinterpret_cast<void*>(static_cast<uintptr_t>(indexRegion_.offset)),
                getBaseVertex()
            );
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