#pragma once
#include <glad/glad.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "Core/Model/Components/MeshComponent.h"

namespace OnYuu {

    // Rappresenta un range all'interno di un buffer OpenGL
    struct GLBufferRegion {
        GLuint     buffer = 0;
        GLintptr   offset = 0;
        GLsizeiptr size = 0;
        bool       isIndex = false;
    };

    // Tracking LRU per mesh (analogo a Vulkan)
    struct GLMeshUsageInfo {
        uint64_t lastUsedFrame = 0;
        uint32_t refCount = 0;
        bool     markedForDeletion = false;
    };

    // Callback per rimuovere mesh dallo store del renderer
    class OpenGLBatchRender;

    /**
     * Layout vertex IDENTICO a MeshGPUusage (stride VARIABILE per mesh):
     *   pos3 (sempre)
     *   col4 (se mesh.color non vuoto)
     *   uv2  (se mesh.texCoord non vuoto)
     *   norm3 (se mesh.normal non vuoto)
     *
     * Per il pool unificato usiamo stride FISSO 48 byte (12 float) con tutti
     * i canali sempre presenti (padding con valori di default se mancanti).
     * Il VAO viene configurato una volta sola con questo stride.
     *
     * VERTEX ATTRIBUTE LOCATIONS (devono combaciare con gli shader):
     *   0 = position (vec3)
     *   1 = color    (vec4)
     *   2 = texCoord (vec2)
     *   3 = normal   (vec3)
     *
     * Due mutex separati per evitare deadlock:
     *   allocMutex_   → protegge alloc/free delle regioni (non rientrante)
     *   trackerMutex_ → protegge meshUsageTracker_ (LRU)
     */
    class OpenGLGeometryPool {
    public:
        static constexpr GLsizei  VERTEX_STRIDE = 12 * sizeof(float); // 48 byte
        static constexpr GLintptr POS_OFFSET = 0;
        static constexpr GLintptr COL_OFFSET = 3 * sizeof(float);
        static constexpr GLintptr UV_OFFSET = 7 * sizeof(float);
        static constexpr GLintptr NORM_OFFSET = 9 * sizeof(float);

        explicit OpenGLGeometryPool(GLsizeiptr initialVertexBytes = 64 * 1024 * 1024,
            GLsizeiptr initialIndexBytes = 16 * 1024 * 1024);
        ~OpenGLGeometryPool();

        // Non copiabile
        OpenGLGeometryPool(const OpenGLGeometryPool&) = delete;
        OpenGLGeometryPool& operator=(const OpenGLGeometryPool&) = delete;

        // Alloca regioni — NON thread-safe rispetto all'upload, chiamare dal main thread
        GLBufferRegion allocateVertexRegion(GLsizeiptr size);
        GLBufferRegion allocateIndexRegion(GLsizeiptr size);

        // Carica dati nella regione
        void uploadVertexData(const GLBufferRegion& region, const void* data, GLsizeiptr size);
        void uploadIndexData(const GLBufferRegion& region, const void* data, GLsizeiptr size);

        // Libera regione (aggiunge alla free-list)
        void freeRegion(const GLBufferRegion& region);

        // LRU tracking (mutex separato da allocMutex_)
        void registerMesh(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame);
        void updateMeshUsage(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame);
        void collectGarbage(uint64_t currentFrame, uint32_t framesToKeep,
            OpenGLBatchRender* renderer);

        GLuint getVBO() const { return vbo_; }
        GLuint getIBO() const { return ibo_; }
        GLuint getVAO() const { return vao_; }

        void bind()   const;
        void unbind() const;

        void shutdown();

    private:
        struct FreeBlock { GLintptr offset; GLsizeiptr size; };

        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLuint ibo_ = 0;

        GLsizeiptr vertexBufferSize_ = 0;
        GLsizeiptr vertexUsedSize_ = 0;
        std::vector<FreeBlock> vertexFreeList_;

        GLsizeiptr indexBufferSize_ = 0;
        GLsizeiptr indexUsedSize_ = 0;
        std::vector<FreeBlock> indexFreeList_;

        // Mutex separati — allocMutex_ NON è rientrante
        std::mutex allocMutex_;
        std::mutex trackerMutex_;

        std::unordered_map<std::shared_ptr<Mesh>, GLMeshUsageInfo> meshUsageTracker_;

        void setupVAO();
        // Grow chiamato SENZA allocMutex_ già tenuto
        void growVertexBuffer(GLsizeiptr newSize);
        void growIndexBuffer(GLsizeiptr newSize);
        // Merge blocchi adiacenti nella free-list (già dentro il lock)
        static void mergeFreeList(std::vector<FreeBlock>& list);
    };

    /**
     * PooledMeshGL
     * Carica una Mesh nel GeometryPool e offre draw con offset.
     */
    class PooledMeshGL {
    public:
        PooledMeshGL(const Mesh& mesh, std::shared_ptr<OpenGLGeometryPool> pool);
        ~PooledMeshGL();

        void uploadToGPU();
        void shutdown();

        // Draw diretta (es. skybox) — richiede VAO già bindato
        void drawInstanced(uint32_t instanceCount = 1);

        bool isUploaded()  const { return uploaded_; }

        bool isUploaded() const { return uploaded_; }
        const Mesh& getMesh() const { return mesh_; }
        const GLBufferRegion& getVertexRegion() const { return vertexRegion_; }
        const GLBufferRegion& getIndexRegion()  const { return indexRegion_; }

        uint32_t getIndexCount()  const { return indexCount_; }
        uint32_t getVertexCount() const { return vertexCount_; }

        // firstIndex: offset in unità di uint32_t nel IBO globale
        GLuint getFirstIndex() const {
            return static_cast<GLuint>(indexRegion_.offset / sizeof(uint32_t));
        }
        // baseVertex: offset in unità di vertici nel VBO globale
        GLint getBaseVertex() const {
            return static_cast<GLint>(
                vertexRegion_.offset / OpenGLGeometryPool::VERTEX_STRIDE);
        }

    private:
        const Mesh& mesh_;
        std::shared_ptr<OpenGLGeometryPool>   pool_;
        GLBufferRegion vertexRegion_{};
        GLBufferRegion indexRegion_{};
        uint32_t indexCount_ = 0;
        uint32_t vertexCount_ = 0;
        bool     uploaded_ = false;
    };

} // namespace OnYuu