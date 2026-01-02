#pragma once

#include <vulkan/vulkan.h>
#include "vulkan-bts/VkBootstrap.h"
#include <vector>
#include <string>
#include <iostream>

namespace OnYuu {

    /**
     * VulkanDevice - Gestisce device, physical device e queue
     * Responsabilità: Creazione device, gestione queue, dispatch table
     */
    class VulkanDevice {
    public:
        struct Config {
            std::vector<const char*> requiredExtensions = {
                "VK_EXT_descriptor_indexing",
                "VK_KHR_maintenance3"
            };
            bool enableValidation = true;
            uint32_t minVulkanVersion = VK_MAKE_VERSION(1, 2, 0);
        };

        VulkanDevice() = default;
        ~VulkanDevice();

        // Non copiabile
        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice& operator=(const VulkanDevice&) = delete;

        // Movible
        VulkanDevice(VulkanDevice&& other) noexcept;
        VulkanDevice& operator=(VulkanDevice&& other) noexcept;

        /**
         * Inizializza il device Vulkan
         * @param instance Instance Vulkan già creata
         * @param surface Surface per il presentation
         * @param config Configurazione device
         * @return true se successo, false altrimenti
         */
        bool initialize(vkb::Instance& instance, VkSurfaceKHR surface, const Config& config = Config{});

        /**
         * Libera tutte le risorse
         */
        void shutdown();

        // Getters
        VkDevice getDevice() const { return device_.device; }
        VkPhysicalDevice getPhysicalDevice() const { return device_.physical_device; }
        VkQueue getGraphicsQueue() const { return graphicsQueue_; }
        VkQueue getPresentQueue() const { return presentQueue_; }
        uint32_t getGraphicsQueueFamily() const { return graphicsQueueFamily_; }
        uint32_t getPresentQueueFamily() const { return presentQueueFamily_; }

        const vkb::DispatchTable& getDispatch() const { return dispatch_; }
        const vkb::Device& getVkbDevice() const { return device_; }

        /**
         * Attende che il device finisca tutte le operazioni
         */
        void waitIdle() const;

        /**
         * Verifica se il device è inizializzato
         */
        bool isValid() const { return device_.device != VK_NULL_HANDLE; }

    private:
        vkb::Device device_;
        vkb::DispatchTable dispatch_;

        VkQueue graphicsQueue_ = VK_NULL_HANDLE;
        VkQueue presentQueue_ = VK_NULL_HANDLE;
        uint32_t graphicsQueueFamily_ = 0;
        uint32_t presentQueueFamily_ = 0;

        void reset();
    };

} // namespace OnYuu