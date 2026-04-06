#include "RenderTarget.h"
#include "Render/Renderer.h"
#include "Platform/Vulkan/VulkanRenderTarget.h"
namespace OnYuu {
	std::shared_ptr<RenderTarget> RenderTarget::create(uint32_t width, uint32_t height)
	{
		switch (Render::getAPI()) {
			case API::Vulkan:
				return std::make_shared<VulkanRenderTarget>(width, height);
		}
		throw std::runtime_error("Unsupported Render API");
	}
}