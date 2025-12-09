#pragma once
#include "Render/BatchRenderer.h"
#include "vulkan-bts/VkBootstrap.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "vma/vk_mem_alloc.h"
#include "Render/Renderer.h"
#include <GLFW/glfw3.h>
#include "VulkanMeshGPU.h"
#include <functional> // aggiunto per std::hash
class VulkanRender : public BatchRender
{
    	public:
	VulkanRender();
	~VulkanRender();
	virtual void BeginFrame() override;
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
		std::vector<DescriptorSetInfo> material_descriptor;
		std::vector<DescriptorSetInfo> model_descriptor;
        
        std::vector<VkSemaphore> available_semaphores;
        std::vector<VkSemaphore> finished_semaphore;
        std::vector<VkFence> in_flight_fences;
        std::vector<VkFence> image_in_flight;
        size_t current_frame = 0;
        uint32_t image_index = 0;
    };
    typedef std::pair<std::shared_ptr<Shader>, RenderingTypeEnum > pipelineKey;

    // Hash ed equality personalizzate per `pipelineKey` (pair<shared_ptr<Shader>, RenderingTypeEnum>)
    // Necessarie perché la std::hash per std::pair non è fornita / non è utilizzabile qui.
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
	static const int MAX_FRAMES_IN_FLIGHT = 2;
	GLFWwindow* window = nullptr;
	VmaAllocator allocator = nullptr;
	std::vector<VkPipelineLayout> pipeline_layouts;
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
	int create_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index);
	int create_model_descriptor_set(Init& init, RenderData& data, uint32_t image_index);
	VkPipeline create_graphics_pipeline(Init& init, RenderData& data, std::shared_ptr<Shader> shader, RenderingTypeEnum renderingType);
    void shut_shaders();
	void update_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index, VulkanShader* material);
	void update_model_descriptor_set(Init& init, RenderData& data, uint32_t image_index, glm::mat4 model);
	void update_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index);
	void shut_mesh_buffers();
private:
	void render_scene(VkCommandBuffer command_buffer, RenderScene& scene);
};