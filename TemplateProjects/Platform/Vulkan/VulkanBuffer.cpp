#include "VulkanBuffer.h"

VulkanUniformBuffer::VulkanUniformBuffer(uint32_t bindingPoint, size_t size)
{
	this->bindingPoint = bindingPoint;
	// Inizializza il buffer Vulkan qui (crea VkBuffer, VmaAllocation, ecc.)
	VkDeviceSize bufferSize = size;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;
	// Questo fa sì che il buffer sia già mappato e scrivibile da CPU
	allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
		&vulkanBuffer, &vulkanBufferAllocation, &uniformAllocInfo);
	this->bufferSize = size;
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
}

void VulkanUniformBuffer::bind()
{
}

void VulkanUniformBuffer::unbind()
{
}

void VulkanUniformBuffer::bindToBindingPoint(uint32_t bindingPoint)
{
}

void VulkanUniformBuffer::setData(const void* data, size_t size, BufferUsage usage)
{
	memcpy(uniformAllocInfo.pMappedData, data, size);
	usedSize = size;
}

void VulkanUniformBuffer::updateData(const void* data, size_t size, size_t offset)
{
	memcpy(static_cast<uint8_t*>(uniformAllocInfo.pMappedData) + offset, data, size);
}

void VulkanUniformBuffer::resize(size_t newSize)
{
}

void VulkanUniformBuffer::shutdown()
{
	vmaDestroyBuffer(allocator, vulkanBuffer, vulkanBufferAllocation);
}
