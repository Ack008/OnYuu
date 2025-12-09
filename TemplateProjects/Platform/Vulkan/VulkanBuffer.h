#pragma once
#include "Render/Buffer.h"
#include <vulkan/vulkan.h>
#include "Platform/Vulkan/VulkanRender.h"
#include "Platform/Vulkan/VulkanBuffer.h"
// VulkanUniformBuffer: implementazione concreta di UniformBuffer per Vulkan.

class VulkanUniformBuffer : public UniformBuffer
{
	public:
	VulkanUniformBuffer(uint32_t bindingPoint, size_t size);
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
	VkBuffer vulkanBuffer;
	VmaAllocation vulkanBufferAllocation;
	VmaAllocator allocator;
	VmaAllocationInfo uniformAllocInfo;
public:
	VkBuffer getVulkanBuffer() const { return vulkanBuffer; }
	// Aggiungi qui eventuali membri specifici per Vulkan, come VkBuffer, VkDeviceMemory, ecc.
};