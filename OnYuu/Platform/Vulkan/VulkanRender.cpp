#include "VulkanRender.h"
#include "Application/Application.h"
#include "Core/View/View.h"
#include <iostream>
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include <memory>
#include <unordered_map>
#include "Platform/Vulkan/VulkanTexture.h"
#define MAX_SAMPLERS 120
#define CHECK_RESULT(x) \
    if (x != 0) { \
        std::cout << "Failed" << "\n"; \
        std::exit( -1); \
    }
namespace OnYuu {

// Map pipeline -> pipelineLayout per garantire compatibilità al bind dei descriptor sets
static std::unordered_map<VkPipeline, VkPipelineLayout> g_pipeline_layout_map;

VulkanRender::VulkanRender()
    :BatchRender()
{
    window = (GLFWwindow*)Application::getInstance()->getWindow()->getNativeWindow();
    CHECK_RESULT(device_initialization(init));
    CHECK_RESULT(create_allocator(init.device.physical_device, init.device));
    CHECK_RESULT(create_swapchain(init));
    CHECK_RESULT(get_queues(init, data));
	CHECK_RESULT(create_depth_resources(init, data));
    CHECK_RESULT(create_render_pass(init, data));
    CHECK_RESULT(create_framebuffers(init, data));
    CHECK_RESULT(create_command_pool(init, data));
    CHECK_RESULT(create_command_buffers(init, data));
    CHECK_RESULT(create_sync_objects(init, data));
    CHECK_RESULT(create_descriptor_pool(init, data));
    CHECK_RESULT(create_descriptor_sets(init, data));
    // Inizializza gli UBO per luci e camera
    for (int i = 0; i < data.framebuffers.size(); i++)
    {
        lightUbo.push_back(std::make_shared<VulkanUniformBuffer>(2, sizeof(lightBufferData), allocator));
        cameraUbo.push_back(std::make_shared<VulkanUniformBuffer>(0, sizeof(cameraBufferData), allocator));

    }
}



void VulkanRender::setSkyBox(SkyBoxComponent* skybox)
{
    BatchRender::setSkyBox(skybox);
}



void VulkanRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model)
{
    BatchRender::addMeshRender(mesh, model);
}

VkCommandBuffer VulkanRender::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = data.command_pool;
    alloc_info.commandBufferCount = 1;
    VkCommandBuffer command_buffer;
    init.disp.allocateCommandBuffers(&alloc_info, &command_buffer);
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    init.disp.beginCommandBuffer(command_buffer, &begin_info);
    return command_buffer;
}

void VulkanRender::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    init.disp.endCommandBuffer(commandBuffer);
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffer;
    init.disp.queueSubmit(data.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    init.disp.queueWaitIdle(data.graphics_queue);
    init.disp.freeCommandBuffers(data.command_pool, 1, &commandBuffer);
}

VkSurfaceKHR VulkanRender::create_surface_glfw(VkInstance instance, VkAllocationCallbacks* allocator)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkResult err = glfwCreateWindowSurface(instance, window, allocator, &surface);
    if (err) {
        const char* error_msg;
        int ret = glfwGetError(&error_msg);
        if (ret != 0) {
            std::cout << ret << " ";
            if (error_msg != nullptr) std::cout << error_msg;
            std::cout << "\n";
        }
        surface = VK_NULL_HANDLE;
    }
    return surface;
}

int VulkanRender::device_initialization(Init& init)
{

    vkb::InstanceBuilder instance_builder;
    auto instance_ret = instance_builder
        .use_default_debug_messenger()
        .request_validation_layers()
        .build();
    if (!instance_ret) {
        std::cout << instance_ret.error().message() << "\n";
        return -1;
    }
    init.instance = instance_ret.value();

    init.inst_disp = init.instance.make_table();

    init.surface = create_surface_glfw(init.instance);

    vkb::PhysicalDeviceSelector phys_device_selector(init.instance);

    auto phys_device_ret = phys_device_selector
        .set_surface(init.surface)
        .set_minimum_version(1, 2)
        .add_required_extensions(
            {
                "VK_EXT_descriptor_indexing" ,
                "VK_KHR_maintenance3"
            }
        )

        .select();
    if (!phys_device_ret) {
        std::cout << phys_device_ret.error().message() << "\n";
        if (phys_device_ret.error() == vkb::PhysicalDeviceError::no_suitable_device) {
            const auto& detailed_reasons = phys_device_ret.detailed_failure_reasons();
            if (!detailed_reasons.empty()) {
                std::cerr << "GPU Selection failure reasons:\n";
                for (const std::string& reason : detailed_reasons) {
                    std::cerr << reason << "\n";
                }
            }
        }
        return -1;
    }
    VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{};
    indexing_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexing_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    indexing_features.runtimeDescriptorArray = VK_TRUE;
    indexing_features.descriptorBindingPartiallyBound = VK_TRUE;
    indexing_features.descriptorBindingVariableDescriptorCount = VK_TRUE;
    vkb::PhysicalDevice physical_device = phys_device_ret.value();

    vkb::DeviceBuilder device_builder{ physical_device };

    auto device_ret = device_builder
        .add_pNext(&indexing_features)
        .build();
    if (!device_ret) {
        std::cout << device_ret.error().message() << "\n";
        return -1;
    }
    init.device = device_ret.value();

    init.disp = init.device.make_table();

    return 0;
}

int VulkanRender::create_swapchain(Init& init)
{
    vkb::SwapchainBuilder swapchain_builder{ init.device };
    VkSurfaceFormatKHR preferred_surface;
    preferred_surface.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    preferred_surface.format = VK_FORMAT_R8G8B8A8_UNORM;
    std::vector<VkSurfaceFormatKHR> formats;
	formats.push_back(preferred_surface);

    auto swap_ret = swapchain_builder
		.add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_format(preferred_surface)
        .set_desired_min_image_count(3)
        .set_old_swapchain(init.swapchain)
        .build();
    if (!swap_ret) {
        std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
        return -1;
    }
    vkb::destroy_swapchain(init.swapchain);
    init.swapchain = swap_ret.value();
	MAX_FRAMES_IN_FLIGHT = init.swapchain.image_count;
    return 0;
}

