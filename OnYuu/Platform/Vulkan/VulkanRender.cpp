#include "VulkanRender.h"
#include "Application/Application.h"
#include "Core/View/View.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Platform/Vulkan/VulkanBufferPool.h"
#include "IndirectDrawSystem.h"
#include <iostream>
#include <array>
#include <unordered_set>




namespace OnYuu {

    // ============================================================================
    // CONSTRUCTOR & DESTRUCTOR
    // ============================================================================

    VulkanRender::VulkanRender() : BatchRender() {
        window_ = static_cast<GLFWwindow*>(
            Application::getInstance()->getWindow()->getNativeWindow()
            );

         LOG( "=== VulkanRender Initialization Started ===\n");

        // STEP 1: Instance
        if (!initializeInstance()) {
            throw std::runtime_error("Failed to initialize Vulkan instance");
        }
        LOG( << "✓ Instance created\n");

        // STEP 2: Surface
        if (!initializeSurface()) {
            throw std::runtime_error("Failed to initialize surface");
        }
        LOG( << "✓ Surface created\n");

        // STEP 3: Device
        device_ = std::make_unique<VulkanDevice>();
        if (!device_->initialize(instance_, surface_)) {
            throw std::runtime_error("Failed to initialize Vulkan device");
        }
        LOG( << "✓ Device created\n");

        // STEP 4: Allocator (BEFORE swapchain for depth buffer)
        if (!initializeAllocator()) {
            throw std::runtime_error("Failed to initialize VMA allocator");
        }
        LOG( << "✓ VMA allocator created\n");

        // STEP 5: Swapchain TEMPORANEA (per ottenere il format)
        swapchain_ = std::make_unique<VulkanSwapchain>(device_.get());
        VulkanSwapchain::Config swapConfig;

        // Crea swapchain temporanea SENZA render pass
        if (!swapchain_->create(surface_, renderPass_, depthImageView_, swapConfig)) {
            throw std::runtime_error("Failed to create temporary swapchain");
        }
        LOG( << "✓ Temporary swapchain created (format: " << swapchain_->getFormat() << ")\n");

        // STEP 6: Depth resources (usa extent dalla swapchain)
        if (!createDepthResources()) {
            throw std::runtime_error("Failed to create depth resources");
        }
        LOG( << "✓ Depth resources created\n");

        // STEP 7: Render pass (USA il format dalla swapchain!)
        if (!createRenderPass()) {
            throw std::runtime_error("Failed to create render pass");
        }
        LOG( << "✓ Render pass created\n");

        // STEP 8: Ricrea swapchain CON render pass
        swapchain_->shutdown(); // Distruggi temporanea
        if (!swapchain_->create(surface_, renderPass_, depthImageView_, swapConfig)) {
            throw std::runtime_error("Failed to create final swapchain");
        }
        LOG( << "✓ Final swapchain created with " << swapchain_->getImageCount() << " images\n");

        uint32_t frameCount = swapchain_->getImageCount();
        // STEP 12: Descriptor manager
        descriptorManager_ = std::make_unique<VulkanDescriptorManager>(device_.get());
        if (!descriptorManager_->initialize()) {
            throw std::runtime_error("Failed to initialize descriptor manager");
        }
        LOG( << "✓ Descriptor manager initialized\n");
        // STEP 9: Descriptor layouts
        if (!createDescriptorLayouts()) {
            throw std::runtime_error("Failed to create descriptor layouts");
        }
        LOG( << "✓ Descriptor layouts created\n");

        // STEP 10: Command manager
        commandManager_ = std::make_unique<VulkanCommandManager>(device_.get());
        if (!commandManager_->initialize(device_->getGraphicsQueueFamily(), frameCount)) {
            throw std::runtime_error("Failed to initialize command manager");
        }
        LOG( << "✓ Command manager initialized (" << frameCount << " buffers)\n");

        // STEP 11: Sync manager
        syncManager_ = std::make_unique<VulkanSyncManager>(device_.get());
        if (!syncManager_->initialize(MAX_FRAMES_IN_FLIGHT, frameCount)) {
            throw std::runtime_error("Failed to initialize sync manager");
        }
        LOG( << "✓ Sync manager initialized (" << MAX_FRAMES_IN_FLIGHT << " frames)\n");

       

        // STEP 13: Pipeline manager
        pipelineManager_ = std::make_unique<VulkanPipelineManager>(device_.get());
        LOG( << "✓ Pipeline manager initialized\n");

        // STEP 14: Geometry pool e indirect draw
        geometryPool_ = std::make_shared<GeometryPool>(allocator_, this, 128 * 1024 * 1024);
        indirectDrawManager_ = std::make_shared<IndirectDrawManager>(allocator_);
        LOG( << "✓ Geometry pool and indirect draw manager initialized\n");

        LOG( << "=== VulkanRender Initialization Complete ===\n\n");
    }

