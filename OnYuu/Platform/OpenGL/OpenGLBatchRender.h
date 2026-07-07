#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Render/BatchRenderer.h"
#include "Core/Shader.h"
#include "OpenGLGeometryPool.h"
#include "OpenGLIndirectDraw.h"
#include "OpenGLBuffer.h"

namespace OnYuu {

    /**
     * OpenGLBatchRender
     *
     * Model matrices per-draw:
     *   Con glMultiDrawElementsIndirect una singola GL call serve N mesh,
     *   quindi non è possibile passare u_model come uniform scalare.
     *   La soluzione è un SSBO (binding point 3) con l'array delle model
     *   matrices, indicizzato nello shader tramite gl_DrawID:
     *
     *   // GLSL (vertex shader)
     *   layout(std430, binding = 3) readonly buffer ModelMatrices {
     *       mat4 u_models[];
     *   };
     *   void main() {
     *       mat4 model = u_models[gl_DrawID];
     *       gl_Position = projection * view * model * vec4(aPos, 1.0);
     *   }
     *
     *   gl_DrawID è disponibile da OpenGL 4.6 o con
     *   GL_ARB_shader_draw_parameters (molto diffuso da GL 4.3+).
     */
    class OpenGLBatchRender : public BatchRender {
    public:
        OpenGLBatchRender();
        ~OpenGLBatchRender();

        OpenGLBatchRender(const OpenGLBatchRender&) = delete;
        OpenGLBatchRender& operator=(const OpenGLBatchRender&) = delete;

        void BeginFrame()    override;
        void submit()        override;
        void Shutdown()      override;
        void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) override;
        void setSkyBox(SkyBoxComponent* skybox) override;
        void registeringCallbacks() override;

        void removeCachedMesh(const std::shared_ptr<Mesh>& mesh);
        void invalidateMaterial(const std::shared_ptr<Material>& material);

        // Binding point SSBO model matrices (deve combaciare con lo shader)
        static constexpr uint32_t MODEL_MATRIX_SSBO_BINDING = 3;

    private:
        // ----------------------------------------------------------------
        // UBO structs
        // ----------------------------------------------------------------
        struct CameraInfo {
            glm::mat4 view;
            glm::mat4 projection;
            glm::vec4 position;
        };

        struct alignas(16) GLLight {
            glm::vec4 position;
            glm::vec3 color;
            float     intensity;
        };

        struct alignas(16) LightInfo {
            alignas(16) int  count;
            int              padding[3];
            alignas(16) GLLight lights[125];
        };