int VulkanRender::get_queues(Init& init, RenderData& data)
{
    auto gq = init.device.get_queue(vkb::QueueType::graphics);
    if (!gq.has_value()) {
        std::cout << "failed to get graphics queue: " << gq.error().message() << "\n";
        return -1;
    }
    data.graphics_queue = gq.value();

    auto pq = init.device.get_queue(vkb::QueueType::present);
    if (!pq.has_value()) {
        std::cout << "failed to get present queue: " << pq.error().message() << "\n";
        return -1;
    }
    data.present_queue = pq.value();
    return 0;
}

int VulkanRender::create_render_pass(Init& init, RenderData& data)
{
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = init.swapchain.image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = data.depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // Aggiungi depth attachment reference
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depthAttachment };

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (init.disp.createRenderPass(&render_pass_info, nullptr, &data.render_pass) != VK_SUCCESS) {
        std::cout << "failed to create render pass\n";
        return -1; // failed to create render pass!
    }
    return 0;
}

int VulkanRender::create_framebuffers(Init& init, RenderData& data)
{
    data.swapchain_images = init.swapchain.get_images().value();
    data.swapchain_image_views = init.swapchain.get_image_views().value();

    data.framebuffers.resize(data.swapchain_image_views.size());

    for (size_t i = 0; i < data.swapchain_image_views.size(); i++) {
        std::array<VkImageView, 2> attachments = {
           data.swapchain_image_views[i],
           data.depth_image_view
        };

        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = data.render_pass;
        framebuffer_info.attachmentCount = attachments.size();
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = init.swapchain.extent.width;
        framebuffer_info.height = init.swapchain.extent.height;
        framebuffer_info.layers = 1;

        if (init.disp.createFramebuffer(&framebuffer_info, nullptr, &data.framebuffers[i]) != VK_SUCCESS) {
            return -1; // failed to create framebuffer
        }
    }
    return 0;
}

int VulkanRender::create_command_pool(Init& init, RenderData& data)
{
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = init.device.get_queue_index(vkb::QueueType::graphics).value();

    if (init.disp.createCommandPool(&pool_info, nullptr, &data.command_pool) != VK_SUCCESS) {
        std::cout << "failed to create command pool\n";
        return -1; // failed to create command pool
    }
    return 0;
}

int VulkanRender::create_command_buffers(Init& init, RenderData& data)
{
    data.command_buffers.resize(data.framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = data.command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)data.command_buffers.size();

    if (init.disp.allocateCommandBuffers(&allocInfo, data.command_buffers.data()) != VK_SUCCESS) {
        return -1; // failed to allocate command buffers;
    }

    for (size_t i = 0; i < data.command_buffers.size(); i++) {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (init.disp.beginCommandBuffer(data.command_buffers[i], &begin_info) != VK_SUCCESS) {
            return -1; // failed to begin recording command buffer
        }

        
    }
    return 0;
}

int VulkanRender::create_sync_objects(Init& init, RenderData& data)
{
    data.available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    data.finished_semaphore.resize(init.swapchain.image_count);
    data.in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    data.image_in_flight.resize(init.swapchain.image_count, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < init.swapchain.image_count; i++) {
        if (init.disp.createSemaphore(&semaphore_info, nullptr, &data.finished_semaphore[i]) != VK_SUCCESS) {
            std::cout << "failed to create sync objects\n";
            return -1; // failed to create synchronization objects for a frame
        }
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (init.disp.createSemaphore(&semaphore_info, nullptr, &data.available_semaphores[i]) != VK_SUCCESS ||
            init.disp.createFence(&fence_info, nullptr, &data.in_flight_fences[i]) != VK_SUCCESS) {
            std::cout << "failed to create sync objects\n";
            return -1; // failed to create synchronization objects for a frame
        }
    }
    return 0;
}

int VulkanRender::recreate_swapchain(Init& init, RenderData& data)
{
    init.disp.deviceWaitIdle();

    init.disp.destroyCommandPool(data.command_pool, nullptr);

    for (auto framebuffer : data.framebuffers) {
        init.disp.destroyFramebuffer(framebuffer, nullptr);
    }
    init.disp.destroyImageView(data.depth_image_view, nullptr);
    vmaDestroyImage(allocator, data.depth_image, nullptr);
    init.swapchain.destroy_image_views(data.swapchain_image_views);

    if (0 != create_swapchain(init)) return -1;
	if (0 != create_depth_resources(init, data)) return -1;
    if (0 != create_framebuffers(init, data)) return -1;
    if (0 != create_command_pool(init, data)) return -1;
    if (0 != create_command_buffers(init, data)) return -1;
    return 0;
}

int VulkanRender::create_allocator(VkPhysicalDevice physical_device, VkDevice device)
{
    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.physicalDevice = physical_device;
    allocator_info.device = device;
    allocator_info.instance = init.instance;
    allocator_info.flags = 0;
    if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS)
    {
        std::cout << "failed to create VMA allocator\n";
        return -1;
    }
    return 0;
}

int VulkanRender::create_descriptor_pool(Init& init, RenderData& data)
{
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(init.device, &pool_info, nullptr, &data.descriptors_pool) != VK_SUCCESS)
    {
        std::cout << "failed to create descriptor pool\n";
        return -1; // failed to create descriptor pool
    }
    return 0;
}

int VulkanRender::create_descriptor_sets(Init& init, RenderData& data)
{
    int frame_count = static_cast<int>(MAX_FRAMES_IN_FLIGHT);
    data.global_descriptor.resize(frame_count);
    data.material_descriptor.resize(frame_count);

    // Create single layouts (reuse across frames). The per-frame entries will reference them.
    for (int i = 0; i < frame_count; i++)
    {
        if (create_global_descriptor_set(init, data, i) != 0)
        {
            return -1;
        }
        // Create material descriptor set layout reference for this frame (no allocation of per-material sets yet)
        if (create_material_descriptor_set(init, data, i) != 0)
        {
            return -1;
        }
    }
    return 0;
}

