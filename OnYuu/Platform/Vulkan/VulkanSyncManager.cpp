#include "VulkanSyncManager.h"
#include <iostream>

namespace OnYuu {

    VulkanSyncManager::VulkanSyncManager(VulkanDevice* device)
        : device_(device) {
        if (!device_) {
            throw std::runtime_error("VulkanSyncManager: device is null");
        }
    }

    VulkanSyncManager::~VulkanSyncManager() {
        shutdown();
    }

    bool VulkanSyncManager::initialize(uint32_t frameCount, uint32_t swapchainImageCount) {
        if (!device_->isValid()) {
            std::cerr << "VulkanSyncManager: Device not valid\n";
            return false;
        }

        if (frameCount == 0) {
            std::cerr << "VulkanSyncManager: frameCount must be > 0\n";
            return false;
        }

        const auto& disp = device_->getDispatch();

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // ✅ Inizialmente segnalato

        // ===== Frame syncs (per CPU overlap) =====
        frameSyncs_.resize(frameCount);

        for (uint32_t i = 0; i < frameCount; ++i) {
            // Crea semaphore per image available
            if (disp.createSemaphore(&semaphoreInfo, nullptr, &frameSyncs_[i].imageAvailable) != VK_SUCCESS) {
                std::cerr << "VulkanSyncManager: Failed to create imageAvailable semaphore for frame " << i << "\n";
                shutdown();
                return false;
            }

            // Crea semaphore per render finished
            if (disp.createSemaphore(&semaphoreInfo, nullptr, &frameSyncs_[i].renderFinished) != VK_SUCCESS) {
                std::cerr << "VulkanSyncManager: Failed to create renderFinished semaphore for frame " << i << "\n";
                shutdown();
                return false;
            }

            // Crea fence per in-flight
            if (disp.createFence(&fenceInfo, nullptr, &frameSyncs_[i].inFlight) != VK_SUCCESS) {
                std::cerr << "VulkanSyncManager: Failed to create inFlight fence for frame " << i << "\n";
                shutdown();
                return false;
            }
        }

        std::cout << "VulkanSyncManager: Initialized " << frameCount << " frame sync objects\n";

        // ===== Image syncs (per swapchain images) =====
        if (swapchainImageCount > 0) {
            imageSyncs_.resize(swapchainImageCount);

            for (uint32_t i = 0; i < swapchainImageCount; ++i) {
                // Crea semaphore per render finished
                if (disp.createSemaphore(&semaphoreInfo, nullptr, &imageSyncs_[i].renderFinished) != VK_SUCCESS) {
                    std::cerr << "VulkanSyncManager: Failed to create renderFinished semaphore for image " << i << "\n";
                    shutdown();
                    return false;
                }

                // ✅ FIX CRITICO: Crea fence SIGNALED anche per le immagini!
                // Altrimenti al primo frame waitForImageFence() blocca per sempre
                if (disp.createFence(&fenceInfo, nullptr, &imageSyncs_[i].inFlight) != VK_SUCCESS) {
                    std::cerr << "VulkanSyncManager: Failed to create inFlight fence for image " << i << "\n";
                    shutdown();
                    return false;
                }

                // imageAvailable non serve per le immagini (resta VK_NULL_HANDLE)
            }

            std::cout << "VulkanSyncManager: Initialized " << swapchainImageCount << " image sync objects\n";
        }

        return true;
    }

    void VulkanSyncManager::shutdown() {
        if (!device_->isValid()) {
            return;
        }

        const auto& disp = device_->getDispatch();

        // Cleanup frame syncs
        for (auto& sync : frameSyncs_) {
            if (sync.imageAvailable != VK_NULL_HANDLE) {
                disp.destroySemaphore(sync.imageAvailable, nullptr);
                sync.imageAvailable = VK_NULL_HANDLE;
            }
            if (sync.renderFinished != VK_NULL_HANDLE) {
                disp.destroySemaphore(sync.renderFinished, nullptr);
                sync.renderFinished = VK_NULL_HANDLE;
            }
            if (sync.inFlight != VK_NULL_HANDLE) {
                disp.destroyFence(sync.inFlight, nullptr);
                sync.inFlight = VK_NULL_HANDLE;
            }
        }
        frameSyncs_.clear();

        // Cleanup image syncs
        for (auto& sync : imageSyncs_) {
            if (sync.imageAvailable != VK_NULL_HANDLE) {
                disp.destroySemaphore(sync.imageAvailable, nullptr);
                sync.imageAvailable = VK_NULL_HANDLE;
            }
            if (sync.renderFinished != VK_NULL_HANDLE) {
                disp.destroySemaphore(sync.renderFinished, nullptr);
                sync.renderFinished = VK_NULL_HANDLE;
            }
            if (sync.inFlight != VK_NULL_HANDLE) {
                disp.destroyFence(sync.inFlight, nullptr);
                sync.inFlight = VK_NULL_HANDLE;
            }
        }
        imageSyncs_.clear();

        std::cout << "VulkanSyncManager: Shutdown complete\n";
    }

