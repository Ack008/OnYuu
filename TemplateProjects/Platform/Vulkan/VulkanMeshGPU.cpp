#include "VulkanMeshGPU.h"
#include "Platform/Vulkan/VulkanRender.h"

VulkanMeshGPU::VulkanMeshGPU(Mesh mesh)
{
	this->mesh = mesh;
}

void VulkanMeshGPU::shutdown()
{
	if (uploaded)
	{
		VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		vmaDestroyBuffer(allocator, vertexBuffer, vertexBufferAllocation);
		vmaDestroyBuffer(allocator, indexBuffer, indexBufferAllocation);
		vmaDestroyBuffer(allocator, stagingVertexBuffer, stagingVertexBufferAllocation);
		vmaDestroyBuffer(allocator, stagingIndexBuffer, stagingIndexBufferAllocation);
	}
}

void VulkanMeshGPU::uploadToGPU()
{
	if (uploaded) return;
	createVertexBuffer();
	createIndexBuffer();
	uploaded = true;
}

void VulkanMeshGPU::draw(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	if(indexCount > 0)
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	else 
		vkCmdDraw(commandBuffer, static_cast<uint32_t>(mesh.position.size()), 1, 0, 0);

}

void VulkanMeshGPU::destroyStagingBuffers()

{
	if (stagingVertexBuffer != VK_NULL_HANDLE || stagingIndexBuffer != VK_NULL_HANDLE)
	{
		VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		if (stagingVertexBuffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(allocator, stagingVertexBuffer, stagingVertexBufferAllocation);
			stagingVertexBuffer = VK_NULL_HANDLE;
		}
		if (stagingIndexBuffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(allocator, stagingIndexBuffer, stagingIndexBufferAllocation);
			stagingIndexBuffer = VK_NULL_HANDLE;
		}
	}
}
void VulkanMeshGPU::destroyVertexBuffer()

{
	if (vertexBuffer != VK_NULL_HANDLE)
	{
		VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		vmaDestroyBuffer(allocator, vertexBuffer, vertexBufferAllocation);
		vertexBuffer = VK_NULL_HANDLE;
	}
}

void VulkanMeshGPU::destroyIndexBuffer()
{
	if (indexBuffer != VK_NULL_HANDLE)
	{
		VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		vmaDestroyBuffer(allocator, indexBuffer, indexBufferAllocation);
		indexBuffer = VK_NULL_HANDLE;
	}
}

void VulkanMeshGPU::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(glm::vec3) * mesh.position.size() +
		sizeof(glm::vec4) * mesh.color.size() +
		sizeof(glm::vec2) * mesh.texCoord.size() +
		sizeof(glm::vec3) * mesh.normal.size();
	// Create staging buffer
	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT ;
	VmaAllocationCreateInfo stagingAllocInfo{};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;
	VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
	vmaCreateBuffer(allocator, &stagingBufferInfo, &stagingAllocInfo,
		&stagingVertexBuffer, &stagingVertexBufferAllocation, &stagingVertexAllocInfo);
	// Copy vertex data to staging buffer
	size_t offset = 0;
	std::vector<float> vertexData;
	for (int i = 0; i < mesh.position.size(); i++)
	{
		vertexData.push_back(mesh.position[i].x);
		vertexData.push_back(mesh.position[i].y);
		vertexData.push_back(mesh.position[i].z);
		vertexData.push_back(mesh.color[i].r);
		vertexData.push_back(mesh.color[i].g);
		vertexData.push_back(mesh.color[i].b);
		vertexData.push_back(mesh.color[i].a);
		vertexData.push_back(mesh.texCoord[i].x);
		vertexData.push_back(mesh.texCoord[i].y);
		vertexData.push_back(mesh.normal[i].x);
		vertexData.push_back(mesh.normal[i].y);
		vertexData.push_back(mesh.normal[i].z);

	}
	memcpy(stagingVertexAllocInfo.pMappedData,
		vertexData.data(), bufferSize);
	// Create device local vertex buffer
	VkBufferCreateInfo vertexBufferInfo{};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.size = bufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VmaAllocationCreateInfo vertexAllocInfo{};
	vertexAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	VmaAllocationInfo vertexAllocInfoInfo{};
	vmaCreateBuffer(allocator, &vertexBufferInfo, &vertexAllocInfo,
		&vertexBuffer, &vertexBufferAllocation, &vertexAllocInfoInfo);

	// Copy data from staging buffer to device local buffer
	copyBuffer(stagingVertexBuffer, vertexBuffer, bufferSize);
}

void VulkanMeshGPU::createIndexBuffer()
{
	indexCount = static_cast<uint32_t>(mesh.indices.size());
	VkDeviceSize bufferSize = sizeof(uint32_t) * indexCount;
	// Create staging buffer
	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VmaAllocationCreateInfo stagingAllocInfo{};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;
	VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
	vmaCreateBuffer(allocator, &stagingBufferInfo, &stagingAllocInfo,
		&stagingIndexBuffer, &stagingIndexBufferAllocation, &stagingIndexAllocInfo);
	// Copy index data to staging buffer
	memcpy(stagingIndexAllocInfo.pMappedData,
		mesh.indices.data(), bufferSize);
	// Create device local index buffer
	VkBufferCreateInfo indexBufferInfo{};
	indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	indexBufferInfo.size = bufferSize;
	indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VmaAllocationCreateInfo indexAllocInfo{};
	indexAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	VmaAllocationInfo indexAllocInfoInfo{};
	vmaCreateBuffer(allocator, &indexBufferInfo, &indexAllocInfo,
		&indexBuffer, &indexBufferAllocation, &indexAllocInfoInfo);
	// Copy data from staging buffer to device local buffer
	copyBuffer(stagingIndexBuffer, indexBuffer, bufferSize);
}

void VulkanMeshGPU::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VulkanRender* render = (VulkanRender*)(Render::getInstance().get());
	VkCommandBuffer commandBuffer = render->beginSingleTimeCommands();
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	render->endSingleTimeCommands(commandBuffer);
}
