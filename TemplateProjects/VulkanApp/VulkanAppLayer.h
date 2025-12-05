#pragma once
#include "Application/Layer.h"
class VulkanAppLayer :
	public Layer
{
	public:
	VulkanAppLayer() : Layer("VulkanAppLayer") {}
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(/*Event& event*/) override;
	virtual void onImGuiRender() override;
	virtual void onAttach() override;
	virtual void onDetach() override;
};
