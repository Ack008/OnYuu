#include "Texture.h"
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Render/Renderer.h"
namespace OnYuu {
std::shared_ptr<Texture> Texture::createTexture(const std::string& path, TextureFormat format, TextureWrap wrap, TextureType type)
{
	switch (Render::getAPI())
	{
		case API::OpenGL:
			return std::make_shared<OpenGLTexture>(path, format, wrap, type);

		case API::Vulkan:
			return std::make_shared<VulkanTexture>(path, format, wrap, type);
	}
}
}