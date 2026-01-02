#include "VulkanSwapchain.h"
#include <iostream>
#include <array>
#include <vulkan/vulkan_core.h>

namespace OnYuu {

    VulkanSwapchain::VulkanSwapchain(VulkanDevice* device)
        : device_(device) {
        if (!device_) {
            throw std::runtime_error("VulkanSwapchain: device is null");
        }
    }

    VulkanSwapchain::~VulkanSwapchain() {
        shutdown();
    }

    bool VulkanSwapchain::create(VkSurfaceKHR surface, VkRenderPass renderPass,
        VkImageView depthView, const Config& config) {
        if (!device_->isValid()) {
            std::cerr << "VulkanSwapchain: Device not valid\n";
            return false;
        }

        // Salva parametri per recreate
        lastSurface_ = surface;
        lastRenderPass_ = renderPass;
        lastDepthView_ = depthView;
        config_ = config;

        // Crea swapchain
        vkb::SwapchainBuilder builder{ device_->getVkbDevice() };

        VkSurfaceFormatKHR preferredFormat;
        preferredFormat.format = config.preferredFormat;
        preferredFormat.colorSpace = config.preferredColorSpace;

        auto swapRet = builder
            .set_old_swapchain(swapchain_)
            .set_desired_format(preferredFormat)
            .set_desired_present_mode(config.presentMode)
            .set_desired_min_image_count(config.minImageCount)
            .add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build();

        if (!swapRet) {
            std::cerr << "VulkanSwapchain: Failed to create swapchain: "
                << swapRet.error().message() << "\n";
            return false;
        }

        // Distruggi vecchia swapchain se esiste
        if (swapchain_.swapchain != VK_NULL_HANDLE) {
            vkb::destroy_swapchain(swapchain_);
        }

        swapchain_ = swapRet.value();

        // Ottieni images e crea views
        auto images = swapchain_.get_images();
        auto views = swapchain_.get_image_views();

        if (!images || !views) {
            std::cerr << "VulkanSwapchain: Failed to get images/views\n";
            return false;
        }

        auto imageVec = images.value();
        auto viewVec = views.value();

        frames_.resize(imageVec.size());
        for (size_t i = 0; i < imageVec.size(); ++i) {
            frames_[i].image = imageVec[i];
            frames_[i].view = viewVec[i];
        }

        std::cout << "VulkanSwapchain: Created with " << frames_.size() << " images\n";
        std::cout << "  - Format: " << swapchain_.image_format << "\n";
        std::cout << "  - Extent: " << swapchain_.extent.width << "x" << swapchain_.extent.height << "\n";

        // Crea framebuffers
        if (!createFramebuffers(renderPass, depthView)) {
            std::cerr << "VulkanSwapchain: Failed to create framebuffers\n";
            return false;
        }

        return true;
    }

    bool VulkanSwapchain::recreate(VkSurfaceKHR surface, VkRenderPass renderPass,
        VkImageView depthView) {
        std::cout << "VulkanSwapchain: Recreating...\n";

        device_->waitIdle();
        cleanup();

        return create(surface, renderPass, depthView, config_);
    }

    bool VulkanSwapchain::createFramebuffers(VkRenderPass renderPass, VkImageView depthView) {
        const auto& disp = device_->getDispatch();

        for (auto& frame : frames_) {
            std::array<VkImageView, 2> attachments = { frame.view, depthView };

            VkFramebufferCreateInfo fbInfo{};
            fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbInfo.renderPass = renderPass;
            fbInfo.attachmentCount = attachments.size();
            fbInfo.pAttachments = attachments.data();
            fbInfo.width = swapchain_.extent.width;
            fbInfo.height = swapchain_.extent.height;
            fbInfo.layers = 1;

            if (disp.createFramebuffer(&fbInfo, nullptr, &frame.framebuffer) != VK_SUCCESS) {
                std::cerr << "VulkanSwapchain: Failed to create framebuffer\n";
                return false;
            }
        }

        return true;
    }

    void VulkanSwapchain::cleanup() {
        const auto& disp = device_->getDispatch();

        // Distruggi framebuffers
        for (auto& frame : frames_) {
            if (frame.framebuffer != VK_NULL_HANDLE) {
                disp.destroyFramebuffer(frame.framebuffer, nullptr);
                frame.framebuffer = VK_NULL_HANDLE;
            }
        }

        // Le image views sono gestite da vkb::Swapchain
        if (swapchain_.swapchain != VK_NULL_HANDLE) {
            std::vector<VkImageView> views;
            views.reserve(frames_.size());
            for (const auto& frame : frames_) {
                views.push_back(frame.view);
            }
            swapchain_.destroy_image_views(views);
        }

        frames_.clear();
    }

    void VulkanSwapchain::shutdown() {
        if (swapchain_.swapchain != VK_NULL_HANDLE) {
            cleanup();
            vkb::destroy_swapchain(swapchain_);
            swapchain_ = {};
            std::cout << "VulkanSwapchain: Shutdown complete\n";
        }
    }

    VkResult VulkanSwapchain::acquireNextImage(VkSemaphore semaphore, uint32_t* imageIndex) {
        return vkAcquireNextImageKHR(
            device_->getDevice(),
            swapchain_.swapchain,
            UINT64_MAX,
            semaphore,
            VK_NULL_HANDLE,
            imageIndex
        );
    }

    VkResult VulkanSwapchain::present(VkQueue queue, uint32_t imageIndex,
        VkSemaphore waitSemaphore) {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &waitSemaphore;

        VkSwapchainKHR swapchains[] = { swapchain_.swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        return vkQueuePresentKHR(queue, &presentInfo);
    }

} // namespace OnYuu