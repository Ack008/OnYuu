#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/View/View.h"
namespace OnYuu {
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
        virtual double getTime() override { return glfwGetTime(); }
        virtual bool shouldClose() override;
        virtual void beginFrame() override;
        virtual void* getNativeWindow() const override { return window; }
    private:
        double lastTime = 0.0;
        double deltaTime = 0.0;
    };

} // namespace OnYuu