int VulkanRender::begin_record_command_buffer(
    Init& init,
    RenderData& data,
    uint32_t image_index
) {
    VkCommandBuffer cmd = data.command_buffers[data.current_frame];

    // Assicuriamoci che il command buffer non sia in uso:
    // la fence del frame corrente è già stata attesa/reset in BeginFrame,
    // e la fence associata all'immagine (se esiste) è stata attesa prima.
    // È quindi sicuro resettare il command buffer.
    if (vkResetCommandBuffer(cmd, 0) != VK_SUCCESS) {
        std::cerr << "vkResetCommandBuffer failed\n";
        return -1;
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0; // NO SIMULTANEOUS_USE

    if (vkBeginCommandBuffer(cmd, &begin_info) != VK_SUCCESS) {
        std::cerr << "vkBeginCommandBuffer failed\n";
        return -1;
    }

    VkRenderPassBeginInfo rp_info{};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.renderPass = data.render_pass;
    rp_info.framebuffer = data.framebuffers[image_index];
    rp_info.renderArea.offset = { 0, 0 };
    rp_info.renderArea.extent = init.swapchain.extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.2f, 0.2f, 0.1f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    rp_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    rp_info.pClearValues = clearValues.data();
    

    vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{ 0, 0,
        (float)init.swapchain.extent.width,
        (float)init.swapchain.extent.height,
        0.0f, 1.0f };

    VkRect2D scissor{ {0, 0}, init.swapchain.extent };

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    return 0;
}

int VulkanRender::create_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index)
{
    // Create a single descriptor set layout for globals and reuse across frames
    if (globalDescriptorSetLayout == VK_NULL_HANDLE)
    {
        VkDescriptorSetLayoutBinding timeBinding = {};
        timeBinding.binding = 0;
        timeBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        timeBinding.descriptorCount = 1;
        timeBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        timeBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding viewProjBinding = {};
        viewProjBinding.binding = 1;
        viewProjBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        viewProjBinding.descriptorCount = 1;
        viewProjBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        viewProjBinding.pImmutableSamplers = nullptr;
        VkDescriptorSetLayoutBinding lightBinding = {};
        lightBinding.binding = 2;
        lightBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightBinding.descriptorCount = 1;
        lightBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        lightBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding modelMatricesBinding = {};
        modelMatricesBinding.binding = 3;
        modelMatricesBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        modelMatricesBinding.descriptorCount = 1;
        modelMatricesBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelMatricesBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding bindings[] = { timeBinding, viewProjBinding, lightBinding, modelMatricesBinding };
        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 4;
        layout_info.pBindings = bindings;
        if (init.disp.createDescriptorSetLayout(&layout_info, nullptr, &globalDescriptorSetLayout) != VK_SUCCESS)
        {
            std::cout << "failed to create global descriptor set layout\n";
            return -1; // failed to create descriptor set layout
        }
    }
    return 0;

   
}

int VulkanRender::create_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene)
{
    // Allocate descriptor set for this frame using globalDescriptorSetLayout
    VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = data.descriptors_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &globalDescriptorSetLayout;
    if (init.disp.allocateDescriptorSets(&alloc_info, &data.scene_map_descriptor[indexScene][image_index].descriptorSet) != VK_SUCCESS)
    {
        std::cout << "failed to allocate global descriptor set\n";
        return -1;
	}
	// store layout reference
	data.scene_map_descriptor[indexScene][image_index].descriptorSetLayout = globalDescriptorSetLayout;
	// Update descriptor set with UBOs

    return 0;
}

int VulkanRender::create_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index)
{
    // Create a single descriptor set layout for material and reuse across frames
    if (materialDescriptorSetLayout == VK_NULL_HANDLE)
    {
        VkDescriptorSetLayoutBinding materialBinding = {};
        materialBinding.binding = 0;
        materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        materialBinding.descriptorCount = 1;
        materialBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        materialBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding textureBinding = {};
        textureBinding.binding = 1;
        textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureBinding.descriptorCount = MAX_SAMPLERS;
        textureBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        textureBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding bindings[] = { materialBinding, textureBinding };
        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 2;
        layout_info.pBindings = bindings;
        if (init.disp.createDescriptorSetLayout(&layout_info, nullptr, &materialDescriptorSetLayout) != VK_SUCCESS)
        {
            std::cout << "failed to create material descriptor set layout\n";
            return -1; // failed to create descriptor set layout
        }
    }

    // We do not allocate per-material descriptor sets here. We just store the layout reference for the frame.
    data.material_descriptor[image_index].descriptorSetLayout = materialDescriptorSetLayout;
    return 0;
}

int VulkanRender::create_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index, std::shared_ptr<Material> material)
{
    // Ensure index valid
    if (image_index >= data.material_descriptor.size()) {
        std::cerr << "create_material_descriptor_set: invalid image_index\n";
        return -1;
    }

    // Use the single materialDescriptorSetLayout created previously
    VkDescriptorSetLayout layout = data.material_descriptor[image_index].descriptorSetLayout;

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = data.descriptors_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    // allocate into the per-material vector slot
    if (init.disp.allocateDescriptorSets(&alloc_info, &data.material_map_descriptor[material][image_index].descriptorSet) != VK_SUCCESS)
    {
        std::cout << "failed to allocate material descriptor set\n";
        return -1;
    }

    // store layout reference
    data.material_map_descriptor[material][image_index].descriptorSetLayout = layout;

    return 0;
}

int VulkanRender::create_model_descriptor_set(Init& init, RenderData& data, uint32_t image_index)
{

    return 0;
}

int VulkanRender::create_depth_resources(Init& init, RenderData& data)
{
    // Trova un formato depth supportato
    VkFormat candidates[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    data.depthFormat = VK_FORMAT_UNDEFINED;
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(init.device.physical_device, format, &props);
        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            data.depthFormat = format;
            break;
        }
    }

    if (data.depthFormat == VK_FORMAT_UNDEFINED) {
        std::cout << "failed to find supported depth format\n";
        return -1;
    }

    // Crea depth image
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = init.swapchain.extent.width;
    imageInfo.extent.height = init.swapchain.extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = data.depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocation allocation;
    if (vmaCreateImage(allocator, &imageInfo, &allocInfo, &data.depth_image, &data.depth_image_allocation,nullptr) != VK_SUCCESS) {
        std::cout << "failed to create depth image\n";
        return -1;
    }

    // Crea image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = data.depth_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = data.depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (init.disp.createImageView(&viewInfo, nullptr, &data.depth_image_view) != VK_SUCCESS) {
        std::cout << "failed to create depth image view\n";
        return -1;
    }

    return 0;
}


