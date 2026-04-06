#pragma once
#include "Application/Layer.h"
#include "Core/Engine.h"
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

	RenderMeshComponent renderMesh;
	RenderMeshComponent renderMesh2;
	RenderMeshComponent renderMesh3;

	std::shared_ptr<Camera> camera;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> shader2;
	std::shared_ptr<RenderTarget> renderTarget;

};
