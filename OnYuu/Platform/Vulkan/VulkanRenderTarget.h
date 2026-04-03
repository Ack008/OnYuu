#pragma once
#include "Render/RenderTarget.h"
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace OnYuu {
	class VulkanRenderTarget : public RenderTarget {
	public:
		VulkanRenderTarget(uint32_t width, uint32_t height);
		~VulkanRenderTarget();
		void* getColorAttachment() const override;
		void* getDepthAttachment() const override;
		VkImage getColorImage() const { return colorImage_; }
		VkImageView getColorImageView() const { return colorImageView_; }
		VkImage getDepthImage() const { return depthImage_; }
		VkImageView getDepthImageView() const { return depthImageView_; }
		VkFormat getDepthFormat() const { return depthFormat_; }
		VkFormat getColorFormat() const { return colorFormat_; }
		VkExtent2D getExtent() const { return { width_, height_ }; }
	private:
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		VkImage colorImage_ = VK_NULL_HANDLE;
		VkImageView colorImageView_ = VK_NULL_HANDLE;
		VmaAllocation colorAllocation_ = VK_NULL_HANDLE;
		VkImage depthImage_ = VK_NULL_HANDLE;
		VkImageView depthImageView_ = VK_NULL_HANDLE;
		VmaAllocation depthAllocation_ = VK_NULL_HANDLE;
		VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
		VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;
		VmaAllocator allocator_ = VK_NULL_HANDLE;
		VkDevice device_ = VK_NULL_HANDLE;
	};
}
