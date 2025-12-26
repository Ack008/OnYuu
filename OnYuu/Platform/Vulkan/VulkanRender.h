#pragma once
#include "Render/BatchRenderer.h"
#include "vulkan-bts/VkBootstrap.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "vma/vk_mem_alloc.h"
#include "Render/Renderer.h"
#include <GLFW/glfw3.h>
#include "VulkanMeshGPU.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Core/Material.h"
#include <functional> // aggiunto per std::hash
namespace OnYuu {

    class VulkanRender : public BatchRender
    {
    public:
        VulkanRender();
        ~VulkanRender();
        virtual void BeginFrame() override;
        void updateAllDescriptorDSet();
        virtual void submit() override;
        virtual void Shutdown() override;
        void setSkyBox(SkyBoxComponent* skybox) override;
        void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) override;
    private:
        struct Init {
            vkb::Instance instance;
            vkb::InstanceDispatchTable inst_disp;
            VkSurfaceKHR surface;
            vkb::Device device;
            vkb::DispatchTable disp;
            vkb::Swapchain swapchain;
        };
        struct DescriptorSetInfo {
            VkDescriptorSetLayout descriptorSetLayout;
            VkDescriptorSet descriptorSet;
        };
        struct RenderData {
            VkQueue graphics_queue;
            VkQueue present_queue;

            std::vector<VkImage> swapchain_images;
            std::vector<VkImageView> swapchain_image_views;
            std::vector<VkFramebuffer> framebuffers;

            VkRenderPass render_pass;


            VkCommandPool command_pool;
            std::vector<VkCommandBuffer> command_buffers;

            VkDescriptorPool descriptors_pool;

            std::vector<DescriptorSetInfo> global_descriptor;
            std::unordered_map <int, std::vector<DescriptorSetInfo>> scene_map_descriptor;
            std::unordered_map<std::shared_ptr<Material>, std::vector<DescriptorSetInfo>> material_map_descriptor;
            std::vector<DescriptorSetInfo> material_descriptor;

            std::vector<VkSemaphore> available_semaphores;
            std::vector<VkSemaphore> finished_semaphore;
            std::vector<VkFence> in_flight_fences;
            std::vector<VkFence> image_in_flight;
            size_t current_frame = 0;
            uint32_t image_index = 0;
        };
        struct SceneRenderCache {
            std::vector<VkCommandBuffer> secondaryBuffers; // Uno per frame in flight
            bool isDirty = true;
            size_t geometryHash = 0; // Per invalidare cache
        };
        std::unordered_map<int, SceneRenderCache> sceneRenderCaches;
        VkCommandPool secondaryCommandPool;

        typedef std::pair<std::shared_ptr<Shader>, RenderingTypeEnum > pipelineKey;

        // Hash ed equality personalizzate per `pipelineKey` (pair<shared_ptr<Shader>, RenderingTypeEnum>)
        // Necessarie perchè la std::hash per std::pair non è fornita / non è utilizzabile qui.
        struct PipelineKeyHash {
            std::size_t operator()(const pipelineKey& k) const noexcept {
                Shader* raw = k.first ? k.first.get() : nullptr;
                std::size_t h1 = std::hash<Shader*>{}(raw);
                std::size_t h2 = std::hash<int>{}(static_cast<int>(k.second));
                return h1 ^ (h2 << 1);
            }
        };
        struct PipelineKeyEqual {
            bool operator()(const pipelineKey& a, const pipelineKey& b) const noexcept {
                return a.first.get() == b.first.get() && a.second == b.second;
            }
        };


        Init init;
        RenderData data;
        int MAX_FRAMES_IN_FLIGHT = 3;
        GLFWwindow* window = nullptr;
        VmaAllocator allocator = nullptr;
        std::vector<VkPipelineLayout> pipeline_layouts;

        // Single (unique) layouts reused across frames to avoid double-creation / double-destroy issues
        VkDescriptorSetLayout globalDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout materialDescriptorSetLayout = VK_NULL_HANDLE;

        // model data structures
        struct ModelMatrixData {
            glm::mat4 model;
            // Padding per alignment std430 se necessario
        };
        // Hash personalizzato per std::pair<Mesh*, std::shared_ptr<Material>>
        struct MeshMaterialHash {
            std::size_t operator()(const std::pair<Mesh*, std::shared_ptr<Material>>& p) const {
                std::size_t h1 = std::hash<Mesh*>{}(p.first);
                std::size_t h2 = std::hash<void*>{}(p.second.get());
                return h1 ^ (h2 << 1); // Combina gli hash
            }
        };
        // Storage buffer per tutte le model matrices della scena
        std::unordered_map<int, std::vector<std::shared_ptr<VulkanStorageBuffer>>> sceneModelMatricesSSBO;

