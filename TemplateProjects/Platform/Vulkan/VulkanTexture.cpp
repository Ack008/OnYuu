#include "VulkanTexture.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <stb/stbi_image.h>
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

void VulkanTexture::createTextureImage(const std::string& path)
{
	// Implementazione della creazione dell'immagine Vulkan dalla texture caricata
	

}

void VulkanTexture::createTextureImageView()
{
}

void VulkanTexture::createTextureSampler()
{
}

void VulkanTexture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
}
