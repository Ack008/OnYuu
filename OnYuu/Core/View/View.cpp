#include "View.h"
#include <memory>
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLView.h"
#include "Platform/Vulkan/VulkanWindow.h"
#include "Render/Renderer.h"
namespace OnYuu {
Window::Window(uint32_t width, uint32_t height)
	:_witdh(width), _height(height)
{
}
uint32_t Window::getWidth() const
{
	return _witdh;
}


uint32_t Window::getHeight() const
{
	return _height;
}

void Window::resize(uint32_t width, uint32_t height)
{
	if (_witdh != 0 && _height != 0) {
		_witdh = width;
		_height = height;
	}
}

std:: shared_ptr<Window> Window::create(uint32_t width, uint32_t height)
{
	switch (Render::getAPI())
	{
		case API::OpenGL:
		// Return OpenGLWindow instance
		return std::make_shared<OpenGLWindow>(width, height);
			break;
		case API::Vulkan:
			// Return VulkanWindow instance
			return std::make_shared<VulkanWindow>(width, height);
			break;
		default:
			break;
	}
}
} // namespace OnYuu