static VkPrimitiveTopology get_vk_primitive_topology(RenderingTypeEnum type) {
    switch (type) {
    case RenderingTypeEnum::TRIANGLE:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case RenderingTypeEnum::TRIANGLE_FAN:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    case RenderingTypeEnum::TRIANGLE_STRIP:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case RenderingTypeEnum::LINE:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    default:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

}
VkPipeline VulkanRender::create_graphics_pipeline(Init& init, RenderData& data, std::shared_ptr<Shader> shader, RenderingTypeEnum renderingType)
{
    VkPipeline pipeline;
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = ((VulkanShader*)shader.get())->getVertexShaderModule();
    vertShaderStageInfo.pName = "main";
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = ((VulkanShader*)shader.get())->getFragmentShaderModule();
    fragShaderStageInfo.pName = "main";
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    bindingDescriptions.push_back({
        0,
        sizeof(glm::vec3) +
        sizeof(glm::vec4) +
        sizeof(glm::vec2)
        + sizeof(glm::vec3),
     VK_VERTEX_INPUT_RATE_VERTEX }
    );
    attributeDescriptions.push_back({
        0,
        0,
        VK_FORMAT_R32G32B32_SFLOAT,
        0
        });
    attributeDescriptions.push_back({
        1,
        0,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        sizeof(glm::vec3)
        });
    attributeDescriptions.push_back({
        2,
        0,
        VK_FORMAT_R32G32_SFLOAT,
        sizeof(glm::vec3) + sizeof(glm::vec4)
        });
    attributeDescriptions.push_back({
        3,
        0,
        VK_FORMAT_R32G32B32_SFLOAT,
        sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec2)
        });
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = get_vk_primitive_topology(renderingType);
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)init.swapchain.extent.width;
    viewport.height = (float)init.swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = init.swapchain.extent;
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    // Aggiungi dopo multisampling e prima di colorBlending
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    // Poi nella VkGraphicsPipelineCreateInfo
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    VkPipelineLayout pipelineLayout;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.push_back(globalDescriptorSetLayout);
    descriptorSetLayouts.push_back(materialDescriptorSetLayout);
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    VkPushConstantRange push_constant;
    push_constant.offset = 0;
    push_constant.size = sizeof(glm::mat4);
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &push_constant;
    if (init.disp.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        std::cout << "failed to create pipeline layout\n";
        throw std::runtime_error("failed to create pipeline layout!");
    }
    pipeline_layouts.push_back(pipelineLayout);
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil; // Aggiungi questa riga
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = data.render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.pDynamicState = &dynamicState; // <-- aggiungi questa riga

    if (init.disp.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        std::cout << "failed to create graphics pipeline\n";
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // memorizza la relazione pipeline -> layout per il corretto bindDescriptorSets
    g_pipeline_layout_map[pipeline] = pipelineLayout;

    return pipeline;
}

void VulkanRender::shut_shaders()
{
    std::vector<std::shared_ptr<Shader>> toFree;
    for (auto [key, ubo] : materialUboMap)
    {
        for (auto& buffer : ubo)
        {
            buffer->shutdown();
        }
    }

    // Non distruggiamo i DescriptorSetLayout qui (evitiamo doppie distruzioni).
    // L'eliminazione dei layout unici sarà fatta in Shutdown() in modo controllato.

    for (auto& pair : pipelines)
    {
        init.disp.destroyPipeline(pair.second, nullptr);
        // controllo che la shader non sia già presente nella lista
        if (std::find(toFree.begin(), toFree.end(), pair.first.first) == toFree.end())
        {
            toFree.push_back(pair.first.first);
        }
    }
    for (auto& shader : toFree)
    {
        shader->shutdown();
    }
    for (auto layout : pipeline_layouts)
    {
        init.disp.destroyPipelineLayout(layout, nullptr);
    }
    g_pipeline_layout_map.clear();
}

void VulkanRender::update_material_descriptor_set(Init& init, RenderData& data, uint32_t image_index, std::shared_ptr<Material> material)
{
    if (data.material_map_descriptor.find(material) == data.material_map_descriptor.end())
    {
        data.material_map_descriptor[material] = std::vector<DescriptorSetInfo>(data.material_descriptor.size());
        for (size_t i = 0; i < data.material_descriptor.size(); i++)
        {
            create_material_descriptor_set(init, data, i, material);
            create_material_ubo(init, data, i, material);
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = materialUboMap[material][i]->getVulkanBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = data.material_map_descriptor[material][i].descriptorSet;
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            init.disp.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

			std::vector<VkDescriptorImageInfo> imageInfos;
            for (size_t j = 0; j < material->getTextures().size(); j++)
            {
                auto texture = material->getTextures()[j];
                VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(texture.get());
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = vulkanTexture->getImageView();
                imageInfo.sampler = vulkanTexture->getSampler();
                imageInfos.push_back(imageInfo);
            }
            if (material->getTextures().size() > 0) {
			    VkWriteDescriptorSet textureDescriptorWrite{};
			    textureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			    textureDescriptorWrite.dstSet = data.material_map_descriptor[material][i].descriptorSet;
			    textureDescriptorWrite.dstBinding = 1;
			    textureDescriptorWrite.dstArrayElement = 0;
			    textureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			    textureDescriptorWrite.descriptorCount = static_cast<uint32_t>(imageInfos.size());
			    textureDescriptorWrite.pImageInfo = imageInfos.data();
			    init.disp.updateDescriptorSets(1, &textureDescriptorWrite, 0, nullptr);
            }
			
        }

    }
    material->apply();
    VulkanShader* vulkanShader = static_cast<VulkanShader*>(material->getShader().get());

    materialUboMap[material][image_index]->setData(vulkanShader->getUniformBuffer().data()
        , vulkanShader->getUniformBuffer().size() * sizeof(uint8_t), BufferUsage::DYNAMIC);

}



