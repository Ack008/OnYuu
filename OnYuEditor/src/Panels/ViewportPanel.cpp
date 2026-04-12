#include "ViewportPanel.h"
#include "../EditorLayer.h"
#include "Application/ImGuiTextureWrapper.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
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

		if (m_EditorLayer)
		{
			auto renderTarget = m_EditorLayer->getRenderTarget();
			if (renderTarget)
			{
				if (!m_viewportTextureWrapper || m_boundRenderTarget != renderTarget)
				{
					m_viewportTextureWrapper = ImGuiTextureWrapper::create(renderTarget);
					m_boundRenderTarget = renderTarget;
				}

				void* textureID = m_viewportTextureWrapper ? m_viewportTextureWrapper->getTextureID() : nullptr;
				if (textureID)
					ImGui::Image(textureID, imageSize, ImVec2(0, 1), ImVec2(1, 0));
				else
					ImGui::Dummy(imageSize);
			}
			else
			{
				m_viewportTextureWrapper.reset();
				m_boundRenderTarget.reset();
				ImGui::Dummy(imageSize);
			}
		}
		else
		{
			ImGui::Dummy(imageSize);
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
			auto& tc = selectedObject.getComponent<Transform>();
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
			bool useSnap = Input::isKeyPressed(KeyCode::LeftControl) || Input::isKeyPressed(KeyCode::RightControl);
			glm::mat4 deltaMatrix = glm::mat4(1.0f);

			ImGuizmo::Manipulate(
				glm::value_ptr(cameraView),
				glm::value_ptr(cameraProjection),
				currentOperation,
				ImGuizmo::WORLD,
				glm::value_ptr(modelMatrix),
				glm::value_ptr(deltaMatrix),   // <-- era nullptr
				useSnap ? snapValues : nullptr
			);

			if (ImGuizmo::IsUsing()) {
				float translation[3], rotation[3], scale[3];

				// Translate e Scale: decomposizione completa va bene
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), translation, rotation, scale);
				tc.setPosition({ translation[0], translation[1], translation[2] });
				tc.setScale({ scale[0], scale[1], scale[2] });

				// Rotation: accumula il delta invece di ridecomporre
				float dT[3], dR[3], dS[3];
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(deltaMatrix), dT, dR, dS);
				tc.setRotation(tc.getRotation() + glm::vec3(dR[0], dR[1], dR[2]));
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

		const bool rightMouseDown = Input::isMouseButtonPressed(1);
		if (m_isHovered || m_isRightMouseControlling || rightMouseDown) {
			bool retFlag;
			mouseInput(deltaTime, retFlag);
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
			if (!m_isRightMouseControlling) {
				if (!m_isHovered || !m_isFocused) {
					retFlag = false;
					return;
				}
				m_isRightMouseControlling = true;
				m_firstClick = true;
			}

			double currentX, currentY;
			Input::getMouseWindowPos(currentX, currentY);

			if (m_firstClick) {
				m_lastMouseX = currentX;
				m_lastMouseY = currentY;
				m_firstClick = false;
				Input::lockMouse(true);
				retFlag = false;
				return;
			}

			double deltaX = currentX - m_lastMouseX;
			double deltaY = currentY - m_lastMouseY;

			m_lastMouseX = currentX;
			m_lastMouseY = currentY;

			if (m_EditorLayer->m_editorCamera.getCameraType() == CameraType::Perspective) {
				m_EditorLayer->m_editorCamera.rotate((float)deltaX, (float)deltaY);
			}
			else {
				m_EditorLayer->m_editorCamera.moveHorizontal((float)deltaX, deltaTime);
				m_EditorLayer->m_editorCamera.moveVertical((float)deltaY, deltaTime);
			}
			Input::lockMouse(true);
		}
		else {
			m_firstClick = true;
			if (m_isRightMouseControlling) {
				Input::lockMouse(false);
			}
			m_isRightMouseControlling = false;
		}
		retFlag = false;

		if (Input::isMouseButtonPressed(0)) {
			if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
				raycast();
		}
	}

	void ViewportPanel::keyboardInput(float deltaTime)
	{
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
			m_EditorLayer->m_editorCamera.moveVertical(-1.0f, deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::E)) {
			m_EditorLayer->m_editorCamera.moveVertical(1.0f, deltaTime);
		}
		if (Input::isKeyPressedOnce(KeyCode::Z)) {
			if (m_EditorLayer->m_editorCamera.getCameraType() == CameraType::Perspective)
				m_EditorLayer->m_editorCamera.setCameraType(CameraType::Orthographic);
			else
				m_EditorLayer->m_editorCamera.setCameraType(CameraType::Perspective);
		}

		if (Input::isKeyPressedOnce(KeyCode::T) || Input::isKeyPressedOnce(KeyCode::D1)) {
			m_currentGizmoOperation = GizmoOperation::Translate;
		}
		if (Input::isKeyPressedOnce(KeyCode::R) || Input::isKeyPressedOnce(KeyCode::D2)) {
			m_currentGizmoOperation = GizmoOperation::Rotate;
		}
		if (Input::isKeyPressedOnce(KeyCode::X) || Input::isKeyPressedOnce(KeyCode::D3)) {
			m_currentGizmoOperation = GizmoOperation::Scale;
		}
		if (Input::isKeyPressedOnce(KeyCode::G) || Input::isKeyPressedOnce(KeyCode::D4)) {
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
