#include "VulkanPipelineManager.h"
#include <iostream>

namespace OnYuu {

    VulkanPipelineManager::PipelineConfig VulkanPipelineManager::PipelineConfig::defaultConfig() {
        PipelineConfig config;

        // Input assembly - triangoli
        config.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Rasterizer
        config.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.rasterizer.depthClampEnable = VK_FALSE;
        config.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        config.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        config.rasterizer.lineWidth = 1.0f;
        config.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        config.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        config.rasterizer.depthBiasEnable = VK_FALSE;

        // Multisampling
        config.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.multisampling.sampleShadingEnable = VK_FALSE;
        config.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Depth/Stencil
        config.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config.depthStencil.depthTestEnable = VK_TRUE;
        config.depthStencil.depthWriteEnable = VK_TRUE;
        config.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        config.depthStencil.depthBoundsTestEnable = VK_FALSE;
        config.depthStencil.stencilTestEnable = VK_FALSE;

        // Color blend attachment
        config.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        config.colorBlendAttachment.blendEnable = VK_FALSE;

        return config;
    }

    VulkanPipelineManager::VulkanPipelineManager(VulkanDevice* device)
        : device_(device) {
        if (!device_) {
            throw std::runtime_error("VulkanPipelineManager: device is null");
        }
    }

    VulkanPipelineManager::~VulkanPipelineManager() {
        shutdown();
    }

    VkPipeline VulkanPipelineManager::createGraphicsPipeline(
        const PipelineConfig& config, VkPipelineLayout* outLayout) {

        if (!device_->isValid()) {
            std::cerr << "VulkanPipelineManager: Device not valid\n";
            return VK_NULL_HANDLE;
        }

        const auto& disp = device_->getDispatch();

        // Crea pipeline layout
        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(config.descriptorLayouts.size());
        layoutInfo.pSetLayouts = config.descriptorLayouts.data();
        layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(config.pushConstants.size());
        layoutInfo.pPushConstantRanges = config.pushConstants.data();

        VkPipelineLayout layout;
        if (disp.createPipelineLayout(&layoutInfo, nullptr, &layout) != VK_SUCCESS) {
            std::cerr << "VulkanPipelineManager: Failed to create pipeline layout\n";
            return VK_NULL_HANDLE;
        }

        layouts_.push_back(layout);
        if (outLayout) {
            *outLayout = layout;
        }

        // Dynamic state
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(config.dynamicStates.size());
        dynamicState.pDynamicStates = config.dynamicStates.data();

        // Viewport state (dynamic, quindi valori placeholder)
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = 800.0f;
        viewport.height = 600.0f;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = { 800, 600 };

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Color blend state
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &config.colorBlendAttachment;

        // Crea pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        // Dynamic rendering
        VkPipelineRenderingCreateInfo renderingCreateInfo{};
        if (!config.colorAttachmentFormats.empty() || config.depthAttachmentFormat != VK_FORMAT_UNDEFINED) {
            renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
            renderingCreateInfo.colorAttachmentCount = static_cast<uint32_t>(config.colorAttachmentFormats.size());
            renderingCreateInfo.pColorAttachmentFormats = config.colorAttachmentFormats.data();
            renderingCreateInfo.depthAttachmentFormat = config.depthAttachmentFormat;
            renderingCreateInfo.stencilAttachmentFormat = config.stencilAttachmentFormat;
            pipelineInfo.pNext = &renderingCreateInfo;
        }

        pipelineInfo.stageCount = static_cast<uint32_t>(config.shaderStages.size());
        pipelineInfo.pStages = config.shaderStages.data();
        pipelineInfo.pVertexInputState = &config.vertexInput;
        pipelineInfo.pInputAssemblyState = &config.inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &config.rasterizer;
        pipelineInfo.pMultisampleState = &config.multisampling;
        pipelineInfo.pDepthStencilState = &config.depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = config.renderPass;
        pipelineInfo.subpass = config.subpass;

        VkPipeline pipeline;
        if (disp.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
            std::cerr << "VulkanPipelineManager: Failed to create graphics pipeline\n";
            disp.destroyPipelineLayout(layout, nullptr);
            layouts_.pop_back();
            return VK_NULL_HANDLE;
        }

        pipelines_.push_back(pipeline);
        pipelineLayouts_[pipeline] = layout;

        std::cout << "VulkanPipelineManager: Created pipeline successfully\n";
        return pipeline;
    }

    void VulkanPipelineManager::destroyPipeline(VkPipeline pipeline) {
        if (pipeline == VK_NULL_HANDLE) return;

        const auto& disp = device_->getDispatch();

        // Distruggi pipeline
        disp.destroyPipeline(pipeline, nullptr);

        // Rimuovi dalla lista
        auto it = std::find(pipelines_.begin(), pipelines_.end(), pipeline);
        if (it != pipelines_.end()) {
            pipelines_.erase(it);
        }

        // Rimuovi mapping layout (ma NON distruggerlo qui, lo farà shutdown)
        pipelineLayouts_.erase(pipeline);
    }

    VkPipelineLayout VulkanPipelineManager::getLayout(VkPipeline pipeline) const {
        auto it = pipelineLayouts_.find(pipeline);
        if (it != pipelineLayouts_.end()) {
            return it->second;
        }
        return VK_NULL_HANDLE;
    }

    void VulkanPipelineManager::shutdown() {
        if (!device_->isValid()) {
            return;
        }

        const auto& disp = device_->getDispatch();

        // Distruggi pipelines
        for (auto pipeline : pipelines_) {
            if (pipeline != VK_NULL_HANDLE) {
                disp.destroyPipeline(pipeline, nullptr);
            }
        }
        pipelines_.clear();

        // Distruggi layouts
        for (auto layout : layouts_) {
            if (layout != VK_NULL_HANDLE) {
                disp.destroyPipelineLayout(layout, nullptr);
            }
        }
        layouts_.clear();
        pipelineLayouts_.clear();

        std::cout << "VulkanPipelineManager: Shutdown complete\n";
    }

} // namespace OnYuu