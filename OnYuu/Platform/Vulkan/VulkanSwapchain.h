#pragma once

#include "VulkanDevice.h"
#include <vector>

namespace OnYuu {

    /**
     * VulkanSwapchain - Gestisce swapchain, images, views e framebuffers
     * Responsabilità: Creazione swapchain, gestione images, present
     */
    class VulkanSwapchain {
    public:
        struct FrameData {
            VkImage image = VK_NULL_HANDLE;
            VkImageView view = VK_NULL_HANDLE;
            VkFramebuffer framebuffer = VK_NULL_HANDLE;
        };

        struct Config {
            VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
            VkFormat preferredFormat = VK_FORMAT_R8G8B8A8_UNORM;
            VkColorSpaceKHR preferredColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
            uint32_t minImageCount = 3;
        };

        explicit VulkanSwapchain(VulkanDevice* device);
        ~VulkanSwapchain();

        // Non copiabile
        VulkanSwapchain(const VulkanSwapchain&) = delete;
        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

        /**
         * Crea la swapchain
         * @param surface Surface per il presentation
         * @param renderPass Render pass per i framebuffer
         * @param depthView Depth image view
         * @param config Configurazione swapchain
         * @return true se successo
         */
        bool create(VkSurfaceKHR surface, VkRenderPass renderPass,
            VkImageView depthView, const Config& config = Config{});

        /**
         * Ricrea la swapchain (es. dopo resize)
         * @return true se successo
         */
        bool recreate(VkSurfaceKHR surface, VkRenderPass renderPass, VkImageView depthView);

        /**
         * Libera tutte le risorse
         */
        void shutdown();

        /**
         * Acquisisce la prossima immagine dalla swapchain
         * @param semaphore Semaforo da segnalare quando l'immagine è disponibile
         * @param imageIndex Output: indice dell'immagine acquisita
         * @return VkResult dell'operazione
         */
        VkResult acquireNextImage(VkSemaphore semaphore, uint32_t* imageIndex);

        /**
         * Presenta l'immagine renderizzata
         * @param queue Queue per il present
         * @param imageIndex Indice dell'immagine da presentare
         * @param waitSemaphore Semaforo da attendere prima del present
         * @return VkResult dell'operazione
         */
        VkResult present(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore);

        // Getters
        VkExtent2D getExtent() const { return swapchain_.extent; }
        VkFormat getFormat() const { return swapchain_.image_format; }
        uint32_t getImageCount() const { return static_cast<uint32_t>(frames_.size()); }
        const FrameData& getFrame(uint32_t index) const { return frames_[index]; }
        VkSwapchainKHR getHandle() const { return swapchain_.swapchain; }

        bool isValid() const { return swapchain_.swapchain != VK_NULL_HANDLE; }

    private:
        bool createFramebuffers(VkRenderPass renderPass, VkImageView depthView);
        void cleanup();

        VulkanDevice* device_;
        vkb::Swapchain swapchain_;
        std::vector<FrameData> frames_;

        // Salva parametri per recreate
        VkSurfaceKHR lastSurface_ = VK_NULL_HANDLE;
        VkRenderPass lastRenderPass_ = VK_NULL_HANDLE;
        VkImageView lastDepthView_ = VK_NULL_HANDLE;
        Config config_;
    };

} // namespace OnYuu