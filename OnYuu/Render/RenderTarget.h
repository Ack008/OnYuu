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
		virtual ~RenderTarget() = default;
		virtual void* getColorAttachment() const = 0;
		virtual void* getDepthAttachment() const = 0;
		uint32_t getWidth() const { return width; }
		uint32_t getHeight() const { return height; }
		static std::shared_ptr<RenderTarget> create(uint32_t width, uint32_t height);
	private:
		uint32_t width, height;

	};
};
