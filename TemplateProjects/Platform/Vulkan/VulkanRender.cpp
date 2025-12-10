#include "VulkanRender.h"
#include "Application/Application.h"
#include "Core/View/View.h"
#include <iostream>
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include <memory>
#include <unordered_map>
#define MAX_SAMPLERS 120
#define CHECK_RESULT(x) \
    if (x != 0) { \
        std::cout << "Failed" << "\n"; \
        std::exit( -1); \
    }
// Map pipeline -> pipelineLayout per garantire compatibilità al bind dei descriptor sets
static std::unordered_map<VkPipeline, VkPipelineLayout> g_pipeline_layout_map;

VulkanRender::VulkanRender()
    :BatchRender()
{
    window = (GLFWwindow*)Application::getInstance()->getWindow()->getNativeWindow();
    CHECK_RESULT(device_initialization(init));
    CHECK_RESULT(create_swapchain(init));
    CHECK_RESULT(get_queues(init, data));
    CHECK_RESULT(create_render_pass(init, data));
    CHECK_RESULT(create_framebuffers(init, data));
    CHECK_RESULT(create_command_pool(init, data));
    CHECK_RESULT(create_command_buffers(init, data));
    CHECK_RESULT(create_sync_objects(init, data));
    CHECK_RESULT(create_allocator(init.device.physical_device, init.device));
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
    auto instance_ret = instance_builder.use_default_debug_messenger().request_validation_layers().build();
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
    auto swap_ret = swapchain_builder.set_old_swapchain(init.swapchain).build();
    if (!swap_ret) {
        std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
        return -1;
    }
    vkb::destroy_swapchain(init.swapchain);
    init.swapchain = swap_ret.value();
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

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
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
        VkImageView attachments[] = { data.swapchain_image_views[i] };

        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = data.render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
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

        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = data.render_pass;
        render_pass_info.framebuffer = data.framebuffers[i];
        render_pass_info.renderArea.offset = { 0, 0 };
        render_pass_info.renderArea.extent = init.swapchain.extent;
        VkClearValue clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clearColor;

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

        init.disp.cmdSetViewport(data.command_buffers[i], 0, 1, &viewport);
        init.disp.cmdSetScissor(data.command_buffers[i], 0, 1, &scissor);

        init.disp.cmdBeginRenderPass(data.command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        //init.disp.cmdDraw(data.command_buffers[i], 3, 1, 0, 0);

        init.disp.cmdEndRenderPass(data.command_buffers[i]);

        if (init.disp.endCommandBuffer(data.command_buffers[i]) != VK_SUCCESS) {
            std::cout << "failed to record command buffer\n";
            return -1; // failed to record command buffer!
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

    init.swapchain.destroy_image_views(data.swapchain_image_views);

    if (0 != create_swapchain(init)) return -1;
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
    int frame_count = static_cast<int>(data.swapchain_image_views.size());
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

int VulkanRender::begin_record_command_buffer(Init& init, RenderData& data, uint32_t image_index)
{
    init.disp.resetCommandBuffer(data.command_buffers[image_index], 0);
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    if (init.disp.beginCommandBuffer(data.command_buffers[image_index], &begin_info) != VK_SUCCESS) {
        return -1; // failed to begin recording command buffer
    }

    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = data.render_pass;
    render_pass_info.framebuffer = data.framebuffers[image_index];
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = init.swapchain.extent;
    VkClearValue clearColor{ { { 0.2,0.2,0.1, 1.0f } } };
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

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

    init.disp.cmdSetViewport(data.command_buffers[image_index], 0, 1, &viewport);
    init.disp.cmdSetScissor(data.command_buffers[image_index], 0, 1, &scissor);

    init.disp.cmdBeginRenderPass(data.command_buffers[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    //init.disp.cmdDraw(data.command_buffers[i], 3, 1, 0, 0);

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

        VkDescriptorSetLayoutBinding bindings[] = { timeBinding, viewProjBinding, lightBinding };
        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 3;
        layout_info.pBindings = bindings;
        if (init.disp.createDescriptorSetLayout(&layout_info, nullptr, &globalDescriptorSetLayout) != VK_SUCCESS)
        {
            std::cout << "failed to create global descriptor set layout\n";
            return -1; // failed to create descriptor set layout
        }
    }

    // Allocate descriptor set for this frame using globalDescriptorSetLayout
    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = data.descriptors_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &globalDescriptorSetLayout;
    if (init.disp.allocateDescriptorSets(&alloc_info, &data.global_descriptor[image_index].descriptorSet) != VK_SUCCESS)
    {
        std::cout << "failed to allocate global descriptor set\n";
        return -1; // failed to allocate descriptor set
    }

    data.global_descriptor[image_index].descriptorSetLayout = globalDescriptorSetLayout;
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
    descriptorSetLayouts.push_back(data.global_descriptor[0].descriptorSetLayout);
    descriptorSetLayouts.push_back(data.material_descriptor[0].descriptorSetLayout);
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
        }

    }
    material->apply();
    VulkanShader* vulkanShader = static_cast<VulkanShader*>(material->getShader().get());

    materialUboMap[material][image_index]->setData(vulkanShader->getUniformBuffer().data()
        , vulkanShader->getUniformBuffer().size() * sizeof(uint8_t), BufferUsage::DYNAMIC);

}



void VulkanRender::update_light_descriptor_set(Init& init, RenderData& data, uint32_t image_index, RenderScene& scene)
{
    lightBufferData.count = static_cast<int>(scene.sceneLight.size());
    for (size_t i = 0; i < scene.sceneLight.size() && i < 125; i++)
    {
        lightBufferData.lights[i].lightPositions = scene.sceneLight[i].position;
        lightBufferData.lights[i].lightColors = scene.sceneLight[i].light.color;
        lightBufferData.lights[i].intensity = scene.sceneLight[i].light.intensity;
    }
    lightUbo[image_index]->setData(&lightBufferData, sizeof(lightBufferData), BufferUsage::DYNAMIC);
    VkDescriptorBufferInfo bufferInfo{};
    VkBuffer lightBuffer = lightUbo[image_index]->getVulkanBuffer();
    bufferInfo.buffer = lightBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = data.global_descriptor[image_index].descriptorSet;
    descriptorWrite.dstBinding = 2;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    init.disp.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void VulkanRender::update_camera_descriptor_set(Init& init, RenderData& data, uint32_t image_index, RenderScene& scene)
{
    // Prendi le matrici generate da GLM (probabilmente in stile GL)
    glm::mat4 projGL = scene.activeCamera->getProjectionMatrix();
    glm::mat4 viewGL = scene.activeCamera->getViewMatrix();

    // Correzione GL -> Vulkan (flip Y + mappa depth [-1,1] -> [0,1])
    glm::mat4 glToVk = glm::mat4(1.0f);
    glToVk[1][1] = -1.0f;   // flip Y
    glToVk[2][2] = 0.5f;    // scale zq
    glToVk[3][2] = 0.5f;    // translate z

    // Applica la conversione e assegna i campi nell'ordine atteso dallo shader (proj, view, position)
    cameraBufferData.projection = projGL;
    cameraBufferData.view = viewGL;
    cameraBufferData.cameraPosition = glm::vec4(scene.activeCamera->getPosition(), 1.0f);


    cameraUbo[image_index]->setData(&cameraBufferData, sizeof(cameraBufferData), BufferUsage::DYNAMIC);
    VkDescriptorBufferInfo bufferInfo{};
    VkBuffer cameraBuffer = cameraUbo[image_index]->getVulkanBuffer();
    bufferInfo.buffer = cameraBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = data.global_descriptor[image_index].descriptorSet;
    descriptorWrite.dstBinding = 1;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    init.disp.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);


}

void VulkanRender::update_global_descriptor_set(Init& init, RenderData& data, uint32_t image_index, RenderScene& scene)
{
    update_light_descriptor_set(init, data, image_index, scene);
    update_camera_descriptor_set(init, data, image_index, scene);

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

void VulkanRender::render_scene(VkCommandBuffer command_buffer, RenderScene& scene)
{
    auto batches = scene.batches;
    for (const auto& pair : batches)
    {
        auto material = pair.first.first;
        auto renderingType = pair.first.second;
        auto batchRenders = pair.second;
        auto pipelineKey = std::make_pair(material->getShader(), renderingType);
        VulkanShader* vulkanShader = static_cast<VulkanShader*>(material->getShader().get());
        VkPipeline pipeline;
        if (pipelines.find(pipelineKey) == pipelines.end())
        {
            pipeline = create_graphics_pipeline(init, data, material->getShader(), renderingType);
            pipelines[pipelineKey] = pipeline;
        }
        else
        {
            pipeline = pipelines[pipelineKey];
        }

        // Bind exactly il pipeline usato (obbligatorio)
        init.disp.cmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        for (const auto& renderData : batchRenders)
        {
            auto mesh = renderData.renderMesh;
            auto meshKey = mesh->mesh.get();
			auto it = meshGPUMap.find(meshKey);
            VulkanMeshGPU& meshGPU = it->second;
            // Bind descriptor sets usando il pipelineLayout corrispondente al pipeline bindato
            VkPipelineLayout bindLayout = VK_NULL_HANDLE;
            auto itLayout = g_pipeline_layout_map.find(pipeline);
            if (itLayout != g_pipeline_layout_map.end()) bindLayout = itLayout->second;
            else if (!pipeline_layouts.empty()) bindLayout = pipeline_layouts[0]; // fallback (non ideale)

            std::vector<VkDescriptorSet> descriptorSets;
            descriptorSets.push_back(data.global_descriptor[data.image_index].descriptorSet);
            descriptorSets.push_back(data.material_map_descriptor[material][data.image_index].descriptorSet);
            init.disp.cmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, bindLayout,
                0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
            vkCmdPushConstants(
                command_buffer,
                bindLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(glm::mat4),
                &renderData.model
            );
            meshGPU.draw(command_buffer);
        }
    }
}


VulkanRender::~VulkanRender()
{

}

void VulkanRender::BeginFrame()
{
    init.disp.waitForFences(1, &data.in_flight_fences[data.current_frame], VK_TRUE, UINT64_MAX);

    uint32_t image_index = 0;
    VkResult result = init.disp.acquireNextImageKHR(
        init.swapchain, UINT64_MAX, data.available_semaphores[data.current_frame], VK_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        CHECK_RESULT(recreate_swapchain(init, data))
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        std::cout << "failed to acquire swapchain image. Error " << result << "\n";
        CHECK_RESULT(-1)
    }
    if (data.image_in_flight[image_index] != VK_NULL_HANDLE) {
        init.disp.waitForFences(1, &data.image_in_flight[image_index], VK_TRUE, UINT64_MAX);
        // optional: clear, the image will be assigned the current frame's fence at submit time
        data.image_in_flight[image_index] = VK_NULL_HANDLE;
    }
    data.image_index = image_index;
    updateAllDescriptorDSet();
    begin_record_command_buffer(init, data, data.image_index);
    for (auto& scene : renderScenes)
    {
        render_scene(data.command_buffers[data.image_index], scene);
    }
}

void VulkanRender::updateAllDescriptorDSet()
{
    for (auto& scene : renderScenes)
    {
        // Aggiorna globali (luce, camera)
        update_global_descriptor_set(init, data, data.image_index, scene);

        // Per ogni batch aggiorna material e model descriptor prima della registrazione
        for (const auto& pair : scene.batches)
        {
            auto material = pair.first.first;
            VulkanShader* vulkanShader = static_cast<VulkanShader*>(material->getShader().get());
            // stampo il contenuto del buffer delle shader
            material->bind();
            update_material_descriptor_set(init, data, data.image_index, material);

            // Per ogni istanza aggiorna il modello (model UBO) prima di registrare
            for (const auto& renderData : pair.second)
            {
                auto mesh = renderData.renderMesh;
                auto meshKey = mesh->mesh.get();
                auto [it, inserted] = meshGPUMap.try_emplace(meshKey, *(mesh->mesh.get()));
                VulkanMeshGPU& meshGPU = it->second;
                meshGPU.uploadToGPU(); // se upload usa single-time-submit è ok farlo qui
            }
        }
    }
}

void VulkanRender::submit()
{
    init.disp.cmdEndRenderPass(data.command_buffers[data.image_index]);

    if (init.disp.endCommandBuffer(data.command_buffers[data.image_index]) != VK_SUCCESS) {
        std::cout << "failed to record command buffer\n";
        std::exit(-1); // failed to record command buffer!
    }
    if (data.image_in_flight[data.image_index] != VK_NULL_HANDLE) {
        init.disp.waitForFences(1, &data.image_in_flight[data.image_index], VK_TRUE, UINT64_MAX);
    }
    data.image_in_flight[data.image_index] = data.in_flight_fences[data.current_frame];
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = { data.available_semaphores[data.current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = wait_semaphores;
    submitInfo.pWaitDstStageMask = wait_stages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &data.command_buffers[data.image_index];

    VkSemaphore signal_semaphores[] = { data.finished_semaphore[data.image_index] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signal_semaphores;

    init.disp.resetFences(1, &data.in_flight_fences[data.current_frame]);

    if (init.disp.queueSubmit(data.graphics_queue, 1, &submitInfo, data.in_flight_fences[data.current_frame]) != VK_SUCCESS) {
        std::cout << "failed to submit draw command buffer\n";
        CHECK_RESULT(-1); //"failed to submit draw command buffer
    }

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapChains[] = { init.swapchain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapChains;

    present_info.pImageIndices = &data.image_index;

    VkResult result = init.disp.queuePresentKHR(data.present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        CHECK_RESULT(recreate_swapchain(init, data));
    }
    else if (result != VK_SUCCESS) {
        std::cout << "failed to present swapchain image\n";
        CHECK_RESULT(-1);
    }

    data.current_frame = (data.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    renderScenes.clear();
}

void VulkanRender::Shutdown()
{
    init.disp.deviceWaitIdle();

    for (auto& ubo : cameraUbo)
    {
        ubo->shutdown();
    }
    for (auto& ubo : lightUbo)
    {
        ubo->shutdown();
    }
    shut_mesh_buffers();
    shut_shaders();
    vkDestroyDescriptorPool(init.device, data.descriptors_pool, nullptr);

    // Destroy the unique descriptor set layouts once
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

    vmaDestroyAllocator(allocator);
    for (size_t i = 0; i < init.swapchain.image_count; i++) {
        init.disp.destroySemaphore(data.finished_semaphore[i], nullptr);
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        init.disp.destroySemaphore(data.available_semaphores[i], nullptr);
        init.disp.destroyFence(data.in_flight_fences[i], nullptr);
    }

    init.disp.destroyCommandPool(data.command_pool, nullptr);

    for (auto framebuffer : data.framebuffers) {
        init.disp.destroyFramebuffer(framebuffer, nullptr);
    }

    init.disp.destroyRenderPass(data.render_pass, nullptr);

    init.swapchain.destroy_image_views(data.swapchain_image_views);

    vkb::destroy_swapchain(init.swapchain);
    vkb::destroy_device(init.device);
    vkb::destroy_surface(init.instance, init.surface);
    vkb::destroy_instance(init.instance);
}
