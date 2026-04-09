#pragma once
#include "Application/ImGuiTextureWrapper.h"
#include <vector>

namespace OnYuu {

class VulkanImGuiTextureWrapper : public ImGuiTextureWrapper {
public:
    VulkanImGuiTextureWrapper(const std::shared_ptr<Texture>& texture);
    VulkanImGuiTextureWrapper(const std::shared_ptr<RenderTarget>& renderTarget);
    virtual ~VulkanImGuiTextureWrapper() override;
    virtual void* getTextureID() const override;

private:
    void* m_textureID = nullptr;
    std::vector<void*> m_renderTargetTextureIDs;
};

} // namespace OnYuu
