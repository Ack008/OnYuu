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
        GLuint buffer = 0;        // VBO/IBO handle
        GLintptr offset = 0;      // byte offset nel buffer
        GLsizeiptr size = 0;      // dimensione in byte
        bool isIndex = false;
    };

    // Tracking LRU per mesh (analogo a Vulkan)
    struct GLMeshUsageInfo {
        uint64_t lastUsedFrame = 0;
        uint32_t refCount = 0;
        bool markedForDeletion = false;
    };

    // Callback per rimuovere mesh dallo store del renderer
    class OpenGLBatchRender;

    /**
     * GeometryPool OpenGL
     *
     * Alloca un unico VBO e un unico IBO grandi, poi sub-alloca regioni per ogni
     * mesh tramite una free-list (equivalente al GeometryPool Vulkan).
     *
     * Richiede OpenGL 4.4+ (glBufferStorage) oppure fallback con glBufferData.
     * Il VAO globale viene creato qui e condiviso; il formato vertex è fisso
     * (pos3 col4 uv2 norm3 = 12 float = 48 byte per vertex).
     */
    class OpenGLGeometryPool {
    public:
        // initialVertexBytes / initialIndexBytes: dimensioni iniziali dei buffer
        explicit OpenGLGeometryPool(GLsizeiptr initialVertexBytes = 64 * 1024 * 1024,
                                    GLsizeiptr initialIndexBytes  = 16 * 1024 * 1024);
        ~OpenGLGeometryPool();

        // Non copiabile
        OpenGLGeometryPool(const OpenGLGeometryPool&) = delete;
        OpenGLGeometryPool& operator=(const OpenGLGeometryPool&) = delete;

        // Alloca regioni (thread-safe tramite mutex)
        GLBufferRegion allocateVertexRegion(GLsizeiptr size);
        GLBufferRegion allocateIndexRegion(GLsizeiptr size);

        // Carica dati nella regione
        void uploadVertexData(const GLBufferRegion& region, const void* data, GLsizeiptr size);
        void uploadIndexData(const GLBufferRegion& region, const void* data, GLsizeiptr size);

        // Libera regione (aggiunge alla free-list)
        void freeRegion(const GLBufferRegion& region);

        // LRU tracking
        void registerMesh(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame);
        void updateMeshUsage(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame);
        void collectGarbage(uint64_t currentFrame, uint32_t framesToKeep,
                            OpenGLBatchRender* renderer);

        GLuint getVBO() const { return vbo_; }
        GLuint getIBO() const { return ibo_; }
        GLuint getVAO() const { return vao_; }

        // Bind VAO + VBO + IBO in un colpo solo (necessario prima di drawcall)
        void bind() const;
        void unbind() const;

        void shutdown();

    private:
        struct FreeBlock { GLintptr offset; GLsizeiptr size; };

        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLuint ibo_ = 0;

        GLsizeiptr vertexBufferSize_ = 0;
        GLsizeiptr vertexUsedSize_   = 0;
        std::vector<FreeBlock> vertexFreeList_;

        GLsizeiptr indexBufferSize_ = 0;
        GLsizeiptr indexUsedSize_   = 0;
        std::vector<FreeBlock> indexFreeList_;

        std::unordered_map<std::shared_ptr<Mesh>, GLMeshUsageInfo> meshUsageTracker_;
        std::mutex trackerMutex_;

        void setupVAO();
        void growVertexBuffer(GLsizeiptr newSize);
        void growIndexBuffer(GLsizeiptr newSize);
    };

    /**
     * PooledMeshGL
     *
     * Equivalente di PooledMeshGPU per OpenGL.
     * Alloca regioni nel pool, carica i dati, e offre draw diretta con offset.
     */
    class PooledMeshGL {
    public:
        PooledMeshGL(Mesh& mesh, std::shared_ptr<OpenGLGeometryPool> pool);
        ~PooledMeshGL();

        void uploadToGPU();
        void shutdown();

        // Disegna usando glDrawElementsBaseVertex con offset nel buffer pooled
        void drawInstanced(uint32_t instanceCount = 1, uint32_t baseInstance = 0);
        // Variante per multi-draw indirect: popola una struct DrawElementsIndirectCommand
        struct DrawCommand {
            GLuint  count;          // numero di indici
            GLuint  instanceCount;
            GLuint  firstIndex;     // offset in unità di indici nel IBO
            GLint   baseVertex;     // offset in unità di vertici nel VBO
            GLuint  baseInstance;
        };
        DrawCommand buildDrawCommand(uint32_t instanceCount = 1,
                                     uint32_t baseInstance  = 0) const;

        bool isUploaded() const { return uploaded_; }
        const Mesh& getMesh() const { return mesh_; }
        const GLBufferRegion& getVertexRegion() const { return vertexRegion_; }
        const GLBufferRegion& getIndexRegion()  const { return indexRegion_; }
        uint32_t getIndexCount()  const { return indexCount_; }
        uint32_t getVertexCount() const { return vertexCount_; }
        // firstIndex in unità di indici (non byte)
        GLuint   getFirstIndex()  const {
            return static_cast<GLuint>(indexRegion_.offset / sizeof(uint32_t));
        }
        // baseVertex in unità di vertici (non byte); 48 = sizeof(Vertex)
        GLint    getBaseVertex()  const {
            return static_cast<GLint>(vertexRegion_.offset / (12 * sizeof(float)));
        }

    private:
        Mesh& mesh_;
        std::shared_ptr<OpenGLGeometryPool> pool_;

        GLBufferRegion vertexRegion_;
        GLBufferRegion indexRegion_;
        uint32_t indexCount_  = 0;
        uint32_t vertexCount_ = 0;
        bool     uploaded_    = false;
    };

} // namespace OnYuu
