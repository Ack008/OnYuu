#pragma once
#include "Core/Texture.h"
#include "vulkan-bts/VkBootstrap.h"
#include <vma/vk_mem_alloc.h>
namespace OnYuu {
	class VulkanRender;
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& path, TextureFormat format, TextureWrap wrap, TextureType type);
		~VulkanTexture();
		virtual void bind(uint32_t slot = 0) override;
		VkImageView getImageView() const { return textureImageView; }
		VkImage getImage() const { return textureImage; }
		VkSampler getSampler() const { return textureSampler; }
		virtual void shutdown() override;
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

		// Seleziona il miglior formato RGBA supportato dalla GPU fisica.
		// Preferenza: R8G8B8A8_SRGB → R8G8B8A8_UNORM → BGRA_SRGB (con swizzle) → BGRA_UNORM
		static VkFormat pickTextureFormat(VulkanRender* renderer);

		// Formato scelto a runtime — usato sia dall'immagine che dalla view
		VkFormat textureFormat_ = VK_FORMAT_R8G8B8A8_SRGB;
	};
}