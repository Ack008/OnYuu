#include "VulkanImGuiTextureWrapper.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Platform/Vulkan/VulkanRenderTarget.h"
#include "Platform/Vulkan/VulkanRender.h"
#include "Render/Renderer.h"
#include <ImGui/backends/imgui_impl_vulkan.h>

namespace OnYuu {

VulkanImGuiTextureWrapper::VulkanImGuiTextureWrapper(const std::shared_ptr<Texture>& texture)
    : ImGuiTextureWrapper(texture)
{
    if (!m_texture)
        return;

    auto vulkanTexture = std::dynamic_pointer_cast<VulkanTexture>(m_texture);
    if (!vulkanTexture)
        return;

    m_textureID = reinterpret_cast<void*>(ImGui_ImplVulkan_AddTexture(
        vulkanTexture->getSampler(),
        vulkanTexture->getImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
}

VulkanImGuiTextureWrapper::VulkanImGuiTextureWrapper(const std::shared_ptr<RenderTarget>& renderTarget)
    : ImGuiTextureWrapper(renderTarget)
{
    if (!m_renderTarget)
        return;

    auto vulkanRenderTarget = std::dynamic_pointer_cast<VulkanRenderTarget>(m_renderTarget);
    if (!vulkanRenderTarget)
        return;

    const uint32_t frameCount = vulkanRenderTarget->getFrameCount() == 0 ? 1 : vulkanRenderTarget->getFrameCount();
    m_renderTargetTextureIDs.resize(frameCount, nullptr);

    for (uint32_t i = 0; i < frameCount; ++i)
    {
        VkImageLayout layout = vulkanRenderTarget->getColorLayout(i);
        if (layout == VK_IMAGE_LAYOUT_UNDEFINED)
            layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkDescriptorSet descriptorSet = ImGui_ImplVulkan_AddTexture(
            vulkanRenderTarget->getColorSampler(),
            vulkanRenderTarget->getColorImageView(i),
            layout);

        m_renderTargetTextureIDs[i] = reinterpret_cast<void*>(descriptorSet);
    }
}

VulkanImGuiTextureWrapper::~VulkanImGuiTextureWrapper()
{
    auto* renderer = static_cast<VulkanRender*>(Render::getInstance().get());
    if (renderer && renderer->getInit().device != VK_NULL_HANDLE)
        vkDeviceWaitIdle(renderer->getInit().device);

    for (void* textureID : m_renderTargetTextureIDs)
    {
        if (textureID != nullptr)
            ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(textureID));
    }
    m_renderTargetTextureIDs.clear();

    if (m_textureID != nullptr)
    {
        ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(m_textureID));
        m_textureID = nullptr;
    }
}

void* VulkanImGuiTextureWrapper::getTextureID() const
{
    if (!m_renderTargetTextureIDs.empty())
    {
        auto* renderer = static_cast<VulkanRender*>(Render::getInstance().get());
        const uint32_t frame = renderer ? renderer->getCurrentFrame() : 0;
        if (frame < m_renderTargetTextureIDs.size())
            return m_renderTargetTextureIDs[frame];

        return m_renderTargetTextureIDs[0];
    }

    return m_textureID;
}

} // namespace OnYuu
