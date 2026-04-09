#include "VulkanRenderTarget.h"
#include "Render/Renderer.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <stdexcept>
#include <cstdint>

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
		frameCount_ = renderer->getInit().swapchain.image_count;
		if (frameCount_ == 0) {
			frameCount_ = 1;
		}

		colorImages_.resize(frameCount_, VK_NULL_HANDLE);
		colorImageViews_.resize(frameCount_, VK_NULL_HANDLE);
		colorAllocations_.resize(frameCount_, VK_NULL_HANDLE);
		depthImages_.resize(frameCount_, VK_NULL_HANDLE);
		depthImageViews_.resize(frameCount_, VK_NULL_HANDLE);
		depthAllocations_.resize(frameCount_, VK_NULL_HANDLE);
		colorLayouts_.resize(frameCount_, VK_IMAGE_LAYOUT_UNDEFINED);

		for (uint32_t i = 0; i < frameCount_; ++i) {
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

			if (vmaCreateImage(allocator_, &colorImageInfo, &colorAllocInfo, &colorImages_[i], &colorAllocations_[i], nullptr) != VK_SUCCESS) {
				throw std::runtime_error("VulkanRenderTarget: failed to create color image");
			}

			VkImageViewCreateInfo colorViewInfo{};
			colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorViewInfo.image = colorImages_[i];
			colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorViewInfo.format = colorFormat_;
			colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorViewInfo.subresourceRange.baseMipLevel = 0;
			colorViewInfo.subresourceRange.levelCount = 1;
			colorViewInfo.subresourceRange.baseArrayLayer = 0;
			colorViewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device_, &colorViewInfo, nullptr, &colorImageViews_[i]) != VK_SUCCESS) {
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

			if (vmaCreateImage(allocator_, &depthImageInfo, &depthAllocInfo, &depthImages_[i], &depthAllocations_[i], nullptr) != VK_SUCCESS) {
				throw std::runtime_error("VulkanRenderTarget: failed to create depth image");
			}

			VkImageViewCreateInfo depthViewInfo{};
			depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthViewInfo.image = depthImages_[i];
			depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthViewInfo.format = depthFormat_;
			depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			depthViewInfo.subresourceRange.baseMipLevel = 0;
			depthViewInfo.subresourceRange.levelCount = 1;
			depthViewInfo.subresourceRange.baseArrayLayer = 0;
			depthViewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device_, &depthViewInfo, nullptr, &depthImageViews_[i]) != VK_SUCCESS) {
				throw std::runtime_error("VulkanRenderTarget: failed to create depth image view");
			}
		}

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		if (vkCreateSampler(device_, &samplerInfo, nullptr, &colorSampler_) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderTarget: failed to create color sampler");
		}

		{
			VkCommandBuffer cmd = renderer->beginSingleTimeCommands();
			std::vector<VkImageMemoryBarrier> initBarriers;
			initBarriers.reserve(frameCount_);
			for (uint32_t i = 0; i < frameCount_; ++i) {
				VkImageMemoryBarrier b{};
				b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				b.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				b.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				b.image = colorImages_[i];
				b.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				b.subresourceRange.baseMipLevel = 0;
				b.subresourceRange.levelCount = 1;
				b.subresourceRange.baseArrayLayer = 0;
				b.subresourceRange.layerCount = 1;
				b.srcAccessMask = 0;
				b.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				initBarriers.push_back(b);
			}

			vkCmdPipelineBarrier(
				cmd,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				static_cast<uint32_t>(initBarriers.size()), initBarriers.data());

			renderer->endSingleTimeCommands(cmd);
		}

		for (uint32_t i = 0; i < frameCount_; ++i) {
			colorLayouts_[i] = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		if (device_ != VK_NULL_HANDLE) {
			vkDeviceWaitIdle(device_);
			if (colorSampler_ != VK_NULL_HANDLE) {
				vkDestroySampler(device_, colorSampler_, nullptr);
				colorSampler_ = VK_NULL_HANDLE;
			}
			for (auto view : colorImageViews_) {
				if (view != VK_NULL_HANDLE) {
					vkDestroyImageView(device_, view, nullptr);
				}
			}
			for (auto view : depthImageViews_) {
				if (view != VK_NULL_HANDLE) {
					vkDestroyImageView(device_, view, nullptr);
				}
			}
		}
		if (allocator_ != VK_NULL_HANDLE) {
			for (size_t i = 0; i < colorImages_.size(); ++i) {
				if (colorImages_[i] != VK_NULL_HANDLE) {
					vmaDestroyImage(allocator_, colorImages_[i], colorAllocations_[i]);
				}
			}
			for (size_t i = 0; i < depthImages_.size(); ++i) {
				if (depthImages_[i] != VK_NULL_HANDLE) {
					vmaDestroyImage(allocator_, depthImages_[i], depthAllocations_[i]);
				}
			}
		}
	}

	VkImage VulkanRenderTarget::getColorImage(uint32_t frameIndex) const
	{
		if (frameIndex >= colorImages_.size()) {
			return VK_NULL_HANDLE;
		}
		return colorImages_[frameIndex];
	}

	VkImageView VulkanRenderTarget::getColorImageView(uint32_t frameIndex) const
	{
		if (frameIndex >= colorImageViews_.size()) {
			return VK_NULL_HANDLE;
		}
		return colorImageViews_[frameIndex];
	}

	VkImage VulkanRenderTarget::getDepthImage(uint32_t frameIndex) const
	{
		if (frameIndex >= depthImages_.size()) {
			return VK_NULL_HANDLE;
		}
		return depthImages_[frameIndex];
	}

	VkImageView VulkanRenderTarget::getDepthImageView(uint32_t frameIndex) const
	{
		if (frameIndex >= depthImageViews_.size()) {
			return VK_NULL_HANDLE;
		}
		return depthImageViews_[frameIndex];
	}

	VkImageLayout VulkanRenderTarget::getColorLayout(uint32_t frameIndex) const
	{
		if (frameIndex >= colorLayouts_.size()) {
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
		return colorLayouts_[frameIndex];
	}

	void VulkanRenderTarget::setColorLayout(uint32_t frameIndex, VkImageLayout layout)
	{
		if (frameIndex >= colorLayouts_.size()) {
			return;
		}
		colorLayouts_[frameIndex] = layout;
	}

	void* VulkanRenderTarget::getColorAttachment() const
	{
		auto* renderer = static_cast<VulkanRender*>(Render::getInstance().get());
		const uint32_t frame = renderer ? renderer->getCurrentFrame() : 0;
#if defined(VK_USE_64_BIT_PTR_DEFINES) && (VK_USE_64_BIT_PTR_DEFINES == 1)
		return reinterpret_cast<void*>(getColorImageView(frame));
#else
		return reinterpret_cast<void*>(static_cast<uint64_t>(getColorImageView(frame)));
#endif
	}

	void* VulkanRenderTarget::getDepthAttachment() const
	{
		auto* renderer = static_cast<VulkanRender*>(Render::getInstance().get());
		const uint32_t frame = renderer ? renderer->getCurrentFrame() : 0;
#if defined(VK_USE_64_BIT_PTR_DEFINES) && (VK_USE_64_BIT_PTR_DEFINES == 1)
		return reinterpret_cast<void*>(getDepthImage(frame));
#else
		return reinterpret_cast<void*>(static_cast<uint64_t>(getDepthImage(frame)));
#endif
	}
}
