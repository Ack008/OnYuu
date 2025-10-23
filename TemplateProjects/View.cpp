#include "View.h"
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