    VulkanRender::~VulkanRender() {
        //Shutdown();
    }

    // ============================================================================
    // INITIALIZATION METHODS
    // ============================================================================

    bool VulkanRender::initializeInstance() {
        vkb::InstanceBuilder builder;

        auto instRet = builder
            .set_app_name("OnYuu Engine")
            .request_validation_layers(true)
            .use_default_debug_messenger()
            .require_api_version(1, 2, 0)
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
        // IMPORTANTE: Usa il format dalla swapchain!
        VkFormat swapchainFormat = swapchain_->getFormat();

        LOG( << "Creating render pass with swapchain format: " << swapchainFormat << "\n");

        // Color attachment
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapchainFormat; // ← USA FORMAT DALLA SWAPCHAIN!
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Depth attachment
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = depthFormat_;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Attachment references
        VkAttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthRef{};
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = &depthRef;

        // Subpass dependency
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

        // Create render pass
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

        // Find depth format
        depthFormat_ = findDepthFormat();

        LOG( << "Creating depth resources " << extent.width << "x" << extent.height
            << " (format: " << depthFormat_ << ")\n");

        // Create depth image
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

        // Create image view
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
        // Global descriptor layout (camera, lights, model matrices SSBO)
        std::vector<VkDescriptorSetLayoutBinding> globalBindings;

        // Binding 0: Time/Global UBO (opzionale, per ora vuoto)
        VkDescriptorSetLayoutBinding timeBinding{};
        timeBinding.binding = 0;
        timeBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        timeBinding.descriptorCount = 1;
        timeBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        globalBindings.push_back(timeBinding);

        // Binding 1: Camera UBO
        VkDescriptorSetLayoutBinding cameraBinding{};
        cameraBinding.binding = 1;
        cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraBinding.descriptorCount = 1;
        cameraBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        globalBindings.push_back(cameraBinding);

        // Binding 2: Light UBO
        VkDescriptorSetLayoutBinding lightBinding{};
        lightBinding.binding = 2;
        lightBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightBinding.descriptorCount = 1;
        lightBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        globalBindings.push_back(lightBinding);

        // Binding 3: Model matrices SSBO
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

        // Material descriptor layout (material properties, textures)
        std::vector<VkDescriptorSetLayoutBinding> materialBindings;

        // Binding 0: Material properties UBO
        VkDescriptorSetLayoutBinding materialBinding{};
        materialBinding.binding = 0;
        materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        materialBinding.descriptorCount = 1;
        materialBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        materialBindings.push_back(materialBinding);

        // Binding 1: Textures array
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

        return VK_FORMAT_D32_SFLOAT; // Fallback
    }

    // ============================================================================
    // FRAME RENDERING METHODS
    // ============================================================================

