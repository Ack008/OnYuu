#pragma once
#include <memory>
#include <utility>
namespace OnYuu {
	struct RenderTargetConfig {
		uint32_t width;
		uint32_t height;

	};
	class RenderTarget {
	public:
		RenderTarget(uint32_t width, uint32_t height) : width(width), height(height) {}
		virtual void* getColorAttachment() const = 0;
		virtual void* getDepthAttachment() const = 0;
		static std::shared_ptr<RenderTarget> create(uint32_t width, uint32_t height);
	private:
		uint32_t width, height;

	};
};
