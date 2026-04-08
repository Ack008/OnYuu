#pragma once
#include "Core/Engine.h"

namespace OnYuu {
	class EditorLayer;

	class ViewportPanel
	{
		enum class GizmoOperation {
			Translate,
			Rotate,
			Scale,
			All
		};
	public:
		ViewportPanel();
		~ViewportPanel();
		void onImGuiRender();
		void setLayer(EditorLayer* layer) { m_EditorLayer = layer; }
		void update(float deltaTime);
		bool isFocused() const { return m_isFocused; }
		glm::vec2 getInSpaceMousePosition() const {
			return glm::vec2(mouseInViewport.x, mouseInViewport.y);
		}
	private:
		void handleInput(float deltaTime);
		void mouseInput(float deltaTime, bool& retFlag);
		void keyboardInput(float deltaTime);
		void raycast();
	private:
		EditorLayer* m_EditorLayer = nullptr;
		bool m_isHovered = false;
		bool m_isFocused = false;
		glm::vec2 mouseInViewport = { 0,0 };
		double m_lastMouseX = 0.0;
		double m_lastMouseY = 0.0;
		bool   m_firstClick = true;
		GizmoOperation m_currentGizmoOperation = GizmoOperation::Translate;
	};
}