    void VulkanRender::BeginFrame() {
        frameNumber++;

        LOG( << "\n=== BeginFrame #" << frameNumber_ << " (currentFrame=" << currentFrame_ << ") ===\n");

        const auto& sync = syncManager_->getFrameSync(currentFrame_);

        // Step 1: Wait for previous frame
        LOG( << "  1. Waiting for fence...\n");
        syncManager_->waitForFence(currentFrame_);
        LOG( << "     ✓ Fence signaled\n");

        // Step 2: Acquire image
        LOG( << "  2. Acquiring swapchain image...\n");
        VkResult result = swapchain_->acquireNextImage(sync.imageAvailable, &imageIndex_);


        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            LOG(<< "     ! Swapchain out of date, recreating...\n");

            device_->waitIdle();

            cleanupDepthResources();

            swapchain_->recreate(surface_, renderPass_, VK_NULL_HANDLE); // ⚠️ Passa NULL temporaneamente

            if (!createDepthResources()) {
                std::cerr << "Failed to recreate depth resources!\n";
                return;
            }

            swapchain_->recreate(surface_, renderPass_, depthImageView_);

            result = swapchain_->acquireNextImage(sync.imageAvailable, &imageIndex_);

            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                std::cerr << "Failed to acquire image after recreate! Result: " << result << "\n";
                syncManager_->resetFence(currentFrame_);
                return;
            }
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            std::cerr << " Failed to acquire swapchain image! Result: " << result << "\n";
            return;
        }

        LOG( << "Acquired image " << imageIndex_ << "\n");

        // Step 3: Reset fence
        LOG( << "  3. Resetting fence...\n");
        syncManager_->resetFence(currentFrame_);
        LOG( << " Fence reset\n");

        // Step 4: Update descriptors
        LOG( << "  4. Updating descriptor sets...\n");
        updateAllDescriptorSets();
        LOG( << "  Descriptors updated\n");

        // Step 5: Begin command buffer
        LOG( << "  5. Recording command buffer...\n");
        VkCommandBuffer cmd = commandManager_->getCommandBuffer(currentFrame_);
        commandManager_->reset(currentFrame_);

        if (!commandManager_->begin(currentFrame_)) {
            std::cerr << " Failed to begin command buffer!\n";
            return;
        }
        LOG( << "  Command buffer recording started\n");

        // Step 6: Begin render pass
        LOG( << "  6. Beginning render pass...\n");
        beginRenderPass(cmd);
        LOG( << "     ✓ Render pass begun (clearing to color)\n");

        // Step 7: Render scenes
        if (renderScenes.empty()) {
            LOG( << "  7. No scenes to render (renderScenes is empty)\n");
        }
        else {
            LOG( << "  7. Rendering " << renderScenes.size() << " scene(s)...\n");
            for (size_t i = 0; i < renderScenes.size(); ++i) {
                LOG( << "     - Rendering scene " << i << "...\n");
                renderScene(cmd, static_cast<int>(i));
            }
            LOG( << "     ✓ All scenes rendered\n");
        }

        if (frameNumber % 60 == 0) {
            geometryPool_->collectGarbage(frameNumber, 180); // Rimuovi mesh non usate da 3 secondi
        }
    }

    void VulkanRender::submit() {
        static int submitNumber = 0;
        submitNumber++;

        LOG(<< "=== Submit #" << submitNumber << " ===\n");

        VkCommandBuffer cmd = commandManager_->getCommandBuffer(currentFrame_);

        // Step 1: End render pass
        LOG(<< "  1. Ending render pass...\n");
        endRenderPass(cmd);
        LOG(<< "     ✓ Render pass ended\n");

        // Step 2: End command buffer
        LOG(<< "  2. Ending command buffer...\n");
        if (!commandManager_->end(currentFrame_)) {
            std::cerr << "     ✗ Failed to end command buffer!\n";
            return;
        }
        LOG(<< "     ✓ Command buffer recording ended\n");

        const auto& frameSync = syncManager_->getFrameSync(currentFrame_);
        const auto& imageSync = syncManager_->getImageSync(imageIndex_);

        // Step 3: Wait per il fence dell'immagine (se in uso da un frame precedente)
        LOG(<< "  3. Waiting for image fence...\n");
        syncManager_->waitForImageFence(imageIndex_);

        // Step 4: Submit to graphics queue
        LOG(<< "  4. Submitting to graphics queue...\n");
        VkSemaphore waitSemaphores[] = { frameSync.imageAvailable };
        VkSemaphore signalSemaphores[] = { imageSync.renderFinished }; // ✅ USA IMMAGINE!
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

        // Segnala sia frame fence che image fence
        VkResult submitResult = vkQueueSubmit(device_->getGraphicsQueue(), 1, &submitInfo, frameSync.inFlight);
        if (submitResult != VK_SUCCESS) {
            std::cerr << "     ✗ Failed to submit draw command buffer! Result: " << submitResult << "\n";
            return;
        }
        LOG(<< "     ✓ Command buffer submitted\n");

        // Step 5: Present
        LOG(<< "  5. Presenting image " << imageIndex_ << "...\n");
        VkResult result = swapchain_->present(device_->getPresentQueue(), imageIndex_, imageSync.renderFinished);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            LOG(<< "     ! Swapchain out of date/suboptimal, will recreate next frame\n");
        }
        else if (result != VK_SUCCESS) {
            std::cerr << "     ✗ Failed to present! Result: " << result << "\n";
        }
        else {
            LOG(<< "     ✓ Image presented successfully\n");
        }

        // Step 6: Advance frame
        uint32_t oldFrame = currentFrame_;
        currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
        LOG(<< "  6. Advanced frame: " << oldFrame << " → " << currentFrame_ << "\n");

        // Step 7: Clear scenes
        renderScenes.clear();
        LOG(<< "  7. Cleared render scenes\n");
        LOG(<< "=== Submit Complete ===\n\n");
    }

    void VulkanRender::beginRenderPass(VkCommandBuffer cmd) {
        LOG( << "     [beginRenderPass] Starting...\n");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass_;
        renderPassInfo.framebuffer = swapchain_->getFrame(imageIndex_).framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapchain_->getExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.5f, 1.0f, 1.0f} }; // BLU ACCESO per debug!
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        LOG( << "     [beginRenderPass] Framebuffer: " << renderPassInfo.framebuffer
            << ", Extent: " << renderPassInfo.renderArea.extent.width << "x"
            << renderPassInfo.renderArea.extent.height << "\n");
        LOG( << "     [beginRenderPass] Clear color: R=" << clearValues[0].color.float32[0]
            << " G=" << clearValues[0].color.float32[1]
            << " B=" << clearValues[0].color.float32[2] << "\n");

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set dynamic viewport and scissor
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain_->getExtent().width);
        viewport.height = static_cast<float>(swapchain_->getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchain_->getExtent();
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        LOG( << "     [beginRenderPass] Viewport & scissor set\n");
    }

    void VulkanRender::endRenderPass(VkCommandBuffer cmd) {
        vkCmdEndRenderPass(cmd);
    }

    void VulkanRender::renderScene(VkCommandBuffer cmd, int sceneIndex) {
        if (sceneIndex < 0 || sceneIndex >= static_cast<int>(renderScenes.size())) {
            LOG( << "       [renderScene] Invalid scene index: " << sceneIndex << "\n");
            return;
        }

        RenderScene& scene = renderScenes[sceneIndex];
        LOG( << "       [renderScene] Scene has " << scene.batches.size() << " batches\n");

        auto& sceneRes = sceneResources_[sceneIndex];

        if (sceneRes.globalDescriptorSets.empty()) {
            LOG( << "       [renderScene] No global descriptor sets, skipping\n");
            return;
        }

        int batchNum = 0;
        for (const auto& [key, batch] : scene.batches) {
            batchNum++;
            auto material = key.first;
            auto renderingType = key.second;

            LOG( << "       [renderScene] Batch " << batchNum << ": "
                << batch.size() << " instances\n");

            if (batch.empty()) {
                LOG( << "         - Empty batch, skipping\n");
                continue;
            }

            // Get pipeline
            PipelineKey pipelineKey{ material->getShader(), renderingType };
            VkPipeline pipeline = getOrCreatePipeline(pipelineKey, material);

            if (pipeline == VK_NULL_HANDLE) {
                std::cerr << "         ✗ Failed to get pipeline!\n";
                continue;
            }

            LOG( << "         - Pipeline: " << pipeline << "\n");

            // Bind pipeline
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            // Bind descriptors
            VkPipelineLayout layout = pipelineManager_->getLayout(pipeline);

            if (materialResources_.find(material) == materialResources_.end()) {
                std::cerr << "         ✗ Material resources not found!\n";
                continue;
            }

            std::array<VkDescriptorSet, 2> descriptorSets = {
                sceneRes.globalDescriptorSets[currentFrame_],
                materialResources_[material].descriptorSets[currentFrame_]
            };

            LOG( << "         - Binding descriptor sets: global="
                << descriptorSets[0] << ", material=" << descriptorSets[1] << "\n");

            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout,
                0, descriptorSets.size(), descriptorSets.data(),
                0, nullptr);

            // Bind geometry
            VkBuffer vertexBuffers[] = { geometryPool_->getVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(cmd, geometryPool_->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

            // Draw
            auto indirectBuffer = indirectDrawManager_->getOrCreateBuffer(material);
            if (!indirectBuffer->isEmpty()) {
                LOG( << "         - Executing indirect draw ("
                    << indirectBuffer->getDrawCount() << " draws)\n");
                indirectBuffer->executeMultiDrawIndirect(cmd);
            }
            else {
                LOG( << "         - Indirect buffer is empty\n");
            }
        }

        indirectDrawManager_->resetAll();
    }

    // ============================================================================
    // DESCRIPTOR UPDATE METHODS
    // ============================================================================

    void VulkanRender::updateAllDescriptorSets() {
        LOG( << "     [updateAllDescriptorSets] Updating for "
            << renderScenes.size() << " scenes\n");

        for (size_t i = 0; i < renderScenes.size(); ++i) {
            updateSceneDescriptors(static_cast<int>(i));
        }

        // Update materials
        std::unordered_set<std::shared_ptr<Material>> usedMaterials;
        for (const auto& scene : renderScenes) {
            for (const auto& [key, batch] : scene.batches) {
                usedMaterials.insert(key.first);
            }
        }

        LOG( << "     [updateAllDescriptorSets] Updating "
            << usedMaterials.size() << " materials\n");

        for (const auto& material : usedMaterials) {
            material->apply();
            material->bind();
            updateMaterialDescriptors(material);
        }
    }

    void VulkanRender::updateSceneDescriptors(int sceneIndex) {
        auto& scene = renderScenes[sceneIndex];
        auto& sceneRes = sceneResources_[sceneIndex];

        // Initialize resources if needed
        if (sceneRes.globalDescriptorSets.empty()) {
            LOG( << "       [updateSceneDescriptors] Initializing scene " << sceneIndex << " resources\n");

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

            LOG( << "       [updateSceneDescriptors] Created " << frameCount << " descriptor sets\n");
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

            LOG( << "       [updateSceneDescriptors] Camera updated\n");
        }
        else {
            LOG( << "       [updateSceneDescriptors] WARNING: No active camera!\n");
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

        LOG( << "       [updateSceneDescriptors] Updated " << lightData.count << " lights\n");

        // Update model matrices and prepare indirect commands
        std::vector<ModelMatrixData> allModelMatrices;

        for (const auto& [key, batch] : scene.batches) {
            auto material = key.first;

            // Group by mesh
            std::unordered_map<std::shared_ptr<Mesh>, std::vector<glm::mat4>> meshInstances;
            for (const auto& renderData : batch) {
                meshInstances[renderData.renderMesh->mesh].push_back(renderData.model);
            }

            auto indirectBuffer = indirectDrawManager_->getOrCreateBuffer(material);

            // For each mesh, create indirect command
            for (const auto& [meshPtr, matrices] : meshInstances) {
                geometryPool_->updateMeshUsage(meshPtr, frameNumber);
                uint32_t firstInstance = static_cast<uint32_t>(allModelMatrices.size());

                // Add all model matrices
                for (const auto& mat : matrices) {
                    allModelMatrices.push_back({ mat });
                }

                // Upload mesh if needed
                if (pooledMeshes_.find(meshPtr) == pooledMeshes_.end()) {
                    auto pooledMesh = std::make_shared<PooledMeshGPU>(
                        *meshPtr, geometryPool_
                    );
                    pooledMesh->uploadToGPU();
                    pooledMeshes_[meshPtr] = pooledMesh;

                    // Save draw info
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

                // Create indirect command
                VkDrawIndexedIndirectCommand cmd{};
                cmd.indexCount = drawInfo.indexCount;
                cmd.instanceCount = static_cast<uint32_t>(matrices.size());
                cmd.firstIndex = drawInfo.firstIndex;
                cmd.vertexOffset = drawInfo.vertexOffset;
                cmd.firstInstance = firstInstance;

                indirectBuffer->addDrawCommand(cmd);
            }
        }

        // Finalize all indirect buffers
        indirectDrawManager_->finalizeAll();

        // Upload model matrices
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

        // Initialize resources if needed
        if (matRes.descriptorSets.empty()) {
            uint32_t frameCount = swapchain_->getImageCount();
            matRes.descriptorSets = descriptorManager_->allocateSets(
                materialDescriptorLayout_, frameCount
            );

            // Create UBOs
            auto shader = static_cast<VulkanShader*>(material->getShader().get());
            size_t uboSize = shader->getUniformBuffer().size();

            matRes.ubos.resize(frameCount);
            for (uint32_t i = 0; i < frameCount; ++i) {
                matRes.ubos[i] = std::make_shared<VulkanUniformBuffer>(0, uboSize, allocator_);

                // Bind material UBO
                descriptorManager_->updateBuffer(matRes.descriptorSets[i], 0,
                    matRes.ubos[i]->getVulkanBuffer(), uboSize);

                // Bind textures if present
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

                    descriptorManager_->updateImages(matRes.descriptorSets[i], 1, imageInfos);
                }
            }
        }

        // Update material properties
        material->apply();
        auto shader = static_cast<VulkanShader*>(material->getShader().get());
        const auto& uniformData = shader->getUniformBuffer();

        if (!uniformData.empty()) {
            matRes.ubos[currentFrame_]->setData(
                uniformData.data(),
                uniformData.size(),
                BufferUsage::DYNAMIC
            );
        }
    }

    // ============================================================================
    // PIPELINE MANAGEMENT
    // ============================================================================

    VkPipeline VulkanRender::getOrCreatePipeline(const PipelineKey& key,
        std::shared_ptr<Material> material) {
        // Check cache
        auto it = pipelineCache_.find(key);
        if (it != pipelineCache_.end()) {
            return it->second;
        }

        // Create new pipeline
        auto shader = static_cast<VulkanShader*>(key.shader.get());

        VulkanPipelineManager::PipelineConfig config =
            VulkanPipelineManager::PipelineConfig::defaultConfig();

        // Shader stages
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

        // Vertex input (position, color, texcoord, normal)
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(glm::vec3) + sizeof(glm::vec4) +
            sizeof(glm::vec2) + sizeof(glm::vec3);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> attributeDescs(4);

        // Position
        attributeDescs[0].binding = 0;
        attributeDescs[0].location = 0;
        attributeDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescs[0].offset = 0;

        // Color
        attributeDescs[1].binding = 0;
        attributeDescs[1].location = 1;
        attributeDescs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescs[1].offset = sizeof(glm::vec3);

        // TexCoord
        attributeDescs[2].binding = 0;
        attributeDescs[2].location = 2;
        attributeDescs[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescs[2].offset = sizeof(glm::vec3) + sizeof(glm::vec4);

        // Normal
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

        // Topology
        switch (key.renderingType) {
        case RenderingTypeEnum::TRIANGLE:
            config.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case RenderingTypeEnum::TRIANGLE_FAN:
            config.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            break;
        case RenderingTypeEnum::TRIANGLE_STRIP:
            config.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        case RenderingTypeEnum::LINE:
            config.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        default:
            config.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }

        // Descriptor layouts
        config.descriptorLayouts = { globalDescriptorLayout_, materialDescriptorLayout_ };

        // Push constants (model matrix)
        VkPushConstantRange pushConstant{};
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(glm::mat4);
        config.pushConstants = { pushConstant };

        config.renderPass = renderPass_;

        VkPipeline pipeline = pipelineManager_->createGraphicsPipeline(config);

        if (pipeline != VK_NULL_HANDLE) {
            pipelineCache_[key] = pipeline;
        }

        return pipeline;
    }

    // ============================================================================
    // HELPER METHODS
    // ============================================================================

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

    // ============================================================================
    // BATCH RENDERER INTERFACE
    // ============================================================================

    void VulkanRender::setSkyBox(SkyBoxComponent* skybox) {
        BatchRender::setSkyBox(skybox);
    }

    void VulkanRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) {
        BatchRender::addMeshRender(mesh, model);
    }


    void VulkanRender::removeCachedMesh(const std::shared_ptr<Mesh>& mesh) {
        //std::lock_guard<std::mutex> lock(meshCacheMutex_); // Aggiungi mutex per thread-safety

        auto it = pooledMeshes_.find(mesh);
        if (it != pooledMeshes_.end()) {
            std::cout << "Removing cached mesh from GPU\n";
            it->second->shutdown(); // Libera risorse GPU
            pooledMeshes_.erase(it);
            meshDrawInfo_.erase(mesh);
        }
    }
    // ============================================================================
    // CLEANUP METHODS
    // ============================================================================

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

    void VulkanRender::Shutdown() {
        if (!device_ || !device_->isValid()) {
            return;
        }

        LOG( << "VulkanRender: Beginning shutdown...\n");

        // ✅ STEP 1: Wait for all GPU operations
        device_->waitIdle();
        LOG( << "  ✓ GPU idle\n");

        // ✅ STEP 2: Free descriptor sets FIRST (before destroying resources they reference)
        LOG( << "  Freeing descriptor sets...\n");
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
        LOG( << "  ✓ Descriptor sets freed\n");

        // ✅ STEP 3: Cleanup geometry resources (VMA allocations!)
        LOG( << "  Cleaning up geometry pool...\n");

        // ✅ FIX: Shutdown PooledMeshGPU objects before clearing
        for (auto& [meshPtr, pooledMesh] : pooledMeshes_) {
            if (pooledMesh) {
                pooledMesh->shutdown(); // Trigger destructor
            }
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
        LOG( << "  ✓ Geometry pool cleaned\n");

        // ✅ STEP 4: Cleanup scene UBO/SSBO resources
        LOG( << "  Cleaning up scene resources...\n");
        for (auto& [sceneIdx, res] : sceneResources_) {
            for (auto& ssbo : res.modelMatrixSsbos) {
                if (ssbo) ssbo->shutdown();
            }
            res.modelMatrixSsbos.clear();

            for (auto& ubo : res.lightUbos) {
                if (ubo) ubo->shutdown();
            }
            res.lightUbos.clear();

            for (auto& ubo : res.cameraUbos) {
                if (ubo) ubo->shutdown();
            }
            res.cameraUbos.clear();
        }
        sceneResources_.clear();
        LOG( << "  ✓ Scene resources cleaned\n");

        // ✅ STEP 5: Cleanup material UBO resources
        LOG( << "  Cleaning up material resources...\n");
        for (auto& [mat, res] : materialResources_) {
            for (auto& ubo : res.ubos) {
                if (ubo) ubo->shutdown();
            }
            res.ubos.clear();
        }
        materialResources_.clear();
        LOG( << "  ✓ Material resources cleaned\n");
		device_->waitIdle();

        // ✅ STEP 6: Cleanup pipelines
        LOG( << "  Cleaning up pipelines...\n");
        for (auto& [key, pipeline] : pipelineCache_) {
            if (pipeline != VK_NULL_HANDLE) {
                pipelineManager_->destroyPipeline(pipeline);
            }
			if (key.shader) {
				key.shader->shutdown();
            }
        }
        pipelineCache_.clear();
        LOG( << "  ✓ Pipelines cleaned\n");

        // ✅ STEP 7: Cleanup descriptor layouts
        LOG( << "  Cleaning up descriptor layouts...\n");
        cleanupDescriptorLayouts();
        LOG( << "  ✓ Descriptor layouts cleaned\n");

        // ✅ STEP 8: Shutdown managers (in reverse order of creation)
        LOG( << "  Shutting down pipeline manager...\n");
        if (pipelineManager_) {
            pipelineManager_->shutdown();
            pipelineManager_.reset();
        }

        LOG( << "  Shutting down descriptor manager...\n");
        if (descriptorManager_) {
            descriptorManager_->shutdown();
            descriptorManager_.reset();
        }

        LOG( << "  Shutting down sync manager...\n");
        if (syncManager_) {
            syncManager_->shutdown();
            syncManager_.reset();
        }

        LOG( << "  Shutting down command manager...\n");
        if (commandManager_) {
            commandManager_->shutdown();
            commandManager_.reset();
        }

        // ✅ STEP 9: Shutdown swapchain (contains framebuffers using depth image!)
        LOG( << "  Shutting down swapchain...\n");
        if (swapchain_) {
            swapchain_->shutdown();
            swapchain_.reset();
        }
        LOG( << "  ✓ Swapchain shutdown\n");

        // ✅ STEP 10: Cleanup depth resources and render pass
        LOG( << "  Cleaning up depth resources and render pass...\n");
        cleanupDepthResources();

        if (renderPass_ != VK_NULL_HANDLE) {
            device_->getDispatch().destroyRenderPass(renderPass_, nullptr);
            renderPass_ = VK_NULL_HANDLE;
        }
        LOG( << "  ✓ Depth and render pass cleaned\n");

        // ✅ STEP 11: Verify allocator is empty and destroy it
        LOG( << "  Destroying VMA allocator...\n");
        if (allocator_ != VK_NULL_HANDLE) {
#ifdef _DEBUG
            // ✅ DEBUG: Print allocator stats before destroying
            VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
            vmaGetHeapBudgets(allocator_, budgets);

            LOG( << "    VMA Budget Report:\n");
            for (uint32_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i) {
                if (budgets[i].usage > 0) {
                    LOG( << "      Heap " << i << ": "
                        << (budgets[i].usage / 1024.0 / 1024.0) << " MB still allocated!\n");
                }
            }
#endif

            vmaDestroyAllocator(allocator_);
            allocator_ = VK_NULL_HANDLE;
        }
        LOG( << "  ✓ VMA allocator destroyed\n");

        // ✅ STEP 12: Shutdown device
        LOG( << "  Shutting down device...\n");
        if (device_) {
            device_->shutdown();
            device_.reset();
        }

        // ✅ STEP 13: Cleanup surface and instance
        LOG( << "  Cleaning up surface and instance...\n");
        if (surface_ != VK_NULL_HANDLE) {
            vkb::destroy_surface(instance_, surface_);
            surface_ = VK_NULL_HANDLE;
        }

        if (instance_.instance != VK_NULL_HANDLE) {
            vkb::destroy_instance(instance_);
            instance_ = {};
        }

        LOG( << "VulkanRender: Shutdown complete ✓\n");
    }

} // namespace OnYuu