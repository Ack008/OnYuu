#include "ViewportPanel.h"
#include "../EditorLayer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "ImGuizmo/ImGuizmo.h"

namespace OnYuu {

	ViewportPanel::ViewportPanel()
	{
	}
	ViewportPanel::~ViewportPanel()
	{
	}

	void ViewportPanel::onImGuiRender()
	{
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		const float viewportAspectRatio = 16.0f / 9.0f;
		ImVec2 imageSize = viewportPanelSize;
		if (viewportPanelSize.x > 0.0f && viewportPanelSize.y > 0.0f)
		{
			float width = viewportPanelSize.x;
			float height = width / viewportAspectRatio;
			if (height > viewportPanelSize.y)
			{
				height = viewportPanelSize.y;
				width = height * viewportAspectRatio;
			}
			imageSize = ImVec2(width, height);
		}

		if (m_EditorLayer && m_EditorLayer->getRenderTarget())
		{
			void* textureID = m_EditorLayer->m_renderTarget->getColorAttachment();
			ImGui::Image(textureID, imageSize, ImVec2(0, 1), ImVec2(1, 0));

		}

		// Gizmo

		GameObject selectedObject = m_EditorLayer->m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedObject) {

			ImGuizmo::SetOrthographic(m_EditorLayer->m_editorCamera.getCameraType() == CameraType::Orthographic);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y);
			// get camera matrices
			auto camera = m_EditorLayer->m_editorCamera.getCamera();
			glm::mat4 cameraView =  camera->getViewMatrix();
			glm::mat4 cameraProjection = camera->getProjectionMatrix();
			if (Render::getAPI() == Vulkan)
			{
				cameraProjection[1][1] *= -1.0f;
			

			}
			auto& tc = selectedObject.getComponent<Trasform>();
			glm::mat4 modelMatrix = tc.getModelMatrix();
			ImGuizmo::OPERATION currentOperation = ImGuizmo::OPERATION::TRANSLATE;
			switch (m_currentGizmoOperation) {
			case GizmoOperation::Translate:
				currentOperation = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case GizmoOperation::Rotate:
				currentOperation = ImGuizmo::OPERATION::ROTATE;
				break;
			case GizmoOperation::Scale:
				currentOperation = ImGuizmo::OPERATION::SCALE;
				break;
			case GizmoOperation::All:
				currentOperation = ImGuizmo::OPERATION::UNIVERSAL;
				break;
			}
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (currentOperation == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), currentOperation, ImGuizmo::WORLD, glm::value_ptr(modelMatrix), nullptr, snapValues);
			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(modelMatrix, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;

				tc.position = translation;
				tc.rotation = rotation;
				tc.scale = scale;

			}

				
		}
		// Input
		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 imageMin = ImGui::GetItemRectMin();   // angolo in alto a sinistra dell'immagine
		ImVec2 imageMax = ImGui::GetItemRectMax();   // angolo in basso a destra

		mouseInViewport = {
			mousePos.x - imageMin.x,
			mousePos.y - imageMin.y
		};
		mouseInViewport /= glm::vec2(imageMax.x - imageMin.x, imageMax.y - imageMin.y);
		if (ImGui::IsItemHovered()) {
			m_isHovered = true;
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		else {
			m_isHovered = false;
		}
		m_isFocused = ImGui::IsWindowFocused();
		ImGui::End();
	}

	void ViewportPanel::update(float deltaTime){
		if (m_isFocused) {
			keyboardInput(deltaTime);
		}
			bool retFlag;
			mouseInput(deltaTime, retFlag);
		if (m_isHovered) {
		}
	}

	void ViewportPanel::handleInput(float deltaTime) {
		bool retFlag;
		mouseInput(deltaTime, retFlag);
		if (retFlag) return;

		keyboardInput(deltaTime);
	}

	void ViewportPanel::mouseInput(float deltaTime, bool& retFlag)
	{
		retFlag = true;
		if (Input::isMouseButtonPressed(1)) {
			double currentX, currentY;
			Input::getMouseWindowPos(currentX, currentY);

			if (m_firstClick) {
				// Prima volta che si preme: inizializza la posizione, nessuna rotazione ancora
				m_lastMouseX = currentX;
				m_lastMouseY = currentY;
				m_firstClick = false;
				Input::lockMouse(true);
				return;
			}

			double deltaX = currentX - m_lastMouseX;
			double deltaY = currentY - m_lastMouseY;

			m_lastMouseX = currentX;
			m_lastMouseY = currentY;

			if (m_EditorLayer->m_editorCamera.getCameraType() == CameraType::Perspective) {

				float yDir = (Render::getAPI() == Vulkan) ? -(float)deltaY : (float)deltaY;
				m_EditorLayer->m_editorCamera.rotate((float)deltaX, yDir);
			}
			else {
				m_EditorLayer->m_editorCamera.moveHorizontal((float)deltaX, deltaTime);
				m_EditorLayer->m_editorCamera.moveVertical((float)deltaY, deltaTime);
			}
			Input::lockMouse(true);
			
			
		}
		else {
			m_firstClick = true; // reset per la prossima pressione
			Input::lockMouse(false);

		}
		retFlag = false;



		if (Input::isMouseButtonPressed(0)) {
			if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
				raycast();
		}

	}

	void ViewportPanel::keyboardInput(float deltaTime)
	{
		float vertDir = (Render::getAPI() == Vulkan) ? -1.0f : 1.0f;

		if (Input::isKeyPressed(KeyCode::W)) {
			m_EditorLayer->m_editorCamera.moveForward(1.0f, deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::S)) {
			m_EditorLayer->m_editorCamera.moveForward(-1.0f, deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::A)) {
			m_EditorLayer->m_editorCamera.moveHorizontal(-1.0f, deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::D)) {
			m_EditorLayer->m_editorCamera.moveHorizontal(1.0f, deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::Q)) {
			m_EditorLayer->m_editorCamera.moveVertical(-1.0f * vertDir, deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::E)) {
			m_EditorLayer->m_editorCamera.moveVertical(1.0f * vertDir, deltaTime);
		}
		if (Input::isKeyPressedOnce(KeyCode::Z)) {
			if (m_EditorLayer->m_editorCamera.getCameraType() == CameraType::Perspective)
				m_EditorLayer->m_editorCamera.setCameraType(CameraType::Orthographic);
			else
				m_EditorLayer->m_editorCamera.setCameraType(CameraType::Perspective);
		}

		if (Input::isKeyPressedOnce(KeyCode::D1)) {
			m_currentGizmoOperation = GizmoOperation::Translate;
		}
		if (Input::isKeyPressedOnce(KeyCode::D2)) {
			m_currentGizmoOperation = GizmoOperation::Rotate;
		}
		if (Input::isKeyPressedOnce(KeyCode::D3)) {
			m_currentGizmoOperation = GizmoOperation::Scale;
		}
		if (Input::isKeyPressedOnce(KeyCode::D4)) {
			m_currentGizmoOperation = GizmoOperation::All;
		}
	}

	void ViewportPanel::raycast()
	{
		if (mouseInViewport.x < 0.0f || mouseInViewport.x > 1.0f || mouseInViewport.y < 0.0f || mouseInViewport.y > 1.0f)
			return;

		auto camera = m_EditorLayer->m_editorCamera.getCamera();
		const glm::mat4 invVP = glm::inverse(camera->getVPMatrix());

		const float ndcX = mouseInViewport.x * 2.0f - 1.0f;
		const float ndcY = 1.0f - mouseInViewport.y * 2.0f;

		glm::vec4 nearPoint = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
		glm::vec4 farPoint = invVP * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

		if (nearPoint.w == 0.0f || farPoint.w == 0.0f)
			return;

		nearPoint /= nearPoint.w;
		farPoint /= farPoint.w;

		glm::vec3 origin = camera->getPosition();
		if (m_EditorLayer->m_editorCamera.getCameraType() == CameraType::Orthographic)
			origin = glm::vec3(nearPoint);

		glm::vec3 direction = glm::normalize(glm::vec3(farPoint - nearPoint));
		GameObject selectedObject = m_EditorLayer->m_scene->editorRaycast(origin, direction, 1000.0f);
		if (selectedObject) {
			std::string tag = selectedObject.getComponent<TagComponent>().tag;
		}
		m_EditorLayer->m_SceneHierarchyPanel.SetSelectedEntity(selectedObject);
	}

}
