#include "VulkanRender.h"
#include "Application/Application.h"
#include "Core/View/View.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanRenderTarget.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Platform/Vulkan/VulkanRenderTarget.h"
#include "Platform/Vulkan/VulkanBufferPool.h"
#include "IndirectDrawSystem.h"
#include "Application/AssetManager.h"
#include <iostream>
#include <array>
#include <unordered_set>

namespace OnYuu {

    namespace {
        VkPrimitiveTopology toPrimitiveTopology(RenderingTypeEnum renderingType) {
            switch (renderingType) {
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
    }

    // ============================================================================
    // CONSTRUCTOR & DESTRUCTOR
    // ============================================================================

    VulkanRender::VulkanRender() : BatchRender() {
        window_ = static_cast<GLFWwindow*>(
            Application::getInstance()->getWindow()->getNativeWindow()
            );

        LOG("=== VulkanRender Initialization Started ===\n");

        // STEP 1: Instance
        if (!initializeInstance()) {
            throw std::runtime_error("Failed to initialize Vulkan instance");
        }
        LOG("✓ Instance created\n");

        // STEP 2: Surface
        if (!initializeSurface()) {
            throw std::runtime_error("Failed to initialize surface");
        }
        LOG("✓ Surface created\n");

        // STEP 3: Device
        device_ = std::make_unique<VulkanDevice>();
        if (!device_->initialize(instance_, surface_)) {
            throw std::runtime_error("Failed to initialize Vulkan device");
        }
        LOG("✓ Device created\n");

        // STEP 4: Allocator (BEFORE swapchain for depth buffer)
        if (!initializeAllocator()) {
            throw std::runtime_error("Failed to initialize VMA allocator");
        }
        LOG("✓ VMA allocator created\n");

        // STEP 5: Swapchain temporanea (per ottenere il format)
        swapchain_ = std::make_unique<VulkanSwapchain>(device_.get());
        VulkanSwapchain::Config swapConfig;

        if (!swapchain_->create(surface_, renderPass_, depthImageView_, swapConfig)) {
            throw std::runtime_error("Failed to create temporary swapchain");
        }
        LOG("✓ Temporary swapchain created (format: " << swapchain_->getFormat() << ")\n");

        // STEP 6: Depth resources
        if (!createDepthResources()) {
            throw std::runtime_error("Failed to create depth resources");
        }
        LOG("✓ Depth resources created\n");

        // STEP 7: Render pass
        if (!createRenderPass()) {
            throw std::runtime_error("Failed to create render pass");
        }
        LOG("✓ Render pass created\n");

        // STEP 8: Ricrea swapchain con render pass
        swapchain_->shutdown();
        if (!swapchain_->create(surface_, renderPass_, depthImageView_, swapConfig)) {
            throw std::runtime_error("Failed to create final swapchain");
        }
        LOG("✓ Final swapchain created with " << swapchain_->getImageCount() << " images\n");

        uint32_t frameCount = swapchain_->getImageCount();

        // STEP 9: Descriptor manager
        descriptorManager_ = std::make_unique<VulkanDescriptorManager>(device_.get());
        if (!descriptorManager_->initialize()) {
            throw std::runtime_error("Failed to initialize descriptor manager");
        }
        LOG("✓ Descriptor manager initialized\n");

        // STEP 10: Descriptor layouts
        if (!createDescriptorLayouts()) {
            throw std::runtime_error("Failed to create descriptor layouts");
        }
        LOG("✓ Descriptor layouts created\n");

        // STEP 11: Command manager
        commandManager_ = std::make_unique<VulkanCommandManager>(device_.get());
        if (!commandManager_->initialize(device_->getGraphicsQueueFamily(), frameCount)) {
            throw std::runtime_error("Failed to initialize command manager");
        }
        LOG("✓ Command manager initialized (" << frameCount << " buffers)\n");

        // STEP 12: Sync manager
        syncManager_ = std::make_unique<VulkanSyncManager>(device_.get());
        if (!syncManager_->initialize(MAX_FRAMES_IN_FLIGHT, frameCount)) {
            throw std::runtime_error("Failed to initialize sync manager");
        }
        LOG("✓ Sync manager initialized (" << MAX_FRAMES_IN_FLIGHT << " frames)\n");

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(swapchain_->getImageCount());
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device_->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device_->getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }

        for (size_t i = 0; i < swapchain_->getImageCount(); i++) {
            if (vkCreateSemaphore(device_->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create render finished semaphore for a swapchain image!");
            }
        }

        imagesInFlight.resize(swapchain_->getImageCount(), VK_NULL_HANDLE);

        // STEP 13: Pipeline manager
        pipelineManager_ = std::make_unique<VulkanPipelineManager>(device_.get());
        LOG("✓ Pipeline manager initialized\n");

        // STEP 14: Geometry pool e indirect draw
        geometryPool_ = std::make_shared<GeometryPool>(allocator_, this, 128 * 1024 * 1024);
        indirectDrawManager_ = std::make_shared<IndirectDrawManager>(allocator_, MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            LOG("Frame " << i << " - Image Available Semaphore: " << imageAvailableSemaphores[i]
                << ", In Flight Fence: " << inFlightFences[i] << "\n");
        }
        for (size_t i = 0; i < swapchain_->getImageCount(); i++) {
            LOG("Swapchain Image " << i << " - Render Finished Semaphore: " << renderFinishedSemaphores[i] << "\n");
        }
        for (size_t i = 0; i < imagesInFlight.size(); i++) {
            LOG("Swapchain Image " << i << " - In Flight Fence: " << imagesInFlight[i] << "\n");
        }

        LOG("✓ Geometry pool and indirect draw manager initialized\n");
        LOG("=== VulkanRender Initialization Complete ===\n\n");
    }

    VulkanRender::~VulkanRender() {
        //Shutdown();
    }

    // ============================================================================
    // INITIALIZATION METHODS
    // ============================================================================

    bool VulkanRender::initializeInstance() {
        vkb::InstanceBuilder builder;
#ifdef DEBUG
        bool enableValidation = true;
#else
        bool enableValidation = false;
#endif

        auto instRet = builder
            .set_app_name("OnYuu Engine")
            .request_validation_layers(enableValidation)
            .use_default_debug_messenger()
            .require_api_version(1, 3, 0)
            .build();

        if (!instRet) {
            std::cerr << "Failed to create Vulkan instance: "
                << instRet.error().message() << "\n";
            return false;
        }

        instance_ = instRet.value();
        return true;
    }

    bool VulkanRender::initializeSurface() {
        VkResult err = glfwCreateWindowSurface(
            instance_.instance,
            window_,
            nullptr,
            &surface_
        );

        if (err != VK_SUCCESS) {
            const char* errorMsg;
            int ret = glfwGetError(&errorMsg);
            std::cerr << "Failed to create window surface: " << ret;
            if (errorMsg) std::cerr << " - " << errorMsg;
            std::cerr << "\n";
            return false;
        }

        return true;
    }

    bool VulkanRender::initializeAllocator() {
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = device_->getPhysicalDevice();
        allocatorInfo.device = device_->getDevice();
        allocatorInfo.instance = instance_.instance;

        if (vmaCreateAllocator(&allocatorInfo, &allocator_) != VK_SUCCESS) {
            std::cerr << "Failed to create VMA allocator\n";
            return false;
        }

        return true;
    }

    bool VulkanRender::createRenderPass() {
        VkFormat swapchainFormat = swapchain_->getFormat();

        LOG("Creating render pass with swapchain format: " << swapchainFormat << "\n");

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapchainFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = depthFormat_;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthRef{};
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = &depthRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (device_->getDispatch().createRenderPass(&renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
            std::cerr << "Failed to create render pass\n";
            return false;
        }

        return true;
    }

    bool VulkanRender::createDepthResources() {
        VkExtent2D extent = swapchain_->getExtent();
        depthFormat_ = findDepthFormat();

        LOG("Creating depth resources " << extent.width << "x" << extent.height
            << " (format: " << depthFormat_ << ")\n");

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = extent.width;
        imageInfo.extent.height = extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat_;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        if (vmaCreateImage(allocator_, &imageInfo, &allocInfo,
            &depthImage_, &depthAllocation_, nullptr) != VK_SUCCESS) {
            std::cerr << "Failed to create depth image\n";
            return false;
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImage_;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat_;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (device_->getDispatch().createImageView(&viewInfo, nullptr, &depthImageView_) != VK_SUCCESS) {
            std::cerr << "Failed to create depth image view\n";
            return false;
        }

        return true;
    }

    bool VulkanRender::createDescriptorLayouts() {
        // Global descriptor layout
        std::vector<VkDescriptorSetLayoutBinding> globalBindings;

        VkDescriptorSetLayoutBinding timeBinding{};
        timeBinding.binding = 0;
        timeBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        timeBinding.descriptorCount = 1;
        timeBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        globalBindings.push_back(timeBinding);

        VkDescriptorSetLayoutBinding cameraBinding{};
        cameraBinding.binding = 1;
        cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraBinding.descriptorCount = 1;
        cameraBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        globalBindings.push_back(cameraBinding);

        VkDescriptorSetLayoutBinding lightBinding{};
        lightBinding.binding = 2;
        lightBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightBinding.descriptorCount = 1;
        lightBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        globalBindings.push_back(lightBinding);

        VkDescriptorSetLayoutBinding modelBinding{};
        modelBinding.binding = 3;
        modelBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        modelBinding.descriptorCount = 1;
        modelBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        globalBindings.push_back(modelBinding);

        globalDescriptorLayout_ = descriptorManager_->createLayout(globalBindings);
        if (globalDescriptorLayout_ == VK_NULL_HANDLE) {
            std::cerr << "Failed to create global descriptor layout\n";
            return false;
        }

        // Material descriptor layout
        std::vector<VkDescriptorSetLayoutBinding> materialBindings;

        VkDescriptorSetLayoutBinding materialBinding{};
        materialBinding.binding = 0;
        materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        materialBinding.descriptorCount = 1;
        materialBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        materialBindings.push_back(materialBinding);

        VkDescriptorSetLayoutBinding textureBinding{};
        textureBinding.binding = 1;
        textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureBinding.descriptorCount = 120; // MAX_SAMPLERS
        textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        materialBindings.push_back(textureBinding);

        materialDescriptorLayout_ = descriptorManager_->createLayout(materialBindings);
        if (materialDescriptorLayout_ == VK_NULL_HANDLE) {
            std::cerr << "Failed to create material descriptor layout\n";
            return false;
        }

        return true;
    }

    VkFormat VulkanRender::findDepthFormat() {
        std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };

        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(device_->getPhysicalDevice(), format, &props);

            if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                return format;
            }
        }

        return VK_FORMAT_D32_SFLOAT;
    }

