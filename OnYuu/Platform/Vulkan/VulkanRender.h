#pragma once
#ifdef _BEBUG
#define LOG(X) std::cout << X << std::endl;
#else
#define LOG(X)
#endif
#include "VulkanShader.h"
#include "Render/BatchRenderer.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandManager.h"
#include "VulkanSyncManager.h"
#include "VulkanDescriptorManager.h"
#include "VulkanPipelineManager.h"
#include "vulkan-bts/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <unordered_map>
#include "VulkanBufferPool.h"

namespace OnYuu {

    /**
     * VulkanRender - Classe principale renderer Vulkan (REFACTORED)
     * Responsabilità: Orchestrazione dei delegate, rendering loop, gestione risorse alto livello
     */
    class VulkanRender : public BatchRender {
    public:
        VulkanRender();
        ~VulkanRender() ;

        // Non copiabile
        VulkanRender(const VulkanRender&) = delete;
        VulkanRender& operator=(const VulkanRender&) = delete;

        // Override BatchRender interface
        void BeginFrame() override;
        void submit() override;
        void invalidateShader(const std::shared_ptr<Shader>& shader) override;
        void invalidateMaterial(const std::shared_ptr<Material>& material) override;
        void Shutdown() override;
        void setSkyBox(SkyBoxComponent* skybox) override;
        void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) override;

        // Accessors per i delegate (per uso esterno se necessario)
        VkInstance getVkInstance() const { return instance_.instance; }
        VulkanDevice* getDevice() const { return device_.get(); }
        VulkanSwapchain* getSwapchain() const { return swapchain_.get(); }
        VulkanCommandManager* getCommandManager() const { return commandManager_.get(); }
        VulkanSyncManager* getSyncManager() const { return syncManager_.get(); }
        VulkanDescriptorManager* getDescriptorManager() const { return descriptorManager_.get(); }
        VulkanPipelineManager* getPipelineManager() const { return pipelineManager_.get(); }

        VmaAllocator getAllocator() const { return allocator_; }
        VkRenderPass getRenderPass() const { return renderPass_; }

        // Restituisce la queue family index usata per la graphics queue
        uint32_t getQueueFamily() const {
            // Assumendo che VulkanDevice abbia un metodo getGraphicsQueueFamily()
            // Se non esiste, sostituire con il modo corretto per ottenere il queue family index
            return device_ ? device_->getGraphicsQueueFamily() : 0;
        }

        VkQueue getGraphicQueue() const { return device_->getGraphicsQueue(); }
        VkFormat getDepthFormat() const { return depthFormat_; }

        uint32_t getCurrentFrame() const { return currentFrame_; }
        void removeCachedMesh(const std::shared_ptr<Mesh>& mesh);
        // Struttura Init per compatibilità con getInit()
        struct InitData {
            VkDevice device;
            struct {
                uint32_t image_count;
                VkFormat image_format;
            } swapchain;
        };

        

        // Restituisce i dati di inizializzazione richiesti da ImGuiLayer
        InitData getInit() const {
            InitData data{};
            data.device = device_ ? device_->getDevice() : VK_NULL_HANDLE;
            if (swapchain_) {
                data.swapchain.image_count = swapchain_->getImageCount();
                data.swapchain.image_format = swapchain_->getFormat();
            }
            return data;
        }