void VulkanRender::update_light_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene)
{
	RenderScene& scene = renderScenes[indexScene];
    lightBufferData.count = static_cast<int>(scene.sceneLight.size());
    for (size_t i = 0; i < scene.sceneLight.size() && i < 125; i++)
    {
        lightBufferData.lights[i].lightPositions = scene.sceneLight[i].position;
        lightBufferData.lights[i].lightColors = scene.sceneLight[i].light.color;
        lightBufferData.lights[i].intensity = scene.sceneLight[i].light.intensity;
    }
    sceneLightUbo[indexScene][image_index]->setData(&lightBufferData, sizeof(lightBufferData), BufferUsage::DYNAMIC);
}

void VulkanRender::update_camera_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene)
{
	RenderScene& scene = renderScenes[indexScene];
    // Prendi le matrici generate da GLM (probabilmente in stile GL)
    glm::mat4 projGL = scene.activeCamera->getProjectionMatrix();
    glm::mat4 viewGL = scene.activeCamera->getViewMatrix();

    // Correzione GL -> Vulkan (flip Y + mappa depth [-1,1] -> [0,1])
    glm::mat4 glToVk = glm::mat4(1.0f);
    glToVk[1][1] = -1.0f;   // flip Y
    glToVk[2][2] = -0.5f;
    glToVk[3][2] = 0.5f;

    // Applica la conversione e assegna i campi nell'ordine atteso dallo shader (proj, view, position)
    cameraBufferData.projection = glToVk*projGL;
    cameraBufferData.view = viewGL;
    cameraBufferData.cameraPosition = glm::vec4(scene.activeCamera->getPosition(), 1.0f);
    sceneCameraUbo[indexScene][image_index]->setData(&cameraBufferData, sizeof(cameraBufferData), BufferUsage::DYNAMIC);
}

void VulkanRender::update_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index, int indexScene)
{
	if (data.scene_map_descriptor.find(indexScene) == data.scene_map_descriptor.end())
    {
        // Initialize per-scene array of descriptor infos for each swapchain image
        int frameCount = static_cast<int>(data.global_descriptor.size());
        data.scene_map_descriptor[indexScene] = std::vector<DescriptorSetInfo>(frameCount);

        // Ensure UBO containers exist and have the same size
        sceneLightUbo[indexScene].resize(frameCount);
        sceneCameraUbo[indexScene].resize(frameCount);

        // Allocate descriptor sets and create / bind UBOs for every frame index (NOT only the current image_index)
        for (int i = 0; i < frameCount; ++i)
        {
            // Allocate descriptor set for this scene and frame
            if (create_global_descriptor_set(init, data, static_cast<uint32_t>(i), indexScene) != 0)
            {
                std::cerr << "create_global_descriptor_set: failed for scene " << indexScene << " frame " << i << "\n";
                continue;
            }

            // Create UBOs per-frame
            sceneLightUbo[indexScene][i] = std::make_shared<VulkanUniformBuffer>(0, sizeof(LightBufferStruct));
            sceneCameraUbo[indexScene][i] = std::make_shared<VulkanUniformBuffer>(0, sizeof(CameraBufferStruct));

            // bind light UBO to descriptor set (binding = 2)
            VkDescriptorBufferInfo lightBufferInfo{};
            lightBufferInfo.buffer = sceneLightUbo[indexScene][i]->getVulkanBuffer();
            lightBufferInfo.offset = 0;
            lightBufferInfo.range = VK_WHOLE_SIZE;
            VkWriteDescriptorSet lightDescriptorWrite{};
            lightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            lightDescriptorWrite.dstSet = data.scene_map_descriptor[indexScene][i].descriptorSet;
            lightDescriptorWrite.dstBinding = 2;
            lightDescriptorWrite.dstArrayElement = 0;
            lightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            lightDescriptorWrite.descriptorCount = 1;
            lightDescriptorWrite.pBufferInfo = &lightBufferInfo;

            // bind camera UBO to descriptor set (binding = 1)
            VkDescriptorBufferInfo cameraBufferInfo{};
            cameraBufferInfo.buffer = sceneCameraUbo[indexScene][i]->getVulkanBuffer();
            cameraBufferInfo.offset = 0;
            cameraBufferInfo.range = VK_WHOLE_SIZE;
            VkWriteDescriptorSet cameraDescriptorWrite{};
            cameraDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            cameraDescriptorWrite.dstSet = data.scene_map_descriptor[indexScene][i].descriptorSet;
            cameraDescriptorWrite.dstBinding = 1;
            cameraDescriptorWrite.dstArrayElement = 0;
            cameraDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            cameraDescriptorWrite.descriptorCount = 1;
            cameraDescriptorWrite.pBufferInfo = &cameraBufferInfo;
            // Bind SSBO al descriptor set
            VkDescriptorBufferInfo ssboInfo{};
            ssboInfo.buffer = sceneModelMatricesSSBO[indexScene][i]->getVulkanBuffer();
            ssboInfo.offset = 0;
            ssboInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet ssboWrite{};
            ssboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            ssboWrite.dstSet = data.scene_map_descriptor[indexScene][i].descriptorSet;
            ssboWrite.dstBinding = 3;
            ssboWrite.dstArrayElement = 0;
            ssboWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            ssboWrite.descriptorCount = 1;
            ssboWrite.pBufferInfo = &ssboInfo;

            VkWriteDescriptorSet writes[3] = { lightDescriptorWrite, cameraDescriptorWrite , ssboWrite};
            init.disp.updateDescriptorSets(3, writes, 0, nullptr);
        }
    }

    // Always update the contents of the UBOs for the currently used image_index

    // Always update the contents of the UBOs for the currently used image_index
    update_light_descriptor_set(init, data, image_index, indexScene);
    update_camera_descriptor_set(init, data, image_index, indexScene);

}

void VulkanRender::create_material_ubo(Init& init, RenderData& data, uint32_t image_index, std::shared_ptr<Material> material)
{
    VulkanShader* vulkanShader = static_cast<VulkanShader*>(material->getShader().get());
    materialUboMap[material].push_back(std::make_shared<VulkanUniformBuffer>(0,
        vulkanShader->getUniformBuffer().size() * sizeof(uint8_t)));
}

void VulkanRender::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    init.disp.cmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    endSingleTimeCommands(commandBuffer);
}

void VulkanRender::shut_mesh_buffers()
{
    for (auto& pair : meshGPUMap)
    {
        pair.second.destroy();
    }
}

