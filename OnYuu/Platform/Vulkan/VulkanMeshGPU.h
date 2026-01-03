#pragma once
/*
#include "Core/Model/Components/MeshComponent.h"
#include "Render/Buffer.h"
#include <vulkan/vulkan.h>
#include "vma/vk_mem_alloc.h"
#include <unordered_map>
#include <vector>
#include "Platform/Vulkan/VulkanBuffer.h"
// VulkanMeshGPU: implementazione concreta di MeshGPU per Vulkan. Gestisce il
// caricamento della mesh sulla GPU utilizzando buffer Vulkan.
using namespace OnYuu;
class VulkanMeshGPU
{
	public:
	VulkanMeshGPU(Mesh &mesh);
	virtual void shutdown();
	void uploadToGPU();
	void draw(VkCommandBuffer commandBuffer);
	void drawInstanced(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance = 0);
	VkBuffer getVertexBuffer() const { return vertexBuffer; }
	VkBuffer getIndexBuffer() const { return indexBuffer; }
	uint32_t getIndexCount() const { return indexCount; }
	VkBuffer updateModelBuffer(int image_index, glm::mat4 modelMatrix);
	void destroyStagingBuffers();
	void destroyVertexBuffer();
	void destroyIndexBuffer();
	void destroy()
	{
		shutdown();
	}
private:
	Mesh &mesh;
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	uint32_t indexCount = 0;
	VmaAllocation vertexBufferAllocation = VK_NULL_HANDLE;
	VmaAllocation indexBufferAllocation = VK_NULL_HANDLE;
	//staging buffers
	VkBuffer stagingVertexBuffer = VK_NULL_HANDLE;
	VkBuffer stagingIndexBuffer = VK_NULL_HANDLE;
	VmaAllocation stagingVertexBufferAllocation = VK_NULL_HANDLE;
	VmaAllocation stagingIndexBufferAllocation = VK_NULL_HANDLE;
	VmaAllocationInfo stagingVertexAllocInfo = {};
	VmaAllocationInfo stagingIndexAllocInfo = {};
	std::vector<std::shared_ptr<VulkanUniformBuffer>> modelBuffers;
	bool uploaded = false;
	void createVertexBuffer();
	void createIndexBuffer();
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};*/