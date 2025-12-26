#include "VulkanTexture.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <stb/stbi_image.h>
namespace OnYuu {

	VulkanTexture::VulkanTexture(const std::string& path, TextureFormat format, TextureWrap wrap, TextureType type)
	{
		allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		createTextureImage(path);
		createTextureImageView();
		createTextureSampler();
	}

	VulkanTexture::~VulkanTexture()
	{
	}

	void VulkanTexture::bind(uint32_t slot)
	{
	}

	void VulkanTexture::shutdown()
	{
		VulkanRender* renderer = (VulkanRender*)(Render::getInstance().get());
		vkDestroySampler(renderer->getInit().device, textureSampler, nullptr);
		vkDestroyImageView(renderer->getInit().device, textureImageView, nullptr);
		vmaDestroyImage(renderer->getAllocator(), textureImage, textureImageAllocation);
	}

	void VulkanTexture::createTextureImage(const std::string& path)
	{
		// Implementazione della creazione dell'immagine Vulkan dalla texture caricata
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels) {
			throw std::runtime_error("Failed to load image!");
		}

		VkDeviceSize imageSize = texWidth * texHeight * 4;
		VkBuffer stagingBuffer;
		VmaAllocation stagingAlloc;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = imageSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
			&stagingBuffer, &stagingAlloc, nullptr);
		void* data;
		vmaMapMemory(allocator, stagingAlloc, &data);
		memcpy(data, pixels, imageSize);
		vmaUnmapMemory(allocator, stagingAlloc);

		stbi_image_free(pixels);
		// Creazione dell'immagine Vulkan e copia dei dati dallo staging buffer

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = texWidth;
		imageInfo.extent.height = texHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage =
			VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo imgAllocInfo{};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;


		((VulkanRender*)(Render::getInstance().get()))->endSingleTimeCommands(((VulkanRender*)(Render::getInstance().get()))->beginSingleTimeCommands());
		vmaCreateImage(allocator, &imageInfo, &imgAllocInfo,
			&textureImage, &this->textureImageAllocation, nullptr);

		copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		vmaDestroyBuffer(allocator, stagingBuffer, stagingAlloc);

	}

	void VulkanTexture::createTextureImageView()
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = textureImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		VulkanRender* renderer = (VulkanRender*)(Render::getInstance().get());
		vkCreateImageView(renderer->getInit().device, &viewInfo, nullptr, &textureImageView);
	}

	void VulkanTexture::createTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VulkanRender* renderer = (VulkanRender*)(Render::getInstance().get());

		vkCreateSampler(renderer->getInit().device, &samplerInfo, nullptr, &textureSampler);
	}

	void VulkanTexture::transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VulkanRender* renderer = (VulkanRender*)(Render::getInstance().get());
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO
		vkCmdPipelineBarrier(
			cmd,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

	void VulkanTexture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VulkanRender* renderer = (VulkanRender*)(Render::getInstance().get());
		VkCommandBuffer cmd = renderer->beginSingleTimeCommands();
		transitionImageLayout(cmd, image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};
		vkCmdCopyBufferToImage(
			cmd,
			buffer,
			textureImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
		transitionImageLayout(cmd, image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		renderer->endSingleTimeCommands(cmd);
	}
}