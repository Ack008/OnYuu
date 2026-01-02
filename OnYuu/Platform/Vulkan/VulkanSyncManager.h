#pragma once

#include "VulkanDevice.h"
#include <vector>

namespace OnYuu {

    /**
     * VulkanSyncManager - Gestisce oggetti di sincronizzazione (semaphores, fences)
     * Responsabilità: Creazione/gestione semafori e fence per sincronizzazione frame
     */
    class VulkanSyncManager {
    public:
        struct FrameSync {
            VkSemaphore imageAvailable = VK_NULL_HANDLE;
            VkSemaphore renderFinished = VK_NULL_HANDLE;
            VkFence inFlight = VK_NULL_HANDLE;
        };

        explicit VulkanSyncManager(VulkanDevice* device);
        ~VulkanSyncManager();

        // Non copiabile
        VulkanSyncManager(const VulkanSyncManager&) = delete;
        VulkanSyncManager& operator=(const VulkanSyncManager&) = delete;

        /**
         * Inizializza oggetti di sincronizzazione
         * @param frameCount Numero di frame in flight
         * @return true se successo
         */
        bool initialize(uint32_t frameCount);

        /**
         * Libera tutte le risorse
         */
        void shutdown();

        /**
         * Ottiene gli oggetti di sync per un frame specifico
         * @param frameIndex Indice del frame
         * @return Struttura FrameSync
         */
        const FrameSync& getFrameSync(uint32_t frameIndex) const;

        /**
         * Attende che un fence sia segnalato
         * @param frameIndex Indice del frame
         * @param timeout Timeout in nanosecondi (default: infinito)
         */
        void waitForFence(uint32_t frameIndex, uint64_t timeout = UINT64_MAX);

        /**
         * Resetta un fence
         * @param frameIndex Indice del frame
         */
        void resetFence(uint32_t frameIndex);

        /**
         * Attende che tutti i fence siano segnalati
         * @param timeout Timeout in nanosecondi (default: infinito)
         */
        void waitForAllFences(uint64_t timeout = UINT64_MAX);

        /**
         * Resetta tutti i fence
         */
        void resetAllFences();

        bool isValid() const { return !frameSyncs_.empty(); }
        uint32_t getFrameCount() const { return static_cast<uint32_t>(frameSyncs_.size()); }

    private:
        VulkanDevice* device_;
        std::vector<FrameSync> frameSyncs_;
    };

} // namespace OnYuu