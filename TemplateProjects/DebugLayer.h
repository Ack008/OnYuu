#pragma once
#include "Scene.h"
#include "Layer.h"
#include "Material.h"
#include <memory>
#include "MeshComponent.h"
class DebugLayer : public Layer {
	public:
	DebugLayer(Scene* scene) : Layer("DebugLayer"), currentScene(scene) {}
	virtual ~DebugLayer() = default;
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(/*Event& event*/) override;
	virtual void onImGuiRender() override;
	virtual const char* getName() const override { return "DebugLayer"; }
	virtual void onAttach() override;
	virtual void onDetach() override;

private:
	Scene* currentScene = nullptr;
	std::shared_ptr<Material> debugMaterial;
	std::shared_ptr<Shader> debugShader;
	std::vector<std::unique_ptr<RenderMeshComponent>> debugMeshes;
	std::vector<std::shared_ptr<Mesh>> debugMeshData;
};