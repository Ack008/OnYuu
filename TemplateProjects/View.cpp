#include "View.h"
#include <memory>
#include "API.h"
#include "OpenGLView.h"
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
	switch (apiInUse)
	{
		case API::OpenGL:
		// Return OpenGLWindow instance
		return std::make_shared<OpenGLWindow>(width, height);
			break;
		default:
			break;
	}
}