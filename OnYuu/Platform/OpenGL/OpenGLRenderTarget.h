#pragma once

#include "Render/RenderTarget.h"
#include <glad/glad.h>
#include <cstdint>

namespace OnYuu {
	class OpenGLRenderTarget : public RenderTarget {
	public:
		OpenGLRenderTarget(uint32_t width, uint32_t height);
		~OpenGLRenderTarget() override;

		void bind() const;
		static void unbind();

		GLuint getFramebuffer() const { return framebuffer_; }
		GLuint getColorTexture() const { return colorTexture_; }
		GLuint getDepthTexture() const { return depthTexture_; }
		uint32_t getWidth() const { return width_; }
		uint32_t getHeight() const { return height_; }

		void* getColorAttachment() const override;
		void* getDepthAttachment() const override;

	private:
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		GLuint framebuffer_ = 0;
		GLuint colorTexture_ = 0;
		GLuint depthTexture_ = 0;
	};
}