        // ----------------------------------------------------------------
        // Shader program cache key
        // ----------------------------------------------------------------
        struct ShaderCacheKey {
            Shader* shader = nullptr;
            RenderingTypeEnum topology = RenderingTypeEnum::TRIANGLE;
            bool operator==(const ShaderCacheKey& o) const {
                return shader == o.shader && topology == o.topology;
            }
        };
        struct ShaderCacheKeyHash {
            size_t operator()(const ShaderCacheKey& k) const {
                size_t h1 = std::hash<void*>{}(k.shader);
                size_t h2 = std::hash<int>{}(static_cast<int>(k.topology));
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        // ----------------------------------------------------------------
        // Material dirty state
        // ----------------------------------------------------------------
        struct MaterialCache {
            bool dirty = true;
            bool initialized = false;
        };

        // ----------------------------------------------------------------
        // Mesh draw cache
        // ----------------------------------------------------------------
        struct MeshDrawCacheEntry {
            std::shared_ptr<PooledMeshGL>    pooledMesh;
            GLDrawElementsIndirectCommand    drawCmd{};
        };

        // ----------------------------------------------------------------
        // Bucket di draw: una entry per ogni (mesh, scena) nel batch
        //   Le istanze della stessa mesh nello stesso bucket condividono
        //   un solo DrawCommand con instanceCount > 1.
        //   Le model matrices di tutte le istanze vanno nell'SSBO.
        // ----------------------------------------------------------------
        struct DrawInstance {
            std::shared_ptr<Mesh> mesh;
            glm::mat4             model;
        };

        // Per bucket (sceneIndex, materialId, topology): lista istanze ordinate
        // nell'esatto ordine in cui verranno emesse le draw (= ordine nell'SSBO).
        struct BucketKey {
            int         sceneIndex;
            std::string materialId;
            GLenum      topology;
            bool operator==(const BucketKey& o) const {
                return sceneIndex == o.sceneIndex
                    && materialId == o.materialId
                    && topology == o.topology;
            }
        };
        struct BucketKeyHash {
            size_t operator()(const BucketKey& k) const {
                size_t h = std::hash<int>{}(k.sceneIndex);
                h ^= std::hash<std::string>{}(k.materialId) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<uint32_t>{}(k.topology) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };

        // ----------------------------------------------------------------
        // Risorse OpenGL
        // ----------------------------------------------------------------
        std::shared_ptr<OpenGLGeometryPool>        geometryPool_;
        std::shared_ptr<OpenGLIndirectDrawManager> indirectManager_;
        std::shared_ptr<UniformBuffer>             cameraUBO_;
        std::shared_ptr<UniformBuffer>             lightsUBO_;

        // SSBO binding point 3 — una per scena (resize automatico)
        // La stessa istanza viene sovrascritta ad ogni frame con le matrices aggiornate.
        std::shared_ptr<OpenGLStorageBuffer>       modelMatrixSSBO_;

        std::unordered_map<ShaderCacheKey, GLuint, ShaderCacheKeyHash> shaderCache_;
        std::unordered_map<std::shared_ptr<Mesh>, MeshDrawCacheEntry> meshCache_;
        std::unordered_map<std::string, MaterialCache>       materialCache_;
        std::vector<std::string>                                        pendingInvalidations_;

        SkyBoxComponent* skybox_ = nullptr;
        uint64_t         frameNumber_ = 0;

        static constexpr uint32_t GC_FRAMES_TO_KEEP = 180;
        static constexpr uint32_t GC_INTERVAL_FRAMES = 60;
        // Dimensione SSBO iniziale: 4096 mat4 = 256 KB
        static constexpr size_t   SSBO_INITIAL_COUNT = 4096;

        size_t materialModifyObserverID_ = static_cast<size_t>(-1);
        size_t materialDeleteObserverID_ = static_cast<size_t>(-1);
        size_t materialCreateObserverID_ = static_cast<size_t>(-1);

        bool initialized_ = false;

        // ----------------------------------------------------------------
        // Metodi privati
        // ----------------------------------------------------------------
        void lazyInit();

        void renderScene(RenderScene& scene, int sceneIndex);
        void renderSkybox(RenderScene& scene);
        void updateGlobalUBOs(RenderScene& scene);

        MeshDrawCacheEntry& getOrUploadMesh(const std::shared_ptr<Mesh>& mesh);

        /**
         * buildDrawData
         *   Percorre i batch, carica mesh nuove nel pool, popola:
         *     - indirectManager_: draw commands per glMultiDrawElementsIndirect
         *     - modelMatrices (out): array FLAT delle mat4, una per draw,
         *       nell'ordine identico a quello dei draw commands.
         *
         *   L'ordine garantisce che gl_DrawID[i] corrisponda alla matrice [i].
         */
        void buildDrawData(RenderScene& scene, int sceneIndex,
            std::vector<glm::mat4>& modelMatrices);

        void executeDrawBucket(int sceneIndex,
            const std::shared_ptr<Material>& material,
            GLenum topology);

        bool   applyMaterialIfDirty(const std::string& materialId, Material* material);
        GLuint getOrCreateProgram(Shader* shader, RenderingTypeEnum topology);

        void collectGarbage();
        void processPendingInvalidations();

        static GLenum toGLTopology(RenderingTypeEnum t);
		// stastics
		double lastFrameTime_ = 0.0;
		uint32_t indirectDrawCalls_ = 0;
		uint32_t totalBatches_ = 0;
        void resetStats();
    };

} // namespace OnYuu