void VulkanRender::render_scene(VkCommandBuffer command_buffer, int indexScene)
{
    auto& cache = sceneRenderCaches[indexScene];

    // Calcola hash della geometria
    size_t currentHash = calculateSceneHash(indexScene);

   

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

    VkCommandBufferInheritanceInfo inheritanceInfo{};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.renderPass = data.render_pass;
    inheritanceInfo.framebuffer = data.framebuffers[data.image_index];
    beginInfo.pInheritanceInfo = &inheritanceInfo;


    // Registra comandi draw (codice esistente)
	RenderScene& scene = renderScenes[indexScene];
    auto batches = scene.batches;
    for (const auto& pair : batches)
    {
        auto material = pair.first.first;
        auto renderingType = pair.first.second;
        auto batchRenders = pair.second;
        auto pipelineKey = std::make_pair(material->getShader(), renderingType);
        VulkanShader* vulkanShader = static_cast<VulkanShader*>(material->getShader().get());
        VkPipeline pipeline;
        get_or_create_pipeline(pipelineKey, pipeline, material, renderingType);

        // Bind exactly il pipeline usato (obbligatorio)
        init.disp.cmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        // Bind descriptor sets usando il pipelineLayout corrispondente al pipeline bindato
        VkPipelineLayout bindLayout = VK_NULL_HANDLE;
        int retFlag;
        bindMaterialDescriptorsSet(pipeline, bindLayout, indexScene, material, command_buffer, retFlag);
        if (retFlag == 3) continue;

		std::unordered_map<Mesh*, std::vector<glm::mat4>> meshInstance;
        for (const auto& renderData : batchRenders) {
            RenderMeshComponent* mesh = renderData.renderMesh;
            if (!mesh)
            {
                std::cerr << "render_scene: renderData.renderMesh is null, skipping instance\n";
                continue;
            }
            meshInstance[mesh->mesh.get()].push_back(renderData.model);

        }

        for (const auto& renderData : batchRenders)
        {
            auto mesh = renderData.renderMesh;
            if (!mesh)
            {
                std::cerr << "render_scene: renderData.renderMesh is null, skipping instance\n";
                continue;
            }

            auto meshPtr = mesh->mesh.get();
            if (!meshPtr)
            {
                std::cerr << "render_scene: mesh->mesh.get() is null, skipping instance\n";
                continue;
            }

            // Recupera o crea VulkanMeshGPU in modo sicuro
            auto [it, inserted] = meshGPUMap.try_emplace(meshPtr, *meshPtr);
            VulkanMeshGPU& meshGPU = it->second;
            auto rangeIt = instanceRanges.find({ meshPtr, material });
            if (rangeIt == instanceRanges.end()) continue;

            const auto& range = rangeIt->second;
			meshGPU.drawInstanced(command_buffer, range.instanceCount, range.firstInstance);
        }
    }

    // Esegui

    cache.isDirty = false;
    cache.geometryHash = currentHash;
}

void VulkanRender::get_or_create_pipeline(VulkanRender::pipelineKey& pipelineKey, VkPipeline& pipeline, std::shared_ptr<Material>& material, RenderingTypeEnum renderingType)
{
    if (pipelines.find(pipelineKey) == pipelines.end())
    {
        pipeline = create_graphics_pipeline(init, data, material->getShader(), renderingType);
        pipelines[pipelineKey] = pipeline;
    }
    else
    {
        pipeline = pipelines[pipelineKey];
    }
}

void VulkanRender::bindMaterialDescriptorsSet(VkPipeline& pipeline, VkPipelineLayout& bindLayout, int& indexScene, std::shared_ptr<Material>& material, VkCommandBuffer command_buffer, int& retFlag)
{
    retFlag = 1;
    auto itLayout = g_pipeline_layout_map.find(pipeline);
    if (itLayout != g_pipeline_layout_map.end())
        bindLayout = itLayout->second;
    else if (!pipeline_layouts.empty())
        bindLayout = pipeline_layouts[0]; // fallback (non ideale)

    // Controlli di validità sui descriptor set della scena e del materiale
    if (indexScene < 0 || static_cast<size_t>(indexScene) >= data.scene_map_descriptor.size())
    {
        std::cerr << "render_scene: invalid indexScene or no scene descriptor allocated\n";
        { retFlag = 3; return; };
    }
    if (data.current_frame >= data.scene_map_descriptor[indexScene].size())
    {
        std::cerr << "render_scene: invalid image_index for scene descriptors\n";
        { retFlag = 3; return; };
    }
    auto matIt = data.material_map_descriptor.find(material);
    if (matIt == data.material_map_descriptor.end())
    {
        std::cerr << "render_scene: material descriptor vector missing for material, skipping instance\n";
        { retFlag = 3; return; };
    }
    if (data.current_frame >= matIt->second.size())
    {
        std::cerr << "render_scene: invalid image_index for material descriptors\n";
        { retFlag = 3; return; };
    }
    const DescriptorSetInfo& matDescInfo = matIt->second[data.current_frame];
    if (matDescInfo.descriptorSet == VK_NULL_HANDLE)
    {
        std::cerr << "render_scene: material descriptor set is null, skipping instance\n";
        { retFlag = 3; return; };
    }
    const DescriptorSetInfo& sceneDescInfo = data.scene_map_descriptor[indexScene][data.current_frame];
    if (sceneDescInfo.descriptorSet == VK_NULL_HANDLE)
    {
        std::cerr << "render_scene: scene descriptor set is null, skipping instance\n";
        { retFlag = 3; return; };
    }


    std::vector<VkDescriptorSet> descriptorSets;
    descriptorSets.push_back(sceneDescInfo.descriptorSet);
    descriptorSets.push_back(matDescInfo.descriptorSet);

    init.disp.cmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, bindLayout,
        0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
}


