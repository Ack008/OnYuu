#include "VulkanDescriptorManager.h"
#include <iostream>

namespace OnYuu {

    VulkanDescriptorManager::VulkanDescriptorManager(VulkanDevice* device)
        : device_(device) {
        if (!device_) {
            throw std::runtime_error("VulkanDescriptorManager: device is null");
        }
    }

    VulkanDescriptorManager::~VulkanDescriptorManager() {
        shutdown();
    }

    bool VulkanDescriptorManager::initialize(uint32_t maxSets) {
        if (!device_->isValid()) {
            std::cerr << "VulkanDescriptorManager: Device not valid\n";
            return false;
        }

        const auto& disp = device_->getDispatch();

        // Crea pool grande per tutti i tipi di descriptor
        std::vector<VkDescriptorPoolSize> poolSizes = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = maxSets;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        if (disp.createDescriptorPool(&poolInfo, nullptr, &pool_) != VK_SUCCESS) {
            std::cerr << "VulkanDescriptorManager: Failed to create descriptor pool\n";
            return false;
        }

        std::cout << "VulkanDescriptorManager: Initialized with max " << maxSets << " sets\n";
        return true;
    }

    void VulkanDescriptorManager::shutdown() {
        if (!device_->isValid()) {
            return;
        }

        const auto& disp = device_->getDispatch();

        // Distruggi layouts
        for (auto layout : layouts_) {
            if (layout != VK_NULL_HANDLE) {
                disp.destroyDescriptorSetLayout(layout, nullptr);
            }
        }
        layouts_.clear();

        // Distruggi pool (libera automaticamente tutti i sets)
        if (pool_ != VK_NULL_HANDLE) {
            disp.destroyDescriptorPool(pool_, nullptr);
            pool_ = VK_NULL_HANDLE;
        }

        std::cout << "VulkanDescriptorManager: Shutdown complete\n";
    }

    VkDescriptorSetLayout VulkanDescriptorManager::createLayout(
        const std::vector<VkDescriptorSetLayoutBinding>& bindings) {

        if (bindings.empty()) {
            std::cerr << "VulkanDescriptorManager: Empty bindings vector\n";
            return VK_NULL_HANDLE;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VkDescriptorSetLayout layout;
        if (device_->getDispatch().createDescriptorSetLayout(&layoutInfo, nullptr, &layout) != VK_SUCCESS) {
            std::cerr << "VulkanDescriptorManager: Failed to create descriptor set layout\n";
            return VK_NULL_HANDLE;
        }

        layouts_.push_back(layout);
        return layout;
    }

    void VulkanDescriptorManager::destroyLayout(VkDescriptorSetLayout layout) {
        if (layout == VK_NULL_HANDLE) return;

        device_->getDispatch().destroyDescriptorSetLayout(layout, nullptr);

        // Rimuovi dalla lista
        auto it = std::find(layouts_.begin(), layouts_.end(), layout);
        if (it != layouts_.end()) {
            layouts_.erase(it);
        }
    }

    VkDescriptorSet VulkanDescriptorManager::allocateSet(VkDescriptorSetLayout layout) {
        if (layout == VK_NULL_HANDLE) {
            std::cerr << "VulkanDescriptorManager: Invalid layout\n";
            return VK_NULL_HANDLE;
        }

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool_;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet set;
        if (device_->getDispatch().allocateDescriptorSets(&allocInfo, &set) != VK_SUCCESS) {
            std::cerr << "VulkanDescriptorManager: Failed to allocate descriptor set\n";
            return VK_NULL_HANDLE;
        }

        return set;
    }

    std::vector<VkDescriptorSet> VulkanDescriptorManager::allocateSets(
        VkDescriptorSetLayout layout, uint32_t count) {

        if (layout == VK_NULL_HANDLE || count == 0) {
            std::cerr << "VulkanDescriptorManager: Invalid parameters for allocateSets\n";
            return {};
        }

        std::vector<VkDescriptorSetLayout> layouts(count, layout);
        std::vector<VkDescriptorSet> sets(count);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool_;
        allocInfo.descriptorSetCount = count;
        allocInfo.pSetLayouts = layouts.data();

        if (device_->getDispatch().allocateDescriptorSets(&allocInfo, sets.data()) != VK_SUCCESS) {
            std::cerr << "VulkanDescriptorManager: Failed to allocate " << count << " descriptor sets\n";
            return {};
        }

        return sets;
    }

    void VulkanDescriptorManager::freeSet(VkDescriptorSet set) {
        if (set != VK_NULL_HANDLE) {
            device_->getDispatch().freeDescriptorSets(pool_, 1, &set);
        }
    }

    void VulkanDescriptorManager::freeSets(const std::vector<VkDescriptorSet>& sets) {
        if (!sets.empty()) {
            device_->getDispatch().freeDescriptorSets(
                pool_,
                static_cast<uint32_t>(sets.size()),
                sets.data()
            );
        }
    }

    void VulkanDescriptorManager::updateBuffer(VkDescriptorSet set, uint32_t binding,
        VkBuffer buffer, VkDeviceSize size,
        VkDeviceSize offset,
        VkDescriptorType descriptorType) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = offset;
        bufferInfo.range = size;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = set;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = descriptorType;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;

        device_->getDispatch().updateDescriptorSets(1, &write, 0, nullptr);
    }

    void VulkanDescriptorManager::updateImage(VkDescriptorSet set, uint32_t binding,
        VkImageView view, VkSampler sampler,
        VkImageLayout layout) {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = layout;
        imageInfo.imageView = view;
        imageInfo.sampler = sampler;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = set;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        device_->getDispatch().updateDescriptorSets(1, &write, 0, nullptr);
    }

    void VulkanDescriptorManager::updateBuffers(VkDescriptorSet set, uint32_t binding,
        const std::vector<VkDescriptorBufferInfo>& buffers,
        VkDescriptorType descriptorType) {
        if (buffers.empty()) return;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = set;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = descriptorType;
        write.descriptorCount = static_cast<uint32_t>(buffers.size());
        write.pBufferInfo = buffers.data();

        device_->getDispatch().updateDescriptorSets(1, &write, 0, nullptr);
    }

    void VulkanDescriptorManager::updateImages(VkDescriptorSet set, uint32_t binding,
        const std::vector<VkDescriptorImageInfo>& images) {
        if (images.empty()) return;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = set;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = static_cast<uint32_t>(images.size());
        write.pImageInfo = images.data();

        device_->getDispatch().updateDescriptorSets(1, &write, 0, nullptr);
    }

} // namespace OnYuu