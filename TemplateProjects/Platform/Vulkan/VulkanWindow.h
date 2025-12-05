#pragma once

#include "Core/View/View.h"
#include "vulkan-bts/VkBootstrap.h"
#include <vulkan/vulkan_core.h>
#include "GLFW/glfw3.h"

class VulkanWindow :
    public Window
{
private:
    vkb::Instance instance;
    vkb::PhysicalDevice physicalDevice;
    vkb::Device device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    vkb::Swapchain swapchain;
    GLFWwindow* window = nullptr;

public:
    VulkanWindow(uint32_t width, uint32_t height);
    ~VulkanWindow();
    VulkanWindow(const VulkanWindow&) = delete;
    VulkanWindow& operator=(const VulkanWindow&) = delete;
    VulkanWindow(VulkanWindow&&) = delete;
    VulkanWindow& operator=(VulkanWindow&&) = delete;

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