        struct RenderData {

        };
        // Helper per operazioni single-time
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer cmd);

    private:
        // ========================================================================
        // DELEGATES - Gestiscono aree specifiche di responsabilità
        // ========================================================================
        std::unique_ptr<VulkanDevice> device_;
        std::unique_ptr<VulkanSwapchain> swapchain_;
        std::unique_ptr<VulkanCommandManager> commandManager_;
        std::unique_ptr<VulkanSyncManager> syncManager_;
        std::unique_ptr<VulkanDescriptorManager> descriptorManager_;
        std::unique_ptr<VulkanPipelineManager> pipelineManager_;

        // ========================================================================
        // CORE VULKAN OBJECTS (gestiti direttamente)
        // ========================================================================
        vkb::Instance instance_;
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;
        VmaAllocator allocator_ = VK_NULL_HANDLE;
        VkRenderPass renderPass_ = VK_NULL_HANDLE;

        // Depth resources
        VkImage depthImage_ = VK_NULL_HANDLE;
        VkImageView depthImageView_ = VK_NULL_HANDLE;
        VmaAllocation depthAllocation_ = VK_NULL_HANDLE;
        VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;
        VkFormat activeColorFormat_ = VK_FORMAT_UNDEFINED;
        VkFormat activeDepthFormat_ = VK_FORMAT_UNDEFINED;

        // Window
        GLFWwindow* window_ = nullptr;

        // ========================================================================
        // FRAME MANAGEMENT
        // ========================================================================
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t currentFrame_ = 0;
        uint32_t imageIndex_ = 0;
		uint32_t frameNumber = 0;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        bool swapchainNeedsRecreate_ = false;
        bool isFrameRecording_ = false; // ? Traccia se il command buffer è in recording

        // ========================================================================
        // DESCRIPTOR SET LAYOUTS (condivisi tra frame)
        // ========================================================================
        VkDescriptorSetLayout globalDescriptorLayout_ = VK_NULL_HANDLE;
        VkDescriptorSetLayout materialDescriptorLayout_ = VK_NULL_HANDLE;

        // ========================================================================
        // PER-SCENE RESOURCES
        // ========================================================================
        struct SceneResources {
            std::vector<VkDescriptorSet> globalDescriptorSets; // uno per frame
            std::vector<std::shared_ptr<class VulkanUniformBuffer>> cameraUbos;
            std::vector<std::shared_ptr<class VulkanUniformBuffer>> lightUbos;
            std::vector<std::shared_ptr<class VulkanStorageBuffer>> modelMatrixSsbos;
        };
        std::unordered_map<int, SceneResources> sceneResources_;
        std::unordered_map<std::shared_ptr<RenderTarget>, std::vector<int>> sceneTarget;
		std::vector<int> swapChainRenderedScenes;

        // ========================================================================
        // PER-MATERIAL RESOURCES
        // ========================================================================
        struct MaterialResources {
            std::vector<VkDescriptorSet> descriptorSets; // uno per frame
            std::vector<std::shared_ptr<class VulkanUniformBuffer>> ubos;
        };
        std::unordered_map<std::shared_ptr<class Material>, MaterialResources> materialResources_;

        // Deferred deletion per materiali invalidati durante il frame
        std::vector<std::shared_ptr<class Material>> pendingMaterialInvalidations_;


        // ========================================================================
		// PER -SHADER RESOURCES ( overriding materiali, se necessario)
        // ========================================================================
        struct MaterialIndex {
            uint32_t index; 
            size_t textureCount; // Numero di texture usate dal materiale, per gestione array di sampler
		};
        struct ShaderResources {
			std::vector<VkDescriptorSet> materialDescriptorSets; // uno per frame
			std::vector<std::shared_ptr<VulkanStorageBuffer>> materials; // uno per frame
			std::unordered_map<std::shared_ptr<class Material>, MaterialIndex, std::hash<std::shared_ptr<class Material>>> materialIndices; // Mappa per tracciare l'indice di ogni materiale nello storage buffer dello shader
            
		};

		std::unordered_map<std::shared_ptr<class Shader>, ShaderResources> shaderResources_;

        std::vector<uint8_t> getMaterialDataForShader(const std::shared_ptr<Material>& material, const std::shared_ptr<VulkanShader>& shader);
        bool isOneMaterialDirty(std::vector<std::shared_ptr<Material>> materials);
        // ========================================================================
        // PIPELINE CACHE
        // ========================================================================
        struct PipelineKey {
            std::shared_ptr<class Shader> shader;
            RenderingTypeEnum renderingType;
            VkFormat colorFormat;
            VkFormat depthFormat;

            bool operator==(const PipelineKey& other) const {
                return shader.get() == other.shader.get() &&
                    renderingType == other.renderingType &&
                    colorFormat == other.colorFormat &&
                    depthFormat == other.depthFormat;
            }
        };

        struct PipelineKeyHash {
            size_t operator()(const PipelineKey& k) const {
                size_t h1 = std::hash<void*>{}(k.shader.get());
                size_t h2 = std::hash<int>{}(static_cast<int>(k.renderingType));
                size_t h3 = std::hash<int>{}(static_cast<int>(k.colorFormat));
                size_t h4 = std::hash<int>{}(static_cast<int>(k.depthFormat));

                // Combine hashes
                size_t hash = h1;
                hash ^= h2 + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                hash ^= h3 + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                hash ^= h4 + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                return hash;
            }
        };

        std::unordered_map<PipelineKey, VkPipeline, PipelineKeyHash> pipelineCache_;

        // ========================================================================
        // GEOMETRY & INDIRECT DRAW SYSTEM
        // ========================================================================
        std::shared_ptr<class GeometryPool> geometryPool_;
        std::shared_ptr<class IndirectDrawManager> indirectDrawManager_;

        struct MeshDrawInfo {
            BufferRegion vertexRegion;
            BufferRegion indexRegion;
            uint32_t indexCount;
            uint32_t vertexCount;
            uint32_t firstIndex;
            int32_t vertexOffset;
        };
        std::unordered_map<std::shared_ptr<class Mesh>, MeshDrawInfo> meshDrawInfo_;
        std::unordered_map<std::shared_ptr<class Mesh>, std::shared_ptr<class PooledMeshGPU>> pooledMeshes_;

        // ========================================================================
        // UBO DATA STRUCTURES
        // ========================================================================
        struct LightData {
            alignas(16) glm::vec3 position;
            alignas(16) glm::vec3 color;
            alignas(16) float intensity;
        };

        struct LightBufferData {
            alignas(16) int count;
            alignas(16) LightData lights[125];
        };

        struct CameraBufferData {   
            alignas(16) glm::mat4 projection;
            alignas(16) glm::mat4 view;
            alignas(16) glm::vec4 cameraPosition;
        };

        struct ModelMatrixData {
            alignas(16) glm::mat4 model;
			 uint32_t materialIndex; // Indice del materiale nello storage buffer dello shader
			 uint32_t textureCount; // Numero di texture usate dal materiale, per gestione array di sampler
			//float padding[3]; // Padding per allineamento a 16 byte

        };

        // ========================================================================
        // INITIALIZATION METHODS
        // ========================================================================
        bool initializeInstance();
        bool initializeSurface();
        bool initializeAllocator();
        bool createRenderPass();
        bool createDepthResources();
        bool createDescriptorLayouts();

        // ========================================================================
        // RENDERING METHODS
        // ========================================================================
        void updateAllDescriptorSets();
        void updateMaterialsData();
        void updateSceneDescriptors(int sceneIndex);
        void updateMaterialDescriptors(std::shared_ptr<class Material> material);

		void beginRendering(VkCommandBuffer cmd, VkImage colorImage, VkImageView colorView, VkImage depthImage, VkImageView depthView, VkExtent2D extent, VkFormat depthFormat, bool isSwapchain = true, VkImageLayout colorOldLayout = VK_IMAGE_LAYOUT_UNDEFINED);
		void beginRenderPass(VkCommandBuffer cmd);
		void endRenderPass(VkCommandBuffer cmd);
		void endRendering(VkCommandBuffer cmd, VkImage colorImage, bool isSwapchain = true);
        void renderScene(VkCommandBuffer cmd, int sceneIndex);

        // Pipeline management
        VkPipeline getOrCreatePipeline(const PipelineKey& key);

        // ========================================================================
        // HELPER METHODS
        // ========================================================================
        VkFormat findDepthFormat();
        void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

        // Cleanup
        void cleanupDepthResources();
        void cleanupDescriptorLayouts();

        // Swapchain recreation
        bool recreateSwapchainResources();
        bool hasFramebufferResize() const;
    };

} // namespace OnYuu