        // Mappa: (mesh, material) -> range di istanze nell'SSBO
        struct InstanceRange {
            uint32_t firstInstance;
            uint32_t instanceCount;
        };
        std::unordered_map<std::pair<Mesh*, std::shared_ptr<Material>>, InstanceRange, MeshMaterialHash> instanceRanges;

    public:
        Init& getInit() { return init; }
        RenderData& getRenderData() { return data; }
        VmaAllocator getAllocator() { return allocator; }
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    private:
        // ora usa hash ed equal personalizzati
        std::unordered_map<pipelineKey, VkPipeline, PipelineKeyHash, PipelineKeyEqual> pipelines;
        std::unordered_map<Mesh*, VulkanMeshGPU> meshGPUMap;
        std::unordered_map<std::shared_ptr<Material>, std::vector<std::shared_ptr<VulkanUniformBuffer>>> materialUboMap;
        VkSurfaceKHR create_surface_glfw(VkInstance instance, VkAllocationCallbacks* allocator = nullptr);
        int device_initialization(Init& init);
        int create_swapchain(Init& init);
        int get_queues(Init& init, RenderData& data);
        int create_render_pass(Init& init, RenderData& data);
        int create_framebuffers(Init& init, RenderData& data);
        int create_command_pool(Init& init, RenderData& data);
        int create_command_buffers(Init& init, RenderData& data);
        int create_sync_objects(Init& init, RenderData& data);
        int recreate_swapchain(Init& init, RenderData& data);
        int create_allocator(VkPhysicalDevice physical_device, VkDevice device);
        int create_descriptor_pool(Init& init, RenderData& data);
        int create_descriptor_sets(Init& init, RenderData& data);
        int begin_record_command_buffer(Init& init, RenderData& data, uint32_t image_index);
        int create_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index);
        int create_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene);
        int create_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index);
        int create_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index, std::shared_ptr<Material> material);
        int create_model_descriptor_set(Init& init, RenderData& data, uint32_t image_index);
        VkPipeline create_graphics_pipeline(Init& init, RenderData& data, std::shared_ptr<Shader> shader, RenderingTypeEnum renderingType);
        void shut_shaders();
        void update_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index, std::shared_ptr<Material> material);
        void update_light_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene);
        void update_camera_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene);
        void update_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene);
        void create_material_ubo(Init& init, RenderData& data, uint32_t image_index, std::shared_ptr<Material> material);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        size_t calculateSceneHash(int indexScene);
        void shut_mesh_buffers();
    private:
        // Light UBO structures
        struct LightCPUStruct {
            alignas(16) glm::vec3 lightPositions;
            alignas(16) glm::vec3 lightColors;
            alignas(16) float intensity;
        } lightData;
        struct LightBufferStruct {
            alignas(16) int count;
            alignas(16) LightCPUStruct lights[125];
        } lightBufferData;
        // camera UBO structures
        struct CameraBufferStruct {
            alignas(16) glm::mat4 projection;
            alignas(16) glm::mat4 view;
            alignas(16) glm::vec4 cameraPosition; // changed from vec3 -> vec4 to match GLSL vec4
        } cameraBufferData;

        std::unordered_map<int, std::vector<std::shared_ptr<VulkanUniformBuffer>>> sceneLightUbo;
        std::unordered_map<int, std::vector<std::shared_ptr<VulkanUniformBuffer>>> sceneCameraUbo;



        std::vector<std::shared_ptr<VulkanUniformBuffer>> lightUbo;
        std::vector<std::shared_ptr<VulkanUniformBuffer>> cameraUbo;
        void render_scene(VkCommandBuffer command_buffer, int indexScene);
        void get_or_create_pipeline(VulkanRender::pipelineKey& pipelineKey, VkPipeline& pipeline, std::shared_ptr<Material>& material, RenderingTypeEnum renderingType);
        void bindMaterialDescriptorsSet(VkPipeline& pipeline, VkPipelineLayout& bindLayout, int& indexScene, std::shared_ptr<Material>& material, VkCommandBuffer command_buffer, int& retFlag);
    };
}