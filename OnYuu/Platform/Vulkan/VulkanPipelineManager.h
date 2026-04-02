#pragma once

#include "VulkanDevice.h"
#include <unordered_map>
#include <vector>

namespace OnYuu {

    /**
     * VulkanPipelineManager - Gestisce graphics pipelines e pipeline layouts
     * Responsabilità: Creazione/gestione pipeline, cache, associazione layout
     */
    class VulkanPipelineManager {
    public:
        struct PipelineConfig {
            // Shader stages
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

            // Vertex input
            VkPipelineVertexInputStateCreateInfo vertexInput{};

            // Input assembly
            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

            // Viewport/Scissor (dynamic states)
            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            // Rasterization
            VkPipelineRasterizationStateCreateInfo rasterizer{};

            // Multisampling
            VkPipelineMultisampleStateCreateInfo multisampling{};

            // Depth/Stencil
            VkPipelineDepthStencilStateCreateInfo depthStencil{};

            // Color blending
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};

            // Descriptor layouts
            std::vector<VkDescriptorSetLayout> descriptorLayouts;

            // Push constants
            std::vector<VkPushConstantRange> pushConstants;

            // Render pass (legacy)
            VkRenderPass renderPass = VK_NULL_HANDLE;
            uint32_t subpass = 0;

            // Dynamic Rendering config
            std::vector<VkFormat> colorAttachmentFormats;
            VkFormat depthAttachmentFormat = VK_FORMAT_UNDEFINED;
            VkFormat stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

            // Helper per inizializzare valori di default
            static PipelineConfig defaultConfig();
        };

        explicit VulkanPipelineManager(VulkanDevice* device);
        ~VulkanPipelineManager();

        // Non copiabile
        VulkanPipelineManager(const VulkanPipelineManager&) = delete;
        VulkanPipelineManager& operator=(const VulkanPipelineManager&) = delete;

        /**
         * Crea una graphics pipeline
         * @param config Configurazione pipeline
         * @param outLayout Output: pipeline layout creato
         * @return Handle della pipeline (VK_NULL_HANDLE se errore)
         */
        VkPipeline createGraphicsPipeline(const PipelineConfig& config,
            VkPipelineLayout* outLayout = nullptr);

        /**
         * Distrugge una pipeline
         * @param pipeline Pipeline da distruggere
         */
        void destroyPipeline(VkPipeline pipeline);

        /**
         * Ottiene il layout associato a una pipeline
         * @param pipeline Pipeline
         * @return Pipeline layout (VK_NULL_HANDLE se non trovato)
         */
        VkPipelineLayout getLayout(VkPipeline pipeline) const;

        /**
         * Libera tutte le risorse
         */
        void shutdown();

        bool isValid() const { return device_ != nullptr && device_->isValid(); }

    private:
        VulkanDevice* device_;
        std::unordered_map<VkPipeline, VkPipelineLayout> pipelineLayouts_;
        std::vector<VkPipeline> pipelines_; // Per tracking
        std::vector<VkPipelineLayout> layouts_; // Per cleanup
    };

} // namespace OnYuu