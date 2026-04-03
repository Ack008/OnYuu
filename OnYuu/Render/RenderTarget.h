#pragma once
#include <utility>
namespace OnYuu {
	class RenderTarget {
	public:
		RenderTarget(uint32_t width, uint32_t height) : width(width), height(height) {}
		virtual void* getColorAttachment() const = 0;
		virtual void* getDepthAttachment() const = 0;
	private:
		uint32_t width, height;

	};
};