VulkanRender::~VulkanRender()
{

}
size_t VulkanRender::calculateSceneHash(int indexScene) {
    size_t hash = 0;
    RenderScene& scene = renderScenes[indexScene];

    for (const auto& pair : scene.batches) {
        hash ^= std::hash<void*>{}(pair.first.first.get()); // Material
        hash ^= static_cast<size_t>(pair.first.second);      // RenderingType
        hash ^= pair.second.size();                          // Numero istanze
    }

    return hash;
}
void VulkanRender::BeginFrame()
{
    // 1. Attendi che il frame precedente (fence per questo slot) sia finito
    vkWaitForFences(
        init.device,
        1,
        &data.in_flight_fences[data.current_frame],
        VK_TRUE,
        UINT64_MAX
    );

    // 2. Acquisisci immagine (semaphore associata a questo frame)
    VkResult res = vkAcquireNextImageKHR(
        init.device,
        init.swapchain,
        UINT64_MAX,
        data.available_semaphores[data.current_frame],
        VK_NULL_HANDLE,
        &data.image_index
    );

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain(init, data);
        return;
    }
    else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        std::cerr << "Failed to acquire swap chain image (vkAcquireNextImageKHR): " << res << "\n";
        return;
    }

    // 3. Se l'immagine è già in flight (da un altro frame), attendi la fence associata
    if (data.image_in_flight[data.image_index] != VK_NULL_HANDLE
        && data.image_in_flight[data.image_index] != data.in_flight_fences[data.current_frame]) {
        vkWaitForFences(
            init.device,
            1,
            &data.image_in_flight[data.image_index],
            VK_TRUE,
            UINT64_MAX
        );
    }

    // 4. Ora è sicuro resettare la fence del frame corrente (non è più in uso)
    vkResetFences(
        init.device,
        1,
        &data.in_flight_fences[data.current_frame]
    );

    // 5. Associa immagine → fence corrente
    data.image_in_flight[data.image_index] =
        data.in_flight_fences[data.current_frame];

    // 6. Registra command buffer PER FRAME
    updateAllDescriptorDSet();
    begin_record_command_buffer(init, data, data.image_index);

    for (int i = 0; i < renderScenes.size(); i++) {
        render_scene(data.command_buffers[data.current_frame], i);
    }
}

void VulkanRender::updateAllDescriptorDSet()
{
    for (int i = 0; i < renderScenes.size(); i++)
    {
		RenderScene& scene = renderScenes[i];
		// take all model matrices of the scene 
        std::vector<ModelMatrixData> allModelMatrices;
        instanceRanges.clear();
        

        for (const auto& pair : scene.batches)
        {
            auto material = pair.first.first;
            auto renderingType = pair.first.second;
            const auto& batchRenders = pair.second;
			// group by mesh
            std::unordered_map<Mesh*, std::vector<glm::mat4>> meshInstances;
            for (const auto& renderData : batchRenders) {
                meshInstances[renderData.renderMesh->mesh.get()].push_back(renderData.model);
            }

			// add to all model matrices and record instance ranges
            for (const auto& meshPair : meshInstances) {
                Mesh* meshPtr = meshPair.first;
                const auto& matrices = meshPair.second;

                uint32_t firstInstance = static_cast<uint32_t>(allModelMatrices.size());
                for (const auto& mat : matrices) {
                    allModelMatrices.push_back({ mat });
                }

                instanceRanges[{meshPtr, material}] = {
                    firstInstance,
                    static_cast<uint32_t>(matrices.size())
                };
            }


            VulkanShader* vulkanShader = static_cast<VulkanShader*>(material->getShader().get());
            material->bind();
			material->apply();
            update_material_descriptor_set(init, data, data.current_frame, material);

			// for every instance, update model (model UBO) before recording
            for (const auto& renderData : pair.second)
            {
                auto mesh = renderData.renderMesh;
                auto meshKey = mesh->mesh.get();
                auto [it, inserted] = meshGPUMap.try_emplace(meshKey, *(mesh->mesh.get()));
                VulkanMeshGPU& meshGPU = it->second;
                meshGPU.uploadToGPU(); // se upload usa single-time-submit è ok farlo qui
            }
        }
        // Aggiorna global descriptor set
		// Aggiorna SSBO globale delle model matrices
         // Crea/aggiorna SSBO per questa scena e frame
        size_t bufferSize = allModelMatrices.size() * sizeof(ModelMatrixData);
        if (sceneModelMatricesSSBO[i].size() <= data.current_frame) {
            sceneModelMatricesSSBO[i].resize(MAX_FRAMES_IN_FLIGHT);
            for (auto& ssbo : sceneModelMatricesSSBO[i])
            {
                ssbo = std::make_shared<VulkanStorageBuffer>(3, bufferSize * 5);
            }
           
            
        }

        update_global_descriptor_set(init, data, data.current_frame, i);

        if (bufferSize > 0) {
            sceneModelMatricesSSBO[i][data.current_frame]->setData(
                allModelMatrices.data(),
                bufferSize,
                BufferUsage::DYNAMIC
            );
        }

       

        
    }
}

