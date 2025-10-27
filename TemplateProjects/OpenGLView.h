#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "View.h"

class OpenGLWindow :
    public Window
{
private:
    GLFWwindow* window;
public:
    OpenGLWindow(uint32_t width, uint32_t height);
    ~OpenGLWindow();
    virtual void draw() override;
    virtual double getFrameTime() override;
    virtual bool shouldClose() override;
    virtual void beginFrame() override;
	virtual void* getNativeWindow() const override { return window; }
private:
	double lastTime = 0.0;
};

