#pragma once
#include "Render/BatchRenderer.h"
#include "vulkan-bts/VkBootstrap.h"
#include <GLFW/glfw3.h>
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

    struct RenderData {
        VkQueue graphics_queue;
        VkQueue present_queue;

        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
        std::vector<VkFramebuffer> framebuffers;

        VkRenderPass render_pass;


        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;

        std::vector<VkSemaphore> available_semaphores;
        std::vector<VkSemaphore> finished_semaphore;
        std::vector<VkFence> in_flight_fences;
        std::vector<VkFence> image_in_flight;
        size_t current_frame = 0;
        uint32_t image_index;
    };
	Init init;
    RenderData data;
	static const int MAX_FRAMES_IN_FLIGHT = 2;
	GLFWwindow* window = nullptr;
public:
	Init& getInit() { return init; }
	RenderData& getRenderData() { return data; }
private:
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
	int begin_record_command_buffer(Init& init, RenderData& data, uint32_t image_index);
};