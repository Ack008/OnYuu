#pragma once
#include "Core/Engine.h"
#include "Panels/SceneHierarchyPanel.h"

class EditorLayer :
	public Layer
{
public:
	EditorLayer() : Layer("EditorLayer") {}
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(/*Event& event*/) override;
	virtual void onImGuiRender() override;
	virtual void onAttach() override;
	virtual void onDetach() override;
private:
	std::shared_ptr<Scene> m_scene;
	std::shared_ptr<Camera> m_editorCamera = nullptr;
	std::shared_ptr<RenderTarget> m_renderTarget;
	SceneHierarchyPanel m_SceneHierarchyPanel;
	

};