void VulkanRender::submit()
{
    VkCommandBuffer cmd = data.command_buffers[data.current_frame];

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        std::exit(-1);
    }

    VkSemaphore wait_semaphores[] = {
        data.available_semaphores[data.current_frame]
    };

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore signal_semaphores[] = {
        data.finished_semaphore[data.image_index]
    };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = wait_semaphores;
    submitInfo.pWaitDstStageMask = wait_stages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(
        data.graphics_queue,
        1,
        &submitInfo,
        data.in_flight_fences[data.current_frame]
    ) != VK_SUCCESS) {
        std::exit(-1);
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signal_semaphores;
    presentInfo.swapchainCount = 1;
	VkSwapchainKHR swapChains[] = { init.swapchain };
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &data.image_index;

    vkQueuePresentKHR(data.present_queue, &presentInfo);

    data.current_frame =
        (data.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    renderScenes.clear();
}

void VulkanRender::Shutdown()
{
    // Ensure GPU is idle before destroying resources
    init.disp.deviceWaitIdle();

    // 1) Free and destroy per-scene descriptor sets
    for (auto& scenePair : data.scene_map_descriptor)
    {
        auto& vec = scenePair.second;
        std::vector<VkDescriptorSet> sets;
        sets.reserve(vec.size());
        for (auto& info : vec)
        {
            if (info.descriptorSet != VK_NULL_HANDLE)
                sets.push_back(info.descriptorSet);
        }
        if (!sets.empty())
        {
            init.disp.freeDescriptorSets(data.descriptors_pool, static_cast<uint32_t>(sets.size()), sets.data());
        }
    }
    data.scene_map_descriptor.clear();

    // 2) Free and destroy per-material descriptor sets
    for (auto& matPair : data.material_map_descriptor)
    {
        auto& vec = matPair.second;
        std::vector<VkDescriptorSet> sets;
        sets.reserve(vec.size());
        for (auto& info : vec)
        {
            if (info.descriptorSet != VK_NULL_HANDLE)
                sets.push_back(info.descriptorSet);
        }
        if (!sets.empty())
        {
            init.disp.freeDescriptorSets(data.descriptors_pool, static_cast<uint32_t>(sets.size()), sets.data());
        }
    }
    data.material_map_descriptor.clear();

    // 3) Shutdown and clear all UBOs created per-material
    for (auto& pair : materialUboMap)
    {
        for (auto& ubo : pair.second)
        {
            if (ubo) ubo->shutdown();
        }
    }
    materialUboMap.clear();

    // 4) Shutdown per-scene UBOs (camera + lights)
    for (auto& [it,vec ]: sceneCameraUbo)
    {
        for (auto& ubo : vec) if (ubo) ubo->shutdown();
    }
    sceneCameraUbo.clear();
    for (auto& [it, vec] : sceneLightUbo)
    {
        for (auto& ubo : vec) if (ubo) ubo->shutdown();
    }
    sceneLightUbo.clear();

    // 5) Shutdown global per-frame UBOs (if still present)
    for (auto& ubo : cameraUbo) if (ubo) ubo->shutdown();
    cameraUbo.clear();
    for (auto& ubo : lightUbo) if (ubo) ubo->shutdown();
    lightUbo.clear();

	// 10) Shutdown per-scene SSBOs
    for (auto& [it, vec] : sceneModelMatricesSSBO)
    {
        for (auto& ssbo : vec) if (ssbo) ssbo->shutdown();
	}

    // 6) Destroy mesh GPU resources
    shut_mesh_buffers();
    meshGPUMap.clear();

    // 7) Destroy pipelines and shader resources (and pipeline layouts)
    shut_shaders();
    pipelines.clear();
    pipeline_layouts.clear();
    g_pipeline_layout_map.clear();

    // 8) Destroy descriptor pool (after individual sets freed)
    if (data.descriptors_pool != VK_NULL_HANDLE)
    {
        // use dispatch if available
        init.disp.destroyDescriptorPool(data.descriptors_pool, nullptr);
        data.descriptors_pool = VK_NULL_HANDLE;
    }

    // 9) Destroy descriptor set layouts (unique ones)
    if (globalDescriptorSetLayout != VK_NULL_HANDLE)
    {
        init.disp.destroyDescriptorSetLayout(globalDescriptorSetLayout, nullptr);
        globalDescriptorSetLayout = VK_NULL_HANDLE;
    }
    if (materialDescriptorSetLayout != VK_NULL_HANDLE)
    {
        init.disp.destroyDescriptorSetLayout(materialDescriptorSetLayout, nullptr);
        materialDescriptorSetLayout = VK_NULL_HANDLE;
    }

   

    // 11) Destroy semaphores and fences
    for (size_t i = 0; i < data.finished_semaphore.size(); ++i)
    {
        if (data.finished_semaphore[i] != VK_NULL_HANDLE)
            init.disp.destroySemaphore(data.finished_semaphore[i], nullptr);
    }
    data.finished_semaphore.clear();

    for (size_t i = 0; i < data.available_semaphores.size(); ++i)
    {
        if (data.available_semaphores[i] != VK_NULL_HANDLE)
            init.disp.destroySemaphore(data.available_semaphores[i], nullptr);
    }
    data.available_semaphores.clear();

    for (size_t i = 0; i < data.in_flight_fences.size(); ++i)
    {
        if (data.in_flight_fences[i] != VK_NULL_HANDLE)
            init.disp.destroyFence(data.in_flight_fences[i], nullptr);
    }
    data.in_flight_fences.clear();
    data.image_in_flight.clear();

    // 12) Destroy command pool
    if (data.command_pool != VK_NULL_HANDLE)
    {
        init.disp.destroyCommandPool(data.command_pool, nullptr);
        data.command_pool = VK_NULL_HANDLE;
    }

    // 13) Destroy framebuffers
    for (auto fb : data.framebuffers)
    {
        if (fb != VK_NULL_HANDLE)
            init.disp.destroyFramebuffer(fb, nullptr);
    }
    data.framebuffers.clear();
    // 14) Destroy depth resources (PRIMA del render pass)
    if (data.depth_image_view != VK_NULL_HANDLE)
    {
        init.disp.destroyImageView(data.depth_image_view, nullptr);
        data.depth_image_view = VK_NULL_HANDLE;
    }
    if (data.depth_image != VK_NULL_HANDLE)
    {
        vmaDestroyImage(allocator, data.depth_image, data.depth_image_allocation);
        data.depth_image = VK_NULL_HANDLE;
        data.depth_image_allocation = VK_NULL_HANDLE;
    }
    // 14) Destroy render pass
    if (data.render_pass != VK_NULL_HANDLE)
    {
        init.disp.destroyRenderPass(data.render_pass, nullptr);
        data.render_pass = VK_NULL_HANDLE;
    }

    // 15) Destroy swapchain image views and swapchain
    if (!data.swapchain_image_views.empty())
    {
        init.swapchain.destroy_image_views(data.swapchain_image_views);
        data.swapchain_image_views.clear();
    }
    data.swapchain_images.clear();

    if (init.swapchain != VK_NULL_HANDLE)
    {
        vkb::destroy_swapchain(init.swapchain);
    }
    // 10) Destroy VMA allocator
    if (allocator != VK_NULL_HANDLE)
    {
        vmaDestroyAllocator(allocator);
        allocator = VK_NULL_HANDLE;
    }
    // 16) Destroy device, surface and instance
    if (init.device) { vkb::destroy_device(init.device); init.device = {}; }
    if (init.surface != VK_NULL_HANDLE) { vkb::destroy_surface(init.instance, init.surface); init.surface = VK_NULL_HANDLE; }
    if (init.instance) { vkb::destroy_instance(init.instance); init.instance = {}; }

    // 17) Clear remaining runtime containers
    data.command_buffers.clear();
    data.swapchain_images.clear();
    renderScenes.clear();
}
}