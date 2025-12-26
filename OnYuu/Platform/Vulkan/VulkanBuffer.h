#pragma once
#include "Render/Buffer.h"
#include <vulkan/vulkan.h>
#include <xxhash.h>
#include "vma/vk_mem_alloc.h"
namespace OnYuu { 
// VulkanUniformBuffer: implementazione concreta di UniformBuffer per Vulkan.
class VulkanUniformBuffer : public UniformBuffer
{
public:
    VulkanUniformBuffer(uint32_t bindingPoint, size_t size, VmaAllocator allocator_ = nullptr);
    virtual ~VulkanUniformBuffer() override;
    virtual void bind() override;
    virtual void unbind() override;
    virtual void bindToBindingPoint(uint32_t bindingPoint = 0) override;
    virtual void setData(const void* data, size_t size, BufferUsage usage) override;
    virtual void updateData(const void* data, size_t size, size_t offset) override;
    virtual void resize(size_t newSize) override;
    virtual void shutdown() override;
private:
    uint32_t bindingPoint;
    VkBuffer vulkanBuffer = VK_NULL_HANDLE;
    VmaAllocation vulkanBufferAllocation = VK_NULL_HANDLE;
    VmaAllocator allocator = nullptr;
    VmaAllocationInfo uniformAllocInfo = {};
    size_t lastDataHash = 0;

    // Helper function per calcolare hash veloce
    size_t calculateHash(const void* data, size_t size) const {
        // FNV-1a hash (veloce e buona distribuzione)
        size_t hash = 14695981039346656037ULL;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);

        for (size_t i = 0; i < size; ++i) {
            hash ^= bytes[i];
            hash *= 1099511628211ULL;
        }

        return hash;
    }
public:
    VkBuffer getVulkanBuffer() const { return vulkanBuffer; }
    // eventuali altri membri o helper specifici per Vulkan
};



class VulkanStorageBuffer : public UniformBuffer
{
    public:
    VulkanStorageBuffer(uint32_t bindingPoint, size_t size, VmaAllocator allocator_ = nullptr);
    virtual ~VulkanStorageBuffer() override;
    virtual void bind() override;
    virtual void unbind() override;
    virtual void bindToBindingPoint(uint32_t bindingPoint = 0) override;
    virtual void setData(const void* data, size_t size, BufferUsage usage) override;
    virtual void updateData(const void* data, size_t size, size_t offset) override;
    virtual void resize(size_t newSize) override;
	virtual void shutdown() override;
    private:
    uint32_t bindingPoint;
    VkBuffer vulkanBuffer = VK_NULL_HANDLE;
    VmaAllocation vulkanBufferAllocation = VK_NULL_HANDLE;
    VmaAllocator allocator = nullptr;
    VmaAllocationInfo storageAllocInfo = {};
    size_t lastDataHash = 0;
    // Helper function per calcolare hash veloce
    size_t calculateHash(const void* data, size_t size) const {
        // FNV-1a hash (veloce e buona distribuzione)
        size_t hash = 14695981039346656037ULL;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < size; ++i) {
            hash ^= bytes[i];
            hash *= 1099511628211ULL;
        }
        return hash;
	}
    std::vector<size_t> chunkHashes; // Hash per blocchi da 4KB
    static constexpr size_t CHUNK_SIZE = 4096;
    public:
    VkBuffer getVulkanBuffer() const { return vulkanBuffer; }
    size_t quickHash(const void* data, size_t len) {
        return XXH64(data, len, 0); // ~10x più veloce
    }
	// eventuali altri membri o helper specifici per Vulkan
};
} // namespace OnYuu