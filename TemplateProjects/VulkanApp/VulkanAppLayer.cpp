#include "VulkanAppLayer.h"
#include "Render/Renderer.h"
#include <ImGui/imgui.h>
extern glm::vec3 clearColorvec;
void VulkanAppLayer::onUpdate(float deltaTime)
{
	
}

void VulkanAppLayer::onEvent()
{
}

void VulkanAppLayer::onImGuiRender()
{
	ImGui::Begin("Vulkan App Layer");
	ImGui::SliderFloat3("Clear Color", &clearColorvec[0], 0.0f, 1.0f);
	ImGui::End();
}

void VulkanAppLayer::onAttach()
{
}

void VulkanAppLayer::onDetach()
{
}
