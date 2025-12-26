#include "VulkanBuffer.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <iostream>
VulkanUniformBuffer::VulkanUniformBuffer(uint32_t bindingPoint, size_t size ,VmaAllocator allocator_)
{
	this->bindingPoint = bindingPoint;
	if (allocator_)
		allocator = allocator_;
	else
		allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
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
	
	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
		&vulkanBuffer, &vulkanBufferAllocation, &uniformAllocInfo);
	this->bufferSize = size;
	std::cout << "Created VulkanUniformBuffer " << vulkanBuffer << " of size " << size << " bytes\n";
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
	size_t newHash = calculateHash(data, size);

	if (lastDataHash == newHash) {
		return; // Dati identici, skip update
	}

	// Aggiorna buffer
	memcpy(uniformAllocInfo.pMappedData, data, size);
	usedSize = size;
	lastDataHash = newHash;
}

void VulkanUniformBuffer::updateData(const void* data, size_t size, size_t offset)
{
	size_t newHash = calculateHash(data, size);

	memcpy(static_cast<uint8_t*>(uniformAllocInfo.pMappedData) + offset, data, size);
}

void VulkanUniformBuffer::resize(size_t newSize)
{
}

void VulkanUniformBuffer::shutdown()
{
	vmaDestroyBuffer(allocator, vulkanBuffer, vulkanBufferAllocation);
}



VulkanStorageBuffer::VulkanStorageBuffer(uint32_t bindingPoint, size_t size, VmaAllocator allocator_)
{
	this->bindingPoint = bindingPoint;
	// Inizializza il buffer Vulkan qui (crea VkBuffer, VmaAllocation, ecc.)
	if (allocator_)
		allocator = allocator_;
	else
		allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
	VkDeviceSize bufferSize = size;
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;
	// Questo fa sì che il buffer sia già mappato e scrivibile da CPU

	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
		&vulkanBuffer, &vulkanBufferAllocation, &storageAllocInfo);
	this->bufferSize = size;
	std::cout << "Created VulkanStorageBuffer " << vulkanBuffer << " of size " << size << " bytes\n";

	
}
VulkanStorageBuffer::~VulkanStorageBuffer()
{
}
void VulkanStorageBuffer::bind()
{
}
void VulkanStorageBuffer::unbind()
{
}
void VulkanStorageBuffer::bindToBindingPoint(uint32_t bindingPoint)
{
}
void VulkanStorageBuffer::setData(const void* data, size_t size, BufferUsage usage)
{
	if (size > bufferSize) {
		resize(size * 5);
	}
	size_t newHash = calculateHash(data, size);
	if (lastDataHash == newHash) {
		return; // Dati identici, skip update
	}
	// Aggiorna buffer
	memcpy(storageAllocInfo.pMappedData, data, size);
	usedSize = size;
	lastDataHash = newHash;
	// print debug
	/*
	const glm::mat4* matData = static_cast<const glm::mat4*>(data);
	const int quantity = size / sizeof(glm::mat4);
	for(int i = 0; i < quantity; i++) {
		std::cout << "VulkanStorageBuffer Data Matrix:\n";
		for (int j = 0; j < 4; ++j) {
			std::cout << matData->operator[](j).x << " " << matData->operator[](j).y << " "
				<< matData->operator[](j).z << " " << matData->operator[](j).w << "\n";
		}
		matData++;
		size -= sizeof(glm::mat4);
	}*/
}
void VulkanStorageBuffer::updateData(const void* data, size_t size, size_t offset)
{
	size_t newHash = calculateHash(data, size);
	memcpy(static_cast<uint8_t*>(storageAllocInfo.pMappedData) + offset, data, size);
}
void VulkanStorageBuffer::resize(size_t newSize)
{
	if (newSize == bufferSize) return; // Early exit

	// Salva vecchie risorse
	VmaAllocation oldAllocation = vulkanBufferAllocation;
	VkBuffer oldBuffer = vulkanBuffer;
	VmaAllocationInfo oldAllocInfo = storageAllocInfo;
	VkDeviceSize oldSize = bufferSize;

	// Crea nuovo buffer
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = newSize;
	bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;

	if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
		&vulkanBuffer, &vulkanBufferAllocation, &storageAllocInfo) != VK_SUCCESS) {
		// Gestione errore: ripristina stato
		vulkanBuffer = oldBuffer;
		vulkanBufferAllocation = oldAllocation;
		storageAllocInfo = oldAllocInfo;
		throw std::runtime_error("Failed to resize VulkanStorageBuffer");
	}

	this->bufferSize = newSize;

	// Copia dati vecchi
	size_t copySize = std::min(oldSize, newSize);
	if (copySize > 0) {
		memcpy(storageAllocInfo.pMappedData, oldAllocInfo.pMappedData, copySize);
	}

	// IMPORTANTE: Assicurati che la GPU abbia finito di usare oldBuffer
	// Potrebbe servire vkDeviceWaitIdle() o un fence/semaphore
	vmaDestroyBuffer(allocator, oldBuffer, oldAllocation);

	// Reset hash per forzare il prossimo update
	lastDataHash = 0;
}

void VulkanStorageBuffer::shutdown()
{
	vmaDestroyBuffer(allocator, vulkanBuffer, vulkanBufferAllocation);
}
