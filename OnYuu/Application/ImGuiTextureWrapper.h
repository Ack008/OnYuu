#pragma once
#include <memory>

namespace OnYuu {

class Texture;
class RenderTarget;

class ImGuiTextureWrapper {
public:
	virtual ~ImGuiTextureWrapper() = default;
	virtual void* getTextureID() const = 0;

	static std::shared_ptr<ImGuiTextureWrapper> create(const std::shared_ptr<Texture>& texture);
	static std::shared_ptr<ImGuiTextureWrapper> create(const std::shared_ptr<RenderTarget>& renderTarget);

protected:
	ImGuiTextureWrapper(std::shared_ptr<Texture> texture) : m_texture(std::move(texture)) {}
	ImGuiTextureWrapper(std::shared_ptr<RenderTarget> renderTarget) : m_renderTarget(std::move(renderTarget)) {}
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<RenderTarget> m_renderTarget;
};

} // namespace OnYuu
