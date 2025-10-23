#pragma once
#include <stdint.h>
#include "Renderer.h"
#include "Scene.h"
class Window {
public:
	Window(uint32_t width, uint32_t height);
	virtual void draw() = 0;
	virtual double getFrameTime() = 0;
	virtual bool shouldClose() = 0;
	uint32_t getWidth() const;
	uint32_t getHeight() const;
	void resize(uint32_t width, uint32_t height);
private:
	uint32_t  _witdh, _height;
};