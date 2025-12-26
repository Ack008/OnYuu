#include "VulkanMeshGPU.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <iostream>
VulkanMeshGPU::VulkanMeshGPU(Mesh &mesh)
	:mesh(mesh)
{
	int max_images = ((VulkanRender*)(Render::getInstance().get()))->getRenderData().framebuffers.size();
	this->mesh = mesh;

	// Creare istanze distinte di VulkanUniformBuffer per ogni image/framebuffer.
	modelBuffers.clear();
	modelBuffers.reserve(max_images);
	for (int i = 0; i < max_images; ++i)
	{
		modelBuffers.emplace_back(std::make_shared<VulkanUniformBuffer>(1, sizeof(glm::mat4)));
	}
}

void VulkanMeshGPU::shutdown()
{
	if (uploaded)
	{
		VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		for (auto& modelBuffer : modelBuffers)
		{
			if (modelBuffer) // controlla il puntatore prima di usarlo
			{
				modelBuffer->shutdown();
				modelBuffer.reset(); // evita doppia distruzione se per qualche motivo il vettore conteneva riferimenti duplicati
			}
		}
		modelBuffers.clear();

		if (vertexBuffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(allocator, vertexBuffer, vertexBufferAllocation);
			vertexBuffer = VK_NULL_HANDLE;
		}
		if (indexBuffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(allocator, indexBuffer, indexBufferAllocation);
			indexBuffer = VK_NULL_HANDLE;
		}
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

		uploaded = false;
	}
}

void VulkanMeshGPU::uploadToGPU()
{
	if (!mesh.empty()) 
	{
		if (uploaded) return;
		createVertexBuffer();
		createIndexBuffer();
		uploaded = true;
		mesh.position.clear();
		mesh.color.clear();
		mesh.texCoord.clear();
		mesh.normal.clear();
		mesh.indices.clear();
	}

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

VkBuffer VulkanMeshGPU::updateModelBuffer(int image_index,glm::mat4 modelMatrix)
{
	modelBuffers[image_index]->setData(&modelMatrix, sizeof(glm::mat4), BufferUsage::DYNAMIC);
	return modelBuffers[image_index]->getVulkanBuffer();
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
	VmaAllocator allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
	if (stagingVertexBuffer == VK_NULL_HANDLE || vertexBuffer == VK_NULL_HANDLE)
	{
		// Create staging buffer
		VkBufferCreateInfo stagingBufferInfo{};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.size = bufferSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT ;
		VmaAllocationCreateInfo stagingAllocInfo{};
		stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;
		vmaCreateBuffer(allocator, &stagingBufferInfo, &stagingAllocInfo,
			&stagingVertexBuffer, &stagingVertexBufferAllocation, &stagingVertexAllocInfo);
		std::cout << "Created staging vertex buffer " << stagingVertexBuffer << " of size " << bufferSize << " bytes\n";
		// Create device local vertex buffer
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
	}
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
	

	// Copy data from staging buffer to device local buffer
	copyBuffer(stagingVertexBuffer, vertexBuffer, bufferSize);
	std::cout << "Created vertex buffer " << vertexBuffer << " of size " << bufferSize << " bytes\n";
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
	std::cout << "Created staging index buffer " << stagingIndexBuffer << " of size " << bufferSize << " bytes\n";
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
	std::cout << "Created index buffer " << indexBuffer << " of size " << bufferSize << " bytes\n";
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
