#pragma once
#include "Application/ImGuiTextureWrapper.h"

namespace OnYuu {

class OpenGLImGuiTextureWrapper : public ImGuiTextureWrapper {
public:
    OpenGLImGuiTextureWrapper(const std::shared_ptr<Texture>& texture);
    OpenGLImGuiTextureWrapper(const std::shared_ptr<RenderTarget>& renderTarget);
    virtual void* getTextureID() const override;
};

} // namespace OnYuu
