#pragma once

#include "VulkanDevice.h"
#include <vector>

namespace OnYuu {

    /**
     * VulkanCommandManager - Gestisce command pools e command buffers
     * Responsabilità: Creazione/gestione command pool, allocazione buffer, single-time commands
     */
    class VulkanCommandManager {
    public:
        explicit VulkanCommandManager(VulkanDevice* device);
        ~VulkanCommandManager();

        // Non copiabile
        VulkanCommandManager(const VulkanCommandManager&) = delete;
        VulkanCommandManager& operator=(const VulkanCommandManager&) = delete;

        /**
         * Inizializza command pools e buffers
         * @param queueFamily Queue family per i command pool
         * @param frameCount Numero di frame in flight
         * @return true se successo
         */
        bool initialize(uint32_t queueFamily, uint32_t frameCount);

        /**
         * Libera tutte le risorse
         */
        void shutdown();

        /**
         * Ottiene il command buffer per un frame specifico
         * @param frameIndex Indice del frame
         * @return Command buffer
         */
        VkCommandBuffer getCommandBuffer(uint32_t frameIndex) const;

        /**
         * Inizia un command buffer single-time (per operazioni immediate)
         * @return Command buffer pronto per la registrazione
         */
        VkCommandBuffer beginSingleTime();

        /**
         * Termina ed esegue un command buffer single-time
         * @param cmd Command buffer da terminare
         * @param queue Queue su cui eseguire
         */
        void endSingleTime(VkCommandBuffer cmd, VkQueue queue);

        /**
         * Resetta un command buffer
         * @param frameIndex Indice del frame
         */
        void reset(uint32_t frameIndex);

        /**
         * Inizia la registrazione su un command buffer
         * @param frameIndex Indice del frame
         * @return true se successo
         */
        bool begin(uint32_t frameIndex);

        /**
         * Termina la registrazione su un command buffer
         * @param frameIndex Indice del frame
         * @return true se successo
         */
        bool end(uint32_t frameIndex);

        /**
         * Inizia la registrazione con flags specifiche
         * @param frameIndex Indice del frame
         * @param flags Command buffer usage flags
         * @return true se successo
         */
        bool begin(uint32_t frameIndex, VkCommandBufferUsageFlags flags);

        bool isValid() const { return commandPool_ != VK_NULL_HANDLE; }

    private:
        VulkanDevice* device_;
        VkCommandPool commandPool_ = VK_NULL_HANDLE;
        VkCommandPool singleTimePool_ = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers_;
    };

} // namespace OnYuu