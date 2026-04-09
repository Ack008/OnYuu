#include "ImGuiTextureWrapper.h"
#include "Render/Renderer.h"
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLImGuiTextureWrapper.h"
#include "Platform/Vulkan/VulkanImGuiTextureWrapper.h"

namespace OnYuu {

std::shared_ptr<ImGuiTextureWrapper> ImGuiTextureWrapper::create(const std::shared_ptr<Texture>& texture)
{
	switch (Render::getAPI())
	{
	case API::OpenGL:
		return std::make_shared<OpenGLImGuiTextureWrapper>(texture);
	case API::Vulkan:
		return std::make_shared<VulkanImGuiTextureWrapper>(texture);
	default:
		return nullptr;
	}
}

std::shared_ptr<ImGuiTextureWrapper> ImGuiTextureWrapper::create(const std::shared_ptr<RenderTarget>& renderTarget)
{
	switch (Render::getAPI())
	{
	case API::OpenGL:
		return std::make_shared<OpenGLImGuiTextureWrapper>(renderTarget);
	case API::Vulkan:
		return std::make_shared<VulkanImGuiTextureWrapper>(renderTarget);
	default:
		return nullptr;
	}
}

} // namespace OnYuu
