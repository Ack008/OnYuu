#include "OpenGLImGuiTextureWrapper.h"
#include "Core/Texture.h"
#include "Render/RenderTarget.h"
#include <cstdint>

namespace OnYuu {

OpenGLImGuiTextureWrapper::OpenGLImGuiTextureWrapper(const std::shared_ptr<Texture>& texture)
    : ImGuiTextureWrapper(texture)
{
}

OpenGLImGuiTextureWrapper::OpenGLImGuiTextureWrapper(const std::shared_ptr<RenderTarget>& renderTarget)
    : ImGuiTextureWrapper(renderTarget)
{
}

void* OpenGLImGuiTextureWrapper::getTextureID() const
{
    if (m_renderTarget)
        return m_renderTarget->getColorAttachment();

    if (m_texture)
        return reinterpret_cast<void*>(static_cast<uintptr_t>(m_texture->getID()));

    return nullptr;
}

} // namespace OnYuu
