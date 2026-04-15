#pragma once
#include "Application/Layer.h"
#include <vulkan/vulkan.h>
namespace OnYuu {
class ImGuiLayer : public Layer {
public:
		ImGuiLayer() : Layer("ImGuiLayer") {}
		virtual ~ImGuiLayer() = default;
		virtual void onUpdate(float deltaTime) override;
		virtual void onEvent(/*Event& event*/) override;
		virtual void onImGuiRender() override;
		virtual void onAttach() override;
		virtual void onDetach() override;
		void begin();
		void end();
private:
	bool vulkanInit(VkDevice device);
	VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
};
} // namespace OnYuu