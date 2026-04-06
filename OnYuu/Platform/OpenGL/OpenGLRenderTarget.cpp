#include "OpenGLRenderTarget.h"

#include <stdexcept>

namespace OnYuu {
    OpenGLRenderTarget::OpenGLRenderTarget(uint32_t width, uint32_t height)
        : RenderTarget(width, height), width_(width), height_(height)
    {
        glGenFramebuffers(1, &framebuffer_);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

        glGenTextures(1, &colorTexture_);
        glBindTexture(GL_TEXTURE_2D, colorTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width_), static_cast<GLsizei>(height_), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_, 0);

        glGenTextures(1, &depthTexture_);
        glBindTexture(GL_TEXTURE_2D, depthTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width_), static_cast<GLsizei>(height_), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            throw std::runtime_error("OpenGLRenderTarget: framebuffer incomplete");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenGLRenderTarget::~OpenGLRenderTarget()
    {
        if (depthTexture_ != 0) {
            glDeleteTextures(1, &depthTexture_);
        }
        if (colorTexture_ != 0) {
            glDeleteTextures(1, &colorTexture_);
        }
        if (framebuffer_ != 0) {
            glDeleteFramebuffers(1, &framebuffer_);
        }
    }

    void OpenGLRenderTarget::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    }

    void OpenGLRenderTarget::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void* OpenGLRenderTarget::getColorAttachment() const
    {
        return reinterpret_cast<void*>(static_cast<intptr_t>(colorTexture_));
    }

    void* OpenGLRenderTarget::getDepthAttachment() const
    {
        return reinterpret_cast<void*>(static_cast<intptr_t>(depthTexture_));
    }
}