    const VulkanSyncManager::FrameSync& VulkanSyncManager::getFrameSync(uint32_t frameIndex) const {
        if (frameIndex >= frameSyncs_.size()) {
            std::cerr << "VulkanSyncManager: Invalid frame index " << frameIndex << "\n";
            static FrameSync empty;
            return empty;
        }
        return frameSyncs_[frameIndex];
    }

    const VulkanSyncManager::FrameSync& VulkanSyncManager::getImageSync(uint32_t imageIndex) const {
        if (imageIndex >= imageSyncs_.size()) {
            std::cerr << "VulkanSyncManager: Invalid image index " << imageIndex << "\n";
            static FrameSync empty;
            return empty;
        }
        return imageSyncs_[imageIndex];
    }

    void VulkanSyncManager::waitForFence(uint32_t frameIndex, uint64_t timeout) {
        if (frameIndex >= frameSyncs_.size()) {
            std::cerr << "VulkanSyncManager: Invalid frame index " << frameIndex << "\n";
            return;
        }

        VkFence fence = frameSyncs_[frameIndex].inFlight;
        if (fence != VK_NULL_HANDLE) {
            vkWaitForFences(device_->getDevice(), 1, &fence, VK_TRUE, timeout);
        }
    }

    void VulkanSyncManager::resetFence(uint32_t frameIndex) {
        if (frameIndex >= frameSyncs_.size()) {
            std::cerr << "VulkanSyncManager: Invalid frame index " << frameIndex << "\n";
            return;
        }

        VkFence fence = frameSyncs_[frameIndex].inFlight;
        if (fence != VK_NULL_HANDLE) {
            vkResetFences(device_->getDevice(), 1, &fence);
        }
    }

    void VulkanSyncManager::waitForImageFence(uint32_t imageIndex, uint64_t timeout) {
        if (imageIndex >= imageSyncs_.size()) {
            return; // Silently ignore se non ci sono image syncs
        }

        VkFence fence = imageSyncs_[imageIndex].inFlight;
        if (fence != VK_NULL_HANDLE) {
            vkWaitForFences(device_->getDevice(), 1, &fence, VK_TRUE, timeout);
        }
    }

    void VulkanSyncManager::waitForAllFences(uint64_t timeout) {
        std::vector<VkFence> fences;
        fences.reserve(frameSyncs_.size() + imageSyncs_.size());

        for (const auto& sync : frameSyncs_) {
            if (sync.inFlight != VK_NULL_HANDLE) {
                fences.push_back(sync.inFlight);
            }
        }

        for (const auto& sync : imageSyncs_) {
            if (sync.inFlight != VK_NULL_HANDLE) {
                fences.push_back(sync.inFlight);
            }
        }

        if (!fences.empty()) {
            vkWaitForFences(device_->getDevice(),
                static_cast<uint32_t>(fences.size()),
                fences.data(),
                VK_TRUE,
                timeout);
        }
    }

    void VulkanSyncManager::resetAllFences() {
        std::vector<VkFence> fences;
        fences.reserve(frameSyncs_.size());

        for (const auto& sync : frameSyncs_) {
            if (sync.inFlight != VK_NULL_HANDLE) {
                fences.push_back(sync.inFlight);
            }
        }

        if (!fences.empty()) {
            vkResetFences(device_->getDevice(),
                static_cast<uint32_t>(fences.size()),
                fences.data());
        }
    }

} // namespace OnYuu