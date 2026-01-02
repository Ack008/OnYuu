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

    bool VulkanSyncManager::initialize(uint32_t frameCount) {
        if (!device_->isValid()) {
            std::cerr << "VulkanSyncManager: Device not valid\n";
            return false;
        }

        if (frameCount == 0) {
            std::cerr << "VulkanSyncManager: frameCount must be > 0\n";
            return false;
        }

        const auto& disp = device_->getDispatch();
        frameSyncs_.resize(frameCount);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Inizialmente segnalato

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

        std::cout << "VulkanSyncManager: Initialized with " << frameCount << " frame sync objects\n";
        return true;
    }

    void VulkanSyncManager::shutdown() {
        if (!device_->isValid() || frameSyncs_.empty()) {
            return;
        }

        const auto& disp = device_->getDispatch();

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

    void VulkanSyncManager::waitForAllFences(uint64_t timeout) {
        std::vector<VkFence> fences;
        fences.reserve(frameSyncs_.size());

        for (const auto& sync : frameSyncs_) {
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