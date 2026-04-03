#include "VulkanRenderTarget.h"
#include "Render/Renderer.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <stdexcept>

namespace OnYuu {
	VulkanRenderTarget::VulkanRenderTarget(uint32_t width, uint32_t height)
		:RenderTarget(width, height), width_(width), height_(height)
	{
		auto* renderer = static_cast<VulkanRender*>(Render::getInstance().get());
		if (!renderer) {
			throw std::runtime_error("VulkanRenderTarget: renderer not initialized");
		}

		allocator_ = renderer->getAllocator();
		device_ = renderer->getInit().device;
		colorFormat_ = renderer->getInit().swapchain.image_format;
		depthFormat_ = renderer->getDepthFormat();

		VkImageCreateInfo colorImageInfo{};
		colorImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colorImageInfo.imageType = VK_IMAGE_TYPE_2D;
		colorImageInfo.extent.width = width_;
		colorImageInfo.extent.height = height_;
		colorImageInfo.extent.depth = 1;
		colorImageInfo.mipLevels = 1;
		colorImageInfo.arrayLayers = 1;
		colorImageInfo.format = colorFormat_;
		colorImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		colorImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		colorImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		colorImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo colorAllocInfo{};
		colorAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(allocator_, &colorImageInfo, &colorAllocInfo, &colorImage_, &colorAllocation_, nullptr) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderTarget: failed to create color image");
		}

		VkImageViewCreateInfo colorViewInfo{};
		colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorViewInfo.image = colorImage_;
		colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorViewInfo.format = colorFormat_;
		colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorViewInfo.subresourceRange.baseMipLevel = 0;
		colorViewInfo.subresourceRange.levelCount = 1;
		colorViewInfo.subresourceRange.baseArrayLayer = 0;
		colorViewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device_, &colorViewInfo, nullptr, &colorImageView_) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderTarget: failed to create color image view");
		}

		VkImageCreateInfo depthImageInfo{};
		depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
		depthImageInfo.extent.width = width_;
		depthImageInfo.extent.height = height_;
		depthImageInfo.extent.depth = 1;
		depthImageInfo.mipLevels = 1;
		depthImageInfo.arrayLayers = 1;
		depthImageInfo.format = depthFormat_;
		depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo depthAllocInfo{};
		depthAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(allocator_, &depthImageInfo, &depthAllocInfo, &depthImage_, &depthAllocation_, nullptr) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderTarget: failed to create depth image");
		}

		VkImageViewCreateInfo depthViewInfo{};
		depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthViewInfo.image = depthImage_;
		depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthViewInfo.format = depthFormat_;
		depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depthViewInfo.subresourceRange.baseMipLevel = 0;
		depthViewInfo.subresourceRange.levelCount = 1;
		depthViewInfo.subresourceRange.baseArrayLayer = 0;
		depthViewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device_, &depthViewInfo, nullptr, &depthImageView_) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderTarget: failed to create depth image view");
		}
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		if (device_ != VK_NULL_HANDLE) {
			if (colorImageView_ != VK_NULL_HANDLE) {
				vkDestroyImageView(device_, colorImageView_, nullptr);
			}
			if (depthImageView_ != VK_NULL_HANDLE) {
				vkDestroyImageView(device_, depthImageView_, nullptr);
			}
		}
		if (allocator_ != VK_NULL_HANDLE) {
			if (colorImage_ != VK_NULL_HANDLE) {
				vmaDestroyImage(allocator_, colorImage_, colorAllocation_);
			}
			if (depthImage_ != VK_NULL_HANDLE) {
				vmaDestroyImage(allocator_, depthImage_, depthAllocation_);
			}
		}
	}

	void* VulkanRenderTarget::getColorAttachment() const
	{
		return reinterpret_cast<void*>(colorImage_);
	}

	void* VulkanRenderTarget::getDepthAttachment() const
	{
		return reinterpret_cast<void*>(depthImage_);
	}
}
