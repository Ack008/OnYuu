#include "VulkanCommandManager.h"
#include <iostream>

namespace OnYuu {

    VulkanCommandManager::VulkanCommandManager(VulkanDevice* device)
        : device_(device) {
        if (!device_) {
            throw std::runtime_error("VulkanCommandManager: device is null");
        }
    }

    VulkanCommandManager::~VulkanCommandManager() {
        shutdown();
    }

    bool VulkanCommandManager::initialize(uint32_t queueFamily, uint32_t frameCount) {
        if (!device_->isValid()) {
            std::cerr << "VulkanCommandManager: Device not valid\n";
            return false;
        }

        const auto& disp = device_->getDispatch();

        // Crea main command pool (resettable per frame)
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamily;

        if (disp.createCommandPool(&poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to create command pool\n";
            return false;
        }

        // Crea single-time command pool (transient, per operazioni immediate)
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        if (disp.createCommandPool(&poolInfo, nullptr, &singleTimePool_) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to create single-time command pool\n";
            disp.destroyCommandPool(commandPool_, nullptr);
            commandPool_ = VK_NULL_HANDLE;
            return false;
        }

        // Alloca command buffers per ogni frame
        commandBuffers_.resize(frameCount);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool_;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = frameCount;

        if (disp.allocateCommandBuffers(&allocInfo, commandBuffers_.data()) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to allocate command buffers\n";
            shutdown();
            return false;
        }

        std::cout << "VulkanCommandManager: Initialized with " << frameCount << " command buffers\n";
        return true;
    }

    void VulkanCommandManager::shutdown() {
        if (!device_->isValid()) {
            return;
        }

        const auto& disp = device_->getDispatch();

        // I command buffers vengono liberati automaticamente con il pool
        commandBuffers_.clear();

        if (commandPool_ != VK_NULL_HANDLE) {
            disp.destroyCommandPool(commandPool_, nullptr);
            commandPool_ = VK_NULL_HANDLE;
        }

        if (singleTimePool_ != VK_NULL_HANDLE) {
            disp.destroyCommandPool(singleTimePool_, nullptr);
            singleTimePool_ = VK_NULL_HANDLE;
        }

        std::cout << "VulkanCommandManager: Shutdown complete\n";
    }

    VkCommandBuffer VulkanCommandManager::getCommandBuffer(uint32_t frameIndex) const {
        if (frameIndex >= commandBuffers_.size()) {
            std::cerr << "VulkanCommandManager: Invalid frame index " << frameIndex << "\n";
            return VK_NULL_HANDLE;
        }
        return commandBuffers_[frameIndex];
    }

    VkCommandBuffer VulkanCommandManager::beginSingleTime() {
        const auto& disp = device_->getDispatch();

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = singleTimePool_;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer cmd;
        if (disp.allocateCommandBuffers(&allocInfo, &cmd) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to allocate single-time command buffer\n";
            return VK_NULL_HANDLE;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (disp.beginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to begin single-time command buffer\n";
            disp.freeCommandBuffers(singleTimePool_, 1, &cmd);
            return VK_NULL_HANDLE;
        }

        return cmd;
    }

    void VulkanCommandManager::endSingleTime(VkCommandBuffer cmd, VkQueue queue) {
        const auto& disp = device_->getDispatch();

        if (disp.endCommandBuffer(cmd) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to end single-time command buffer\n";
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        disp.queueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        disp.queueWaitIdle(queue);

        disp.freeCommandBuffers(singleTimePool_, 1, &cmd);
    }

    void VulkanCommandManager::reset(uint32_t frameIndex) {
        VkCommandBuffer cmd = getCommandBuffer(frameIndex);
        if (cmd != VK_NULL_HANDLE) {
            vkResetCommandBuffer(cmd, 0);
        }
    }

    bool VulkanCommandManager::begin(uint32_t frameIndex) {
        return begin(frameIndex, 0);
    }

    bool VulkanCommandManager::begin(uint32_t frameIndex, VkCommandBufferUsageFlags flags) {
        VkCommandBuffer cmd = getCommandBuffer(frameIndex);
        if (cmd == VK_NULL_HANDLE) {
            return false;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flags;

        if (device_->getDispatch().beginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to begin command buffer " << frameIndex << "\n";
            return false;
        }

        return true;
    }

    bool VulkanCommandManager::end(uint32_t frameIndex) {
        VkCommandBuffer cmd = getCommandBuffer(frameIndex);
        if (cmd == VK_NULL_HANDLE) {
            return false;
        }

        if (device_->getDispatch().endCommandBuffer(cmd) != VK_SUCCESS) {
            std::cerr << "VulkanCommandManager: Failed to end command buffer " << frameIndex << "\n";
            return false;
        }

        return true;
    }

} // namespace OnYuu