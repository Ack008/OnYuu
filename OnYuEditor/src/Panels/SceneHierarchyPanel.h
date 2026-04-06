#pragma once
#include "Core/Engine.h"
namespace OnYuu {
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& scene);

		void SetContext(const std::shared_ptr<Scene>& scene);

		void OnImGuiRender();

		GameObject GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(GameObject entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(GameObject entity);
		void DrawComponents(GameObject entity);
	private:
		std::shared_ptr<Scene> m_Context;
		GameObject m_SelectionContext;
	};
}