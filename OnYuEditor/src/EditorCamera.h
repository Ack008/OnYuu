#pragma once
#include "Core/Engine.h"
namespace OnYuu {
	enum class CameraType {
		Orthographic,
		Perspective
	};
	class EditorCamera {
	public:
		EditorCamera();
		void moveForward(float dir, float deltaTime);
		void moveHorizontal(float dir, float deltaTime);
		void moveVertical(float dir, float deltaTime);
		void rotate(float deltaX, float deltaY);
		void setCameraType(CameraType type) { m_currentCameraType = type; }
		CameraType getCameraType() const { return m_currentCameraType; }
		std::shared_ptr<Camera> getCamera() {
			if (m_currentCameraType == CameraType::Orthographic)
				return m_Orthocamera;
			else
				return m_PerspectiveCamera;
		}

	private:
		std::shared_ptr<Orthographic> m_Orthocamera;
		std::shared_ptr<Perspective> m_PerspectiveCamera;
		CameraType m_currentCameraType = CameraType::Perspective;
		float m_moveSpeed = 5.0f; // Velocità di movimento
		float m_rotationSpeed = 0.1f; // Velocità di rotazione
		float m_yaw = -90.0f;
		float m_pitch = 0.0f;
	};
}