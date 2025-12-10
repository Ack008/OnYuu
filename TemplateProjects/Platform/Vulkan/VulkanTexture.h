#pragma once
#include "Core/Texture.h"
#include "vulkan-bts/VkBootstrap.h"
#include <vma/vk_mem_alloc.h>
class VulkanTexture : public Texture
{
	public:
	VulkanTexture(const std::string& path, TextureFormat format, TextureWrap wrap, TextureType type);
	~VulkanTexture();
	virtual void bind(uint32_t slot = 0) override;
	VkImageView getImageView() const { return textureImageView; }
	VkSampler getSampler() const { return textureSampler; }
private:
	VkImage textureImage;
	VmaAllocation textureImageAllocation;
	VkImageView textureImageView;
	VkSampler textureSampler;
	VmaAllocator allocator;
	void createTextureImage(const std::string& path);
	void createTextureImageView();
	void createTextureSampler();
	void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
