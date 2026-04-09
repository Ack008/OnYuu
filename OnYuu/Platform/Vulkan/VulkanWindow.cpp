#include "VulkanWindow.h"
#include <iostream>
namespace OnYuu {

    VulkanWindow::VulkanWindow(uint32_t width, uint32_t height)
        :Window(width, height)
    {
        vkb::InstanceBuilder builder;
        auto inst_ret =
            builder.set_app_name("Example Vulkan Application").request_validation_layers().use_default_debug_messenger().build();
        if (!inst_ret) {
            std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
            std::exit(-1);
        }
        vkb::Instance vkb_inst = inst_ret.value();

        // Conserva l'istanza nel membro della classe
        instance = inst_ret.value();

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            std::exit(-1);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(width, height, "Vulkan Triangle", NULL, NULL);

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkResult glfw_result = glfwCreateWindowSurface(vkb_inst, window, nullptr, &surface);
        if (glfw_result != VK_SUCCESS) {
            std::cerr << "Failed to select create window surface. Error: " << std::to_string(glfw_result) << "\n";
            std::exit(-1);
        }

    }

    VulkanWindow::~VulkanWindow()
    {
        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();
    }

    void VulkanWindow::draw()
    {
        glfwPollEvents();

    }

    double VulkanWindow::getFrameTime()
    {
        return deltaTime;
    }

    bool VulkanWindow::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    void VulkanWindow::beginFrame()
    {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
    }
}