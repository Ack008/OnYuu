#include "VulkanAppLayer.h"
#include "Render/Renderer.h"
void VulkanAppLayer::onUpdate(float deltaTime)
{
	Render::getInstance()->draw();
}

void VulkanAppLayer::onEvent()
{
}

void VulkanAppLayer::onImGuiRender()
{
}

void VulkanAppLayer::onAttach()
{
}

void VulkanAppLayer::onDetach()
{
}