    bool VulkanRender::hasFramebufferResize() const {
        if (!window_ || !swapchain_) {
            return false;
        }

        int width = 0, height = 0;
        glfwGetFramebufferSize(window_, &width, &height);

        if (width <= 0 || height <= 0) {
            return false;
        }

        const VkExtent2D extent = swapchain_->getExtent();
        return extent.width != static_cast<uint32_t>(width) ||
            extent.height != static_cast<uint32_t>(height);
    }

    bool VulkanRender::recreateSwapchainResources() {
        if (!swapchain_ || !device_) {
            return false;
        }

        int width = 0, height = 0;
        glfwGetFramebufferSize(window_, &width, &height);

        while (width == 0 || height == 0) {
            glfwWaitEvents();
            glfwGetFramebufferSize(window_, &width, &height);
        }

        device_->waitIdle();

        cleanupDepthResources();

        if (!swapchain_->recreate(surface_, renderPass_, VK_NULL_HANDLE)) {
            std::cerr << "Failed to recreate swapchain (without depth)!\n";
            return false;
        }

        if (!createDepthResources()) {
            std::cerr << "Failed to recreate depth resources!\n";
            return false;
        }

        if (!swapchain_->recreate(surface_, renderPass_, depthImageView_)) {
            std::cerr << "Failed to recreate swapchain (with depth)!\n";
            return false;
        }

        for (auto semaphore : renderFinishedSemaphores) {
            if (semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(device_->getDevice(), semaphore, nullptr);
            }
        }

        renderFinishedSemaphores.assign(swapchain_->getImageCount(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (size_t i = 0; i < renderFinishedSemaphores.size(); ++i) {
            if (vkCreateSemaphore(device_->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
                std::cerr << "Failed to create render finished semaphore for a swapchain image!\n";
                return false;
            }
        }

        imagesInFlight.assign(swapchain_->getImageCount(), VK_NULL_HANDLE);
        swapchainNeedsRecreate_ = false;

        return true;
    }

    // ============================================================================
    // FRAME RENDERING METHODS
    // ============================================================================

    void VulkanRender::BeginFrame() {
        frameNumber++;

        // STEP 0: Processa le operazioni differite dal frame precedente
        // Deve avvenire PRIMA di acquireImage, con la GPU idle assicurata dai fence
        pendingMaterialHandling();

        bool retFlag;
        acquireImageAndBeginFrame(retFlag);
        if (retFlag) return;

        // Step 4: Update descriptors
        updateAllDescriptorSets();

        // Step 5: Begin command buffer
        VkCommandBuffer cmd = commandManager_->getCommandBuffer(currentFrame_);
        commandManager_->reset(currentFrame_);

        if (!commandManager_->begin(currentFrame_)) {
            std::cerr << "Failed to begin command buffer!\n";
            return;
        }
        isFrameRecording_ = true;

        for (size_t i = 0; i < renderScenes.size(); ++i) {
            if (renderScenes[i].target) {
                sceneTarget[renderScenes[i].target].push_back(i);
            }
            else {
                swapChainRenderedScenes.push_back(i);
            }
        }
        renderOnTarget(cmd);
        renderOnSwapChain(cmd);

        if (frameNumber % 60 == 0) {
            geometryPool_->collectGarbage(frameNumber, 180);
        }
    }

    void VulkanRender::renderOnSwapChain(VkCommandBuffer cmd)
    {
        activeColorFormat_ = swapchain_->getFormat();
        activeDepthFormat_ = depthFormat_;
        beginRendering(cmd,
            swapchain_->getFrame(imageIndex_).image,
            swapchain_->getFrame(imageIndex_).view,
            depthImage_, depthImageView_,
            swapchain_->getExtent(), depthFormat_,
            true, VK_IMAGE_LAYOUT_UNDEFINED);
        for (int index : swapChainRenderedScenes) {
            renderScene(cmd, index);
        }
    }

    void VulkanRender::renderOnTarget(VkCommandBuffer cmd)
    {
        for (const auto& [target, indices] : sceneTarget) {
            auto* vulkanTarget = static_cast<VulkanRenderTarget*>(target.get());
            activeColorFormat_ = vulkanTarget->getColorFormat();
            activeDepthFormat_ = vulkanTarget->getDepthFormat();
            const VkImageLayout targetOldLayout = vulkanTarget->getColorLayout(currentFrame_);
            beginRendering(cmd,
                vulkanTarget->getColorImage(currentFrame_),
                vulkanTarget->getColorImageView(currentFrame_),
                vulkanTarget->getDepthImage(currentFrame_),
                vulkanTarget->getDepthImageView(currentFrame_),
                vulkanTarget->getExtent(),
                vulkanTarget->getDepthFormat(),
                false, targetOldLayout);
            vulkanTarget->setColorLayout(currentFrame_, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
            for (int index : indices) {
                renderScene(cmd, index);
            }
            endRendering(cmd, vulkanTarget->getColorImage(currentFrame_), false);
            vulkanTarget->setColorLayout(currentFrame_, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    void VulkanRender::acquireImageAndBeginFrame(bool& retFlag)
    {
        retFlag = true;
        vkWaitForFences(device_->getDevice(), 1, &inFlightFences[currentFrame_], VK_TRUE, UINT64_MAX);

        if (swapchainNeedsRecreate_ || hasFramebufferResize()) {
            LOG("! Swapchain resize detected, recreating...\n");
            if (!recreateSwapchainResources()) {
                return;
            }
        }

        VkResult result = swapchain_->acquireNextImage(imageAvailableSemaphores[currentFrame_], &imageIndex_);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            LOG("! Swapchain acquire returned out-of-date/suboptimal, recreating...\n");

            if (!recreateSwapchainResources()) {
                return;
            }

            result = swapchain_->acquireNextImage(imageAvailableSemaphores[currentFrame_], &imageIndex_);

            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                std::cerr << "Failed to acquire image after recreate! Result: " << result << "\n";
                return;
            }
        }
        else if (result != VK_SUCCESS) {
            std::cerr << "Failed to acquire swapchain image! Result: " << result << "\n";
            return;
        }

        if (imagesInFlight[imageIndex_] != VK_NULL_HANDLE) {
            vkWaitForFences(device_->getDevice(), 1, &imagesInFlight[imageIndex_], VK_TRUE, UINT64_MAX);
        }

        imagesInFlight[imageIndex_] = inFlightFences[currentFrame_];
        vkResetFences(device_->getDevice(), 1, &inFlightFences[currentFrame_]);
        retFlag = false;
    }

    void VulkanRender::pendingMaterialHandling()
    {
        // --- FIX 1: Processa invalidazioni differite ---
        if (!pendingMaterialInvalidations_.empty()) {
            LOG("0. Processing " << pendingMaterialInvalidations_.size()
                << " pending material invalidations...\n");
            device_->waitIdle();

            for (const auto& material : pendingMaterialInvalidations_) {
                auto it = materialResources_.find(material);
                if (it != materialResources_.end()) {
                    auto& res = it->second;
                    if (descriptorManager_ && !res.descriptorSets.empty()) {
                        descriptorManager_->freeSets(res.descriptorSets);
                        res.descriptorSets.clear();
                    }
                    for (auto& ubo : res.ubos) {
                        if (ubo) ubo->shutdown();
                    }
                    res.ubos.clear();
                    materialResources_.erase(it);
                }
            }
            pendingMaterialInvalidations_.clear();
            LOG("  ✓ Pending invalidations processed\n");
        }

        // --- FIX 2: Processa aggiornamenti di texture/uniform differiti ---
        // Questi arrivano dall'observer di modifica materiale quando isFrameRecording_ era true.
        // Qui il fence del frame corrente è già stato aspettato, quindi la GPU non sta
        // più usando i descriptor set che stiamo per aggiornare.
        if (!pendingMaterialUpdates_.empty()) {
            LOG("0. Processing " << pendingMaterialUpdates_.size()
                << " pending material updates...\n");

            // waitIdle garantisce che nessun frame in volo stia ancora usando i set.
            // Necessario perché MAX_FRAMES_IN_FLIGHT può essere > 1.
            device_->waitIdle();

            for (const auto& material : pendingMaterialUpdates_) {
                if (material) {
                    updateMaterialDescriptors(material);
                }
            }
            pendingMaterialUpdates_.clear();
            LOG("  ✓ Pending material updates processed\n");
        }
    }

    void VulkanRender::submit() {
        static int submitNumber = 0;
        submitNumber++;

        VkCommandBuffer cmd = commandManager_->getCommandBuffer(currentFrame_);

        endRendering(cmd, swapchain_->getFrame(imageIndex_).image);
        sceneTarget.clear();
        swapChainRenderedScenes.clear();

        if (!commandManager_->end(currentFrame_)) {
            std::cerr << "✗ Failed to end command buffer!\n";
            isFrameRecording_ = false;
            return;
        }
        isFrameRecording_ = false;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame_] };
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[imageIndex_] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkResult submitResult = vkQueueSubmit(device_->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame_]);
        if (submitResult != VK_SUCCESS) {
            std::cerr << "✗ Failed to submit draw command buffer! Result: " << submitResult << "\n";
            return;
        }

        VkResult result = swapchain_->present(device_->getPresentQueue(), imageIndex_, renderFinishedSemaphores[imageIndex_]);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            LOG("! Swapchain out of date/suboptimal, will recreate next frame\n");
            swapchainNeedsRecreate_ = true;
        }
        else if (result != VK_SUCCESS) {
            std::cerr << "✗ Failed to present! Result: " << result << "\n";
        }
        else {
            if (hasFramebufferResize()) {
                LOG("! Framebuffer size changed, scheduling swapchain recreate\n");
                swapchainNeedsRecreate_ = true;
            }
            LOG("✓ Image presented successfully\n");
        }

        currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
        renderScenes.clear();
    }

    void VulkanRender::beginRendering(VkCommandBuffer cmd,
        VkImage colorImage, VkImageView colorView,
        VkImage depthImage, VkImageView depthView,
        VkExtent2D extent, VkFormat depthFormat,
        bool isSwapchain, VkImageLayout colorOldLayout)
    {
        VkImageMemoryBarrier colorBarrier{};
        colorBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        colorBarrier.oldLayout = colorOldLayout;
        colorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.image = colorImage;
        colorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorBarrier.subresourceRange.baseMipLevel = 0;
        colorBarrier.subresourceRange.levelCount = 1;
        colorBarrier.subresourceRange.baseArrayLayer = 0;
        colorBarrier.subresourceRange.layerCount = 1;
        colorBarrier.srcAccessMask = (colorOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            ? VK_ACCESS_SHADER_READ_BIT : 0;
        colorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkImageMemoryBarrier depthBarrier{};
        depthBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        depthBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        depthBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        depthBarrier.image = depthImage;
        depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || depthFormat == VK_FORMAT_D24_UNORM_S8_UINT) {
            depthBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        depthBarrier.subresourceRange.baseMipLevel = 0;
        depthBarrier.subresourceRange.levelCount = 1;
        depthBarrier.subresourceRange.baseArrayLayer = 0;
        depthBarrier.subresourceRange.layerCount = 1;
        depthBarrier.srcAccessMask = 0;
        depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkImageMemoryBarrier barriers[] = { colorBarrier, depthBarrier };
        vkCmdPipelineBarrier(cmd,
            (colorOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            ? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0, 0, nullptr, 0, nullptr, 2, barriers);

        VkClearValue clearColor = { {0.0f, 0.5f, 1.0f, 1.0f} };
        VkClearValue clearDepth = {};
        clearDepth.depthStencil = { 1.0f, 0 };

        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = colorView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = clearColor;

        VkRenderingAttachmentInfo depthAttachmentInfo{};
        depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthAttachmentInfo.imageView = depthView;
        depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentInfo.clearValue = clearDepth;

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = { 0, 0 };
        renderingInfo.renderArea.extent = extent;
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachmentInfo;

        vkCmdBeginRendering(cmd, &renderingInfo);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = static_cast<float>(extent.height);
        viewport.width = static_cast<float>(extent.width);
        viewport.height = -static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;
        vkCmdSetScissor(cmd, 0, 1, &scissor);
    }

    void VulkanRender::beginRenderPass(VkCommandBuffer cmd) {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass_;
        renderPassInfo.framebuffer = swapchain_->getFrame(imageIndex_).framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapchain_->getExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.5f, 1.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = static_cast<float>(swapchain_->getExtent().height);
        viewport.width = static_cast<float>(swapchain_->getExtent().width);
        viewport.height = -static_cast<float>(swapchain_->getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchain_->getExtent();
        vkCmdSetScissor(cmd, 0, 1, &scissor);
    }

    void VulkanRender::endRenderPass(VkCommandBuffer cmd) {
        vkCmdEndRenderPass(cmd);
    }

    void VulkanRender::endRendering(VkCommandBuffer cmd, VkImage colorImage, bool isSwapchain)
    {
        vkCmdEndRendering(cmd);

        VkImageMemoryBarrier colorBarrier{};
        colorBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        colorBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorBarrier.newLayout = isSwapchain
            ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        colorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.image = colorImage;
        colorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorBarrier.subresourceRange.baseMipLevel = 0;
        colorBarrier.subresourceRange.levelCount = 1;
        colorBarrier.subresourceRange.baseArrayLayer = 0;
        colorBarrier.subresourceRange.layerCount = 1;
        colorBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        colorBarrier.dstAccessMask = isSwapchain ? 0 : VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            isSwapchain ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &colorBarrier);
    }

    void VulkanRender::renderScene(VkCommandBuffer cmd, int sceneIndex) {
        if (sceneIndex < 0 || sceneIndex >= static_cast<int>(renderScenes.size())) {
            return;
        }

        RenderScene& scene = renderScenes[sceneIndex];
        auto& sceneRes = sceneResources_[sceneIndex];

        if (sceneRes.globalDescriptorSets.empty()) {
            return;
        }

        for (const auto& [key, batch] : scene.batches) {
            auto material = key.first;
            auto renderingType = key.second;

            if (batch.empty()) {
                LOG("- Empty batch, skipping\n");
                continue;
            }

            PipelineKey pipelineKey{
                AssetManager::instance().getMaterialPtr(material)->getShader(),
                renderingType,
                activeColorFormat_,
                activeDepthFormat_
            };
            VkPipeline pipeline = getOrCreatePipeline(pipelineKey, AssetManager::instance().getMaterialPtr(material));

            if (pipeline == VK_NULL_HANDLE) {
                std::cerr << "✗ Failed to get pipeline!\n";
                continue;
            }

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            VkPipelineLayout layout = pipelineManager_->getLayout(pipeline);
            auto materialPtr = AssetManager::instance().getMaterialPtr(material);
            if (materialResources_.find(materialPtr) == materialResources_.end()) {
                LOG("⚠ Material resources not yet initialized, initializing now...\n");
                updateMaterialDescriptors(materialPtr);
            }

            if (materialResources_.find(materialPtr) == materialResources_.end()) {
                std::cerr << "✗ Material resources still not available!\n";
                continue;
            }

            std::array<VkDescriptorSet, 2> descriptorSets = {
                sceneRes.globalDescriptorSets[currentFrame_],
                materialResources_[materialPtr].descriptorSets[currentFrame_]
            };

            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout,
                0, descriptorSets.size(), descriptorSets.data(),
                0, nullptr);

            VkBuffer vertexBuffers[] = { geometryPool_->getVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(cmd, geometryPool_->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

            auto indirectBuffer = indirectDrawManager_->getOrCreateBuffer(
                sceneIndex, materialPtr, toPrimitiveTopology(renderingType));
            if (!indirectBuffer->isEmpty()) {
                indirectBuffer->executeMultiDrawIndirect(cmd, currentFrame_);
            }
            else {
                LOG("- Indirect buffer is empty\n");
            }
        }
    }

    // ============================================================================
    // DESCRIPTOR UPDATE METHODS
    // ============================================================================

    void VulkanRender::updateAllDescriptorSets() {
        indirectDrawManager_->resetAll();

        for (size_t i = 0; i < renderScenes.size(); ++i) {
            updateSceneDescriptors(static_cast<int>(i));
        }

        indirectDrawManager_->finalizeAll(currentFrame_);
    }

    void VulkanRender::updateSceneDescriptors(int sceneIndex) {
        auto& scene = renderScenes[sceneIndex];
        auto& sceneRes = sceneResources_[sceneIndex];

        if (sceneRes.globalDescriptorSets.empty()) {
            LOG("[updateSceneDescriptors] Initializing scene " << sceneIndex << " resources\n");

            uint32_t frameCount = swapchain_->getImageCount();
            sceneRes.globalDescriptorSets = descriptorManager_->allocateSets(
                globalDescriptorLayout_, frameCount
            );

            sceneRes.cameraUbos.resize(frameCount);
            sceneRes.lightUbos.resize(frameCount);
            sceneRes.modelMatrixSsbos.resize(frameCount);

            for (uint32_t i = 0; i < frameCount; ++i) {
                sceneRes.cameraUbos[i] = std::make_shared<VulkanUniformBuffer>(
                    1, sizeof(CameraBufferData), allocator_
                );
                sceneRes.lightUbos[i] = std::make_shared<VulkanUniformBuffer>(
                    2, sizeof(LightBufferData), allocator_
                );
                sceneRes.modelMatrixSsbos[i] = std::make_shared<VulkanStorageBuffer>(
                    3, 10000 * sizeof(ModelMatrixData), allocator_
                );

                auto set = sceneRes.globalDescriptorSets[i];
                descriptorManager_->updateBuffer(set, 1,
                    sceneRes.cameraUbos[i]->getVulkanBuffer(), sizeof(CameraBufferData));
                descriptorManager_->updateBuffer(set, 2,
                    sceneRes.lightUbos[i]->getVulkanBuffer(), sizeof(LightBufferData));
                descriptorManager_->updateBuffer(set, 3,
                    sceneRes.modelMatrixSsbos[i]->getVulkanBuffer(), VK_WHOLE_SIZE,
                    0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            }

            LOG("[updateSceneDescriptors] Created " << frameCount << " descriptor sets\n");
        }

        // Update camera
        CameraBufferData cameraData{};
        if (scene.activeCamera) {
            glm::mat4 glToVk = glm::mat4(1.0f);
            glToVk[1][1] = -1.0f;
            glToVk[2][2] = 0.5f;
            glToVk[3][2] = 0.5f;

            cameraData.projection = glToVk * scene.activeCamera->getProjectionMatrix();
            cameraData.view = scene.activeCamera->getViewMatrix();
            cameraData.cameraPosition = glm::vec4(scene.activeCamera->getPosition(), 1.0f);
        }
        else {
            LOG("[updateSceneDescriptors] WARNING: No active camera!\n");
        }
        sceneRes.cameraUbos[currentFrame_]->setData(&cameraData, sizeof(cameraData), BufferUsage::DYNAMIC);

        // Update lights
        LightBufferData lightData{};
        lightData.count = std::min(static_cast<int>(scene.sceneLight.size()), 125);
        for (int i = 0; i < lightData.count; ++i) {
            lightData.lights[i].position = scene.sceneLight[i].position;
            lightData.lights[i].color = scene.sceneLight[i].light.color;
            lightData.lights[i].intensity = scene.sceneLight[i].light.intensity;
        }
        sceneRes.lightUbos[currentFrame_]->setData(&lightData, sizeof(lightData), BufferUsage::DYNAMIC);

        LOG("[updateSceneDescriptors] Updated " << lightData.count << " lights\n");

        // Update model matrices and prepare indirect commands
        std::vector<ModelMatrixData> allModelMatrices;

        for (const auto& [key, batch] : scene.batches) {
            auto material = key.first;
            auto renderingType = key.second;

            std::unordered_map<std::shared_ptr<Mesh>, std::vector<glm::mat4>> meshInstances;
            for (const auto& renderData : batch) {
                meshInstances[renderData.renderMesh->getMesh()].push_back(renderData.model);
            }

            auto indirectBuffer = indirectDrawManager_->getOrCreateBuffer(
                sceneIndex, AssetManager::instance().getMaterialPtr(material),
                toPrimitiveTopology(renderingType));

            for (const auto& [meshPtr, matrices] : meshInstances) {
                geometryPool_->updateMeshUsage(meshPtr, frameNumber);
                uint32_t firstInstance = static_cast<uint32_t>(allModelMatrices.size());

                for (const auto& mat : matrices) {
                    allModelMatrices.push_back({ mat });
                }

                if (pooledMeshes_.find(meshPtr) == pooledMeshes_.end()) {
                    auto pooledMesh = std::make_shared<PooledMeshGPU>(*meshPtr, geometryPool_);
                    pooledMesh->uploadToGPU();
                    pooledMeshes_[meshPtr] = pooledMesh;

                    MeshDrawInfo info;
                    info.vertexRegion = pooledMesh->getVertexRegion();
                    info.indexRegion = pooledMesh->getIndexRegion();
                    info.indexCount = pooledMesh->getIndexCount();
                    info.vertexCount = pooledMesh->getVertexCount();

                    constexpr size_t VERTEX_STRIDE = sizeof(glm::vec3) + sizeof(glm::vec4) +
                        sizeof(glm::vec2) + sizeof(glm::vec3);
                    info.firstIndex = static_cast<uint32_t>(info.indexRegion.offset / sizeof(uint32_t));
                    info.vertexOffset = static_cast<int32_t>(info.vertexRegion.offset / VERTEX_STRIDE);

                    meshDrawInfo_[meshPtr] = info;
                }

                const auto& drawInfo = meshDrawInfo_[meshPtr];

                VkDrawIndexedIndirectCommand cmd{};
                cmd.indexCount = drawInfo.indexCount;
                cmd.instanceCount = static_cast<uint32_t>(matrices.size());
                cmd.firstIndex = drawInfo.firstIndex;
                cmd.vertexOffset = drawInfo.vertexOffset;
                cmd.firstInstance = firstInstance;

                indirectBuffer->addDrawCommand(cmd);
            }
        }

        if (!allModelMatrices.empty()) {
            sceneRes.modelMatrixSsbos[currentFrame_]->setData(
                allModelMatrices.data(),
                allModelMatrices.size() * sizeof(ModelMatrixData),
                BufferUsage::DYNAMIC
            );
        }
    }

    void VulkanRender::updateMaterialDescriptors(std::shared_ptr<Material> material) {
        auto& matRes = materialResources_[material];

        // Alloca descriptor set e UBO se è la prima volta
        if (matRes.descriptorSets.empty()) {
            const uint32_t frameCount = MAX_FRAMES_IN_FLIGHT;
            matRes.descriptorSets = descriptorManager_->allocateSets(
                materialDescriptorLayout_, frameCount
            );

            auto shader = static_cast<VulkanShader*>(material->getShader().get());
            size_t uboSize = shader->getUniformBuffer().size();

            matRes.ubos.resize(frameCount);
            for (uint32_t i = 0; i < frameCount; ++i) {
                matRes.ubos[i] = std::make_shared<VulkanUniformBuffer>(0, uboSize, allocator_);
                descriptorManager_->updateBuffer(matRes.descriptorSets[i], 0,
                    matRes.ubos[i]->getVulkanBuffer(), uboSize);
            }
        }

        // Aggiorna le texture su TUTTI i descriptor set
        // FIX 3: prima era fatto già correttamente per le immagini.
        // Assicuriamoci che avvenga sempre quando ci sono texture.
        const auto& textures = material->getTextures();
        if (!textures.empty()) {
            std::vector<VkDescriptorImageInfo> imageInfos;
            imageInfos.reserve(textures.size());

            for (const auto& tex : textures) {
                auto vkTex = static_cast<VulkanTexture*>(tex.get());
                VkDescriptorImageInfo imgInfo{};
                imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imgInfo.imageView = vkTex->getImageView();
                imgInfo.sampler = vkTex->getSampler();
                imageInfos.push_back(imgInfo);
            }

            // Aggiorna tutti i frame: evita che un frame in volo veda la texture vecchia
            for (const auto& set : matRes.descriptorSets) {
                descriptorManager_->updateImages(set, 1, imageInfos);
            }
        }

        // FIX 3: Aggiorna i dati UBO su TUTTI i frame, non solo currentFrame_.
        // Se venisse aggiornato solo currentFrame_, gli altri frame vedrebbero
        // dati stale causando il flickering.
        auto shader = static_cast<VulkanShader*>(material->getShader().get());
        const auto& uniformData = shader->getUniformBuffer();

        if (!uniformData.empty()) {
            for (auto& ubo : matRes.ubos) {
                ubo->setData(
                    uniformData.data(),
                    uniformData.size(),
                    BufferUsage::DYNAMIC
                );
            }
        }
    }

    VkPipeline VulkanRender::getOrCreatePipeline(const PipelineKey& key,
        std::shared_ptr<Material> material)
    {
        auto it = pipelineCache_.find(key);
        if (it != pipelineCache_.end()) {
            return it->second;
        }

        auto shader = static_cast<VulkanShader*>(key.shader.get());

        VulkanPipelineManager::PipelineConfig config =
            VulkanPipelineManager::PipelineConfig::defaultConfig();

        VkPipelineShaderStageCreateInfo vertStage{};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = shader->getVertexShaderModule();
        vertStage.pName = "main";

        VkPipelineShaderStageCreateInfo fragStage{};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = shader->getFragmentShaderModule();
        fragStage.pName = "main";

        config.shaderStages = { vertStage, fragStage };

        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(glm::vec3) + sizeof(glm::vec4) +
            sizeof(glm::vec2) + sizeof(glm::vec3);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> attributeDescs(4);

        attributeDescs[0].binding = 0;
        attributeDescs[0].location = 0;
        attributeDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescs[0].offset = 0;

        attributeDescs[1].binding = 0;
        attributeDescs[1].location = 1;
        attributeDescs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescs[1].offset = sizeof(glm::vec3);

        attributeDescs[2].binding = 0;
        attributeDescs[2].location = 2;
        attributeDescs[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescs[2].offset = sizeof(glm::vec3) + sizeof(glm::vec4);

        attributeDescs[3].binding = 0;
        attributeDescs[3].location = 3;
        attributeDescs[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescs[3].offset = sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec2);

        std::vector<VkVertexInputBindingDescription> bindings = { bindingDesc };

        config.vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        config.vertexInput.vertexBindingDescriptionCount = bindings.size();
        config.vertexInput.pVertexBindingDescriptions = bindings.data();
        config.vertexInput.vertexAttributeDescriptionCount = attributeDescs.size();
        config.vertexInput.pVertexAttributeDescriptions = attributeDescs.data();

        config.inputAssembly.topology = toPrimitiveTopology(key.renderingType);
        config.descriptorLayouts = { globalDescriptorLayout_, materialDescriptorLayout_ };

        VkPushConstantRange pushConstant{};
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(glm::mat4);
        config.pushConstants = { pushConstant };

        config.renderPass = VK_NULL_HANDLE;
        config.colorAttachmentFormats = { key.colorFormat };
        config.depthAttachmentFormat = key.depthFormat;

        VkPipeline pipeline = pipelineManager_->createGraphicsPipeline(config);

        if (pipeline != VK_NULL_HANDLE) {
            pipelineCache_[key] = pipeline;
        }

        return pipeline;
    }

    VkCommandBuffer VulkanRender::beginSingleTimeCommands() {
        return commandManager_->beginSingleTime();
    }

    void VulkanRender::endSingleTimeCommands(VkCommandBuffer cmd) {
        commandManager_->endSingleTime(cmd, device_->getGraphicsQueue());
    }

    void VulkanRender::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
        VkCommandBuffer cmd = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;

        vkCmdCopyBuffer(cmd, src, dst, 1, &copyRegion);

        endSingleTimeCommands(cmd);
    }

    void VulkanRender::setSkyBox(SkyBoxComponent* skybox) {
        BatchRender::setSkyBox(skybox);
    }

    void VulkanRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) {
        BatchRender::addMeshRender(mesh, model);
    }

    void VulkanRender::registeringCallbacks()
    {
        materialCreateObserverID_ = AssetManager::instance().registerOnMaterialCreationObserver(
            [this](const std::string& materialId, bool) {
                auto material = AssetManager::instance().getMaterialPtr(materialId);
                material->apply();
                material->bind();

                // La creazione di un materiale avviene normalmente fuori dal frame,
                // ma gestiamo il caso in cui avvenga durante il recording per sicurezza.
                if (isFrameRecording_) {
                    pendingMaterialUpdates_.push_back(material);
                    return;
                }
                updateMaterialDescriptors(material);
            });

        materialModifyObserverID_ = AssetManager::instance().registerOnMaterialModificationObserver(
            [this](const std::string& materialId, bool) {
                auto material = AssetManager::instance().getMaterialPtr(materialId);
                material->apply();
                material->bind();

                // FIX 2: Se il frame è in recording la GPU sta usando i descriptor set
                // di questo materiale. Differisci l'aggiornamento al prossimo BeginFrame,
                // dove il fence garantisce che la GPU abbia finito.
                if (isFrameRecording_) {
                    LOG("Material modification deferred (frame is recording): " << materialId << "\n");
                    pendingMaterialUpdates_.push_back(material);
                    return;
                }
                updateMaterialDescriptors(material);
            });

        materialDeleteObserverID_ = AssetManager::instance().registerOnMaterialRemovalObserver(
            [this](const std::string& materialId, bool) {
                std::cout << "Material removed: " << materialId << "\n";
                invalidateMaterial(AssetManager::instance().getMaterialPtr(materialId));
            });
    }

    void VulkanRender::removeCachedMesh(const std::shared_ptr<Mesh>& mesh) {
        auto it = pooledMeshes_.find(mesh);
        if (it != pooledMeshes_.end()) {
            std::cout << "Removing cached mesh from GPU\n";
            it->second->shutdown();
            pooledMeshes_.erase(it);
            meshDrawInfo_.erase(mesh);
        }
    }

    void VulkanRender::cleanupDepthResources() {
        if (depthImageView_ != VK_NULL_HANDLE) {
            device_->getDispatch().destroyImageView(depthImageView_, nullptr);
            depthImageView_ = VK_NULL_HANDLE;
        }

        if (depthImage_ != VK_NULL_HANDLE) {
            vmaDestroyImage(allocator_, depthImage_, depthAllocation_);
            depthImage_ = VK_NULL_HANDLE;
            depthAllocation_ = VK_NULL_HANDLE;
        }
    }

    void VulkanRender::cleanupDescriptorLayouts() {
        if (globalDescriptorLayout_ != VK_NULL_HANDLE) {
            descriptorManager_->destroyLayout(globalDescriptorLayout_);
            globalDescriptorLayout_ = VK_NULL_HANDLE;
        }

        if (materialDescriptorLayout_ != VK_NULL_HANDLE) {
            descriptorManager_->destroyLayout(materialDescriptorLayout_);
            materialDescriptorLayout_ = VK_NULL_HANDLE;
        }
    }

    void VulkanRender::invalidateShader(const std::shared_ptr<Shader>& shader)
    {
        if (!shader || !device_ || !device_->isValid()) {
            return;
        }

        device_->waitIdle();

        if (pipelineManager_) {
            for (auto it = pipelineCache_.begin(); it != pipelineCache_.end();) {
                if (it->first.shader == shader) {
                    if (it->second != VK_NULL_HANDLE) {
                        pipelineManager_->destroyPipeline(it->second);
                    }
                    it = pipelineCache_.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        std::vector<std::shared_ptr<Material>> materialsToInvalidate;
        materialsToInvalidate.reserve(materialResources_.size());
        for (const auto& [mat, res] : materialResources_) {
            if (mat && mat->getShader() == shader) {
                materialsToInvalidate.push_back(mat);
            }
        }

        if (isFrameRecording_) {
            LOG("Shader invalidation: " << materialsToInvalidate.size()
                << " materials deferred to next frame\n");
            for (const auto& mat : materialsToInvalidate) {
                pendingMaterialInvalidations_.push_back(mat);
            }
            return;
        }

        for (const auto& mat : materialsToInvalidate) {
            invalidateMaterial(mat);
        }
    }

    void VulkanRender::invalidateShaderByName(const std::string& shaderName)
    {
        if (!device_ || !device_->isValid()) return;

        device_->waitIdle();

        for (auto it = pipelineCache_.begin(); it != pipelineCache_.end();) {
            std::string keyName;
            auto shaderPtr = it->first.shader;
            if (shaderPtr) {
                keyName = OnYuu::AssetManager::instance().findShaderNameForShader(shaderPtr);
            }

            if (!keyName.empty() && keyName == shaderName) {
                if (it->second != VK_NULL_HANDLE) pipelineManager_->destroyPipeline(it->second);
                it = pipelineCache_.erase(it);
            }
            else {
                ++it;
            }
        }

        std::vector<std::shared_ptr<Material>> materialsToInvalidate;
        for (const auto& [mat, res] : materialResources_) {
            if (!mat) continue;
            bool matches = false;

            auto mShaderPtr = mat->getShader();
            if (mShaderPtr) {
                auto registeredName = OnYuu::AssetManager::instance().findShaderNameForShader(mShaderPtr);
                if (!registeredName.empty() && registeredName == shaderName) {
                    matches = true;
                }
            }

            if (!matches) {
                const auto& allMats = OnYuu::AssetManager::instance().getMaterials();
                for (const auto& [matName, matPtr] : allMats) {
                    if (matPtr == mat) {
                        const auto* md = OnYuu::AssetManager::instance().getMaterialMetadata(matName);
                        if (md && md->shaderName == shaderName) {
                            matches = true;
                        }
                        break;
                    }
                }
            }

            if (matches) materialsToInvalidate.push_back(mat);
        }

        if (isFrameRecording_) {
            for (const auto& mat : materialsToInvalidate) pendingMaterialInvalidations_.push_back(mat);
            return;
        }

        for (const auto& mat : materialsToInvalidate) {
            invalidateMaterial(mat);
        }
    }

    void VulkanRender::invalidateMaterial(const std::shared_ptr<Material>& material)
    {
        if (!material || !device_ || !device_->isValid()) {
            return;
        }

        auto it = materialResources_.find(material);
        if (it == materialResources_.end()) {
            return;
        }

        if (isFrameRecording_) {
            LOG("Material invalidation deferred (frame is recording)\n");
            pendingMaterialInvalidations_.push_back(material);
            return;
        }

        device_->waitIdle();

        auto& res = it->second;
        if (descriptorManager_ && !res.descriptorSets.empty()) {
            descriptorManager_->freeSets(res.descriptorSets);
            res.descriptorSets.clear();
        }

        for (auto& ubo : res.ubos) {
            if (ubo) ubo->shutdown();
        }
        res.ubos.clear();

        materialResources_.erase(it);
    }

    void VulkanRender::Shutdown() {

        if (!device_ || !device_->isValid()) {
            return;
        }

        LOG("VulkanRender: Beginning shutdown...\n");

        device_->waitIdle();
		AssetManager::instance().unregisterOnMaterialCreationObserver(materialCreateObserverID_);
		AssetManager::instance().unregisterOnMaterialModificationObserver(materialModifyObserverID_);
		AssetManager::instance().unregisterOnMaterialRemovalObserver(materialDeleteObserverID_);    
        LOG("✓ GPU idle\n");

        LOG("Freeing descriptor sets...\n");
        for (auto& [sceneIdx, res] : sceneResources_) {
            if (!res.globalDescriptorSets.empty()) {
                descriptorManager_->freeSets(res.globalDescriptorSets);
                res.globalDescriptorSets.clear();
            }
        }

        for (auto& [mat, res] : materialResources_) {
            if (!res.descriptorSets.empty()) {
                descriptorManager_->freeSets(res.descriptorSets);
                res.descriptorSets.clear();
            }
        }
        LOG("✓ Descriptor sets freed\n");

        LOG("Cleaning up geometry pool...\n");
        for (auto& [meshPtr, pooledMesh] : pooledMeshes_) {
            if (pooledMesh) pooledMesh->shutdown();
        }
        pooledMeshes_.clear();
        meshDrawInfo_.clear();

        if (indirectDrawManager_) {
            indirectDrawManager_->shutdown();
            indirectDrawManager_.reset();
        }

        if (geometryPool_) {
            geometryPool_->shutdown();
            geometryPool_.reset();
        }
        LOG("✓ Geometry pool cleaned\n");

        LOG("Cleaning up scene resources...\n");
        for (auto& [sceneIdx, res] : sceneResources_) {
            for (auto& ssbo : res.modelMatrixSsbos) { if (ssbo) ssbo->shutdown(); }
            res.modelMatrixSsbos.clear();
            for (auto& ubo : res.lightUbos) { if (ubo) ubo->shutdown(); }
            res.lightUbos.clear();
            for (auto& ubo : res.cameraUbos) { if (ubo) ubo->shutdown(); }
            res.cameraUbos.clear();
        }
        sceneResources_.clear();
        LOG("✓ Scene resources cleaned\n");

        LOG("Cleaning up material resources...\n");
        for (auto& [mat, res] : materialResources_) {
            for (auto& ubo : res.ubos) { if (ubo) ubo->shutdown(); }
            res.ubos.clear();
        }
        materialResources_.clear();
        LOG("✓ Material resources cleaned\n");

        device_->waitIdle();

        LOG("Cleaning up pipelines...\n");
        for (auto& [key, pipeline] : pipelineCache_) {
            if (pipeline != VK_NULL_HANDLE) {
                pipelineManager_->destroyPipeline(pipeline);
            }
            if (key.shader) {
                key.shader->shutdown();
            }
        }
        pipelineCache_.clear();
        LOG("✓ Pipelines cleaned\n");

        LOG("Cleaning up descriptor layouts...\n");
        cleanupDescriptorLayouts();
        LOG("✓ Descriptor layouts cleaned\n");

        LOG("Shutting down pipeline manager...\n");
        if (pipelineManager_) { pipelineManager_->shutdown(); pipelineManager_.reset(); }

        LOG("Shutting down descriptor manager...\n");
        if (descriptorManager_) { descriptorManager_->shutdown(); descriptorManager_.reset(); }

        LOG("Shutting down sync manager...\n");
        if (syncManager_) { syncManager_->shutdown(); syncManager_.reset(); }

        for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
            vkDestroySemaphore(device_->getDevice(), renderFinishedSemaphores[i], nullptr);
        }
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device_->getDevice(), imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device_->getDevice(), inFlightFences[i], nullptr);
        }
        imagesInFlight.clear();

        LOG("Shutting down command manager...\n");
        if (commandManager_) { commandManager_->shutdown(); commandManager_.reset(); }

        LOG("Shutting down swapchain...\n");
        if (swapchain_) { swapchain_->shutdown(); swapchain_.reset(); }
        LOG("✓ Swapchain shutdown\n");

        LOG("Cleaning up depth resources and render pass...\n");
        cleanupDepthResources();

        if (renderPass_ != VK_NULL_HANDLE) {
            device_->getDispatch().destroyRenderPass(renderPass_, nullptr);
            renderPass_ = VK_NULL_HANDLE;
        }
        LOG("✓ Depth and render pass cleaned\n");

        LOG("Destroying VMA allocator...\n");
        if (allocator_ != VK_NULL_HANDLE) {
#ifdef _DEBUG
            VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
            vmaGetHeapBudgets(allocator_, budgets);
            LOG("VMA Budget Report:\n");
            for (uint32_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i) {
                if (budgets[i].usage > 0) {
                    LOG("  Heap " << i << ": "
                        << (budgets[i].usage / 1024.0 / 1024.0) << " MB still allocated!\n");
                }
            }
#endif
            vmaDestroyAllocator(allocator_);
            allocator_ = VK_NULL_HANDLE;
        }
        LOG("✓ VMA allocator destroyed\n");

        LOG("Shutting down device...\n");
        if (device_) { device_->shutdown(); device_.reset(); }

        LOG("Cleaning up surface and instance...\n");
        if (surface_ != VK_NULL_HANDLE) {
            vkb::destroy_surface(instance_, surface_);
            surface_ = VK_NULL_HANDLE;
        }

        if (instance_.instance != VK_NULL_HANDLE) {
            vkb::destroy_instance(instance_);
            instance_ = {};
        }

        LOG("VulkanRender: Shutdown complete ✓\n");
    }

} // namespace OnYuu