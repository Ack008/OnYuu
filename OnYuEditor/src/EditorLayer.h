#pragma once
#include "Core/Engine.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ViewportPanel.h"
#include "EditorCamera.h"

namespace OnYuu {

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

	std::shared_ptr<RenderTarget> getRenderTarget() const { return m_renderTarget; }

private:
	std::shared_ptr<Scene> m_scene;
	EditorCamera m_editorCamera;
	std::shared_ptr<Perspective> m_perspectiveCamera;
	std::shared_ptr<RenderTarget> m_renderTarget;
	SceneHierarchyPanel m_SceneHierarchyPanel;
	ViewportPanel m_ViewportPanel;
	friend class ViewportPanel;
};

}
