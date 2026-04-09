#pragma once
#include "Render/RenderTarget.h"
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <vector>

namespace OnYuu {
	class VulkanRenderTarget : public RenderTarget {
	public:
		VulkanRenderTarget(uint32_t width, uint32_t height);
		~VulkanRenderTarget();
		void* getColorAttachment() const override;
		void* getDepthAttachment() const override;
		VkImage getColorImage() const { return getColorImage(0); }
		VkImageView getColorImageView() const { return getColorImageView(0); }
		VkImage getDepthImage() const { return getDepthImage(0); }
		VkImageView getDepthImageView() const { return getDepthImageView(0); }
		VkImage getColorImage(uint32_t frameIndex) const;
		VkImageView getColorImageView(uint32_t frameIndex) const;
		VkImage getDepthImage(uint32_t frameIndex) const;
		VkImageView getDepthImageView(uint32_t frameIndex) const;
		VkSampler getColorSampler() const { return colorSampler_; }
		VkFormat getDepthFormat() const { return depthFormat_; }
		VkFormat getColorFormat() const { return colorFormat_; }
		VkExtent2D getExtent() const { return { width_, height_ }; }
		uint32_t getFrameCount() const { return frameCount_; }
		VkImageLayout getColorLayout(uint32_t frameIndex) const;
		void setColorLayout(uint32_t frameIndex, VkImageLayout layout);
	private:
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		uint32_t frameCount_ = 0;
		std::vector<VkImage> colorImages_;
		std::vector<VkImageView> colorImageViews_;
		std::vector<VmaAllocation> colorAllocations_;
		std::vector<VkImage> depthImages_;
		std::vector<VkImageView> depthImageViews_;
		std::vector<VmaAllocation> depthAllocations_;
		std::vector<VkImageLayout> colorLayouts_;
		VkSampler colorSampler_ = VK_NULL_HANDLE;
		VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
		VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;
		VmaAllocator allocator_ = VK_NULL_HANDLE;
		VkDevice device_ = VK_NULL_HANDLE;
	};
}
