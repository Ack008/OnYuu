#pragma once
#include <glad/glad.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Core/Material.h"
#include "OpenGLGeometryPool.h"

namespace OnYuu {

    /**
     * Struttura che va direttamente nel Draw Indirect Buffer (layout standard OpenGL).
     * Usata con glMultiDrawElementsIndirect.
     */
    struct GLDrawElementsIndirectCommand {
        GLuint count;           // numero di indici
        GLuint instanceCount;
        GLuint firstIndex;      // offset in unità di indici nel IBO
        GLint  baseVertex;      // offset in unità di vertici nel VBO
        GLuint baseInstance;    // primo instanceID
    };

    /**
     * OpenGLIndirectBuffer
     *
     * Gestisce un GL_DRAW_INDIRECT_BUFFER per un singolo bucket
     * (es. materiale+topologia) e offre multi-draw in una sola call.
     *
     * Mappa la logica di IndirectDrawBuffer da Vulkan su OpenGL:
     *   - CPU-side: std::vector di comandi riempito ogni frame
     *   - GPU-side: GL_DRAW_INDIRECT_BUFFER aggiornato con glBufferSubData
     *   - Esecuzione: glMultiDrawElementsIndirect (una sola drawcall)
     */
    class OpenGLIndirectBuffer {
    public:
        explicit OpenGLIndirectBuffer(uint32_t maxDraws = 10000);
        ~OpenGLIndirectBuffer();

        // Non copiabile
        OpenGLIndirectBuffer(const OpenGLIndirectBuffer&) = delete;
        OpenGLIndirectBuffer& operator=(const OpenGLIndirectBuffer&) = delete;

        // Accumula un comando prima di finalize()
        void addDrawCommand(const GLDrawElementsIndirectCommand& cmd);

        // Carica i comandi CPU->GPU (chiama ogni frame prima del draw)
        void finalize();

        // Svuota i comandi per il frame successivo
        void reset();

        // Esegue glMultiDrawElementsIndirect — richiede VAO + shader già bindati
        void executeMultiDraw(GLenum primitiveMode = GL_TRIANGLES);

        uint32_t getDrawCount() const { return currentDrawCount_; }
        bool     isEmpty()      const { return currentDrawCount_ == 0; }

        void shutdown();

    private:
        GLuint   dibo_ = 0;       // Draw Indirect Buffer Object
        uint32_t maxDrawCommands_ = 0;
        uint32_t currentDrawCount_ = 0;

        std::vector<GLDrawElementsIndirectCommand> cpuCommands_;
        bool needsUpdate_ = false;
    };

    /**
     * Key per il bucket (sceneIndex + Material* + topologia)
     * Identica alla SceneMaterialKey Vulkan.
     */
    struct GLSceneMaterialKey {
        int sceneIndex = -1;
        std::shared_ptr<Material> material;
        GLenum topology = GL_TRIANGLES;

        bool operator==(const GLSceneMaterialKey& o) const {
            return sceneIndex == o.sceneIndex
                && material.get() == o.material.get()
                && topology == o.topology;
        }
    };

    struct GLSceneMaterialKeyHash {
        size_t operator()(const GLSceneMaterialKey& k) const {
            size_t h1 = std::hash<int>{}(k.sceneIndex);
            size_t h2 = std::hash<void*>{}(k.material.get());
            size_t h3 = std::hash<uint32_t>{}(static_cast<uint32_t>(k.topology));
            size_t h = h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            return      h ^ (h3 + 0x9e3779b9 + (h << 6) + (h >> 2));
        }
    };

    /**
     * OpenGLIndirectDrawManager
     *
     * Corrisponde a IndirectDrawManager Vulkan.
     * Mantiene un OpenGLIndirectBuffer per ogni bucket scena/materiale/topologia.
     */
    class OpenGLIndirectDrawManager {
    public:
        OpenGLIndirectDrawManager() = default;
        ~OpenGLIndirectDrawManager();

        // Restituisce (o crea) il buffer per il bucket indicato
        std::shared_ptr<OpenGLIndirectBuffer> getOrCreateBuffer(
            int sceneIndex,
            std::shared_ptr<Material> material,
            GLenum topology = GL_TRIANGLES
        );

        void resetAll();
        void finalizeAll();
        void shutdown();

    private:
        std::unordered_map<GLSceneMaterialKey,
            std::shared_ptr<OpenGLIndirectBuffer>,
            GLSceneMaterialKeyHash> buffers_;
    };

} // namespace OnYuu