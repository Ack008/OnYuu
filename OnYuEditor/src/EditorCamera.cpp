#include "EditorCamera.h"
#include <algorithm>
#include <cmath>

OnYuu::EditorCamera::EditorCamera()
{
	m_PerspectiveCamera = std::make_shared<Perspective>(45.0f, 16.0f/9.0f, 0.01f, 100.0f);
	m_Orthocamera = std::make_shared<Orthographic>(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
}

void OnYuu::EditorCamera::moveForward(float dir, float deltaTime)
{
	if (m_currentCameraType != CameraType::Perspective)
        return;
	m_PerspectiveCamera->setPosition(m_PerspectiveCamera->getPosition() + m_PerspectiveCamera->getDirection() * dir * m_moveSpeed * deltaTime);
    m_PerspectiveCamera->setTarget(m_PerspectiveCamera->getPosition() + m_PerspectiveCamera->getDirection());

}

void OnYuu::EditorCamera::moveHorizontal(float dir, float deltaTime)
{
    switch (m_currentCameraType) {
    case CameraType::Orthographic:
        m_Orthocamera->setPosition(m_Orthocamera->getPosition() + glm::vec3(dir * m_moveSpeed * deltaTime, 0.0f, 0.0f));
        break;
    case CameraType::Perspective:
    {
        const glm::vec3 forward = glm::normalize(m_PerspectiveCamera->getDirection());
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::cross(forward, worldUp);
        if (glm::length2(right) < 0.0001f)
            right = glm::cross(forward, glm::vec3(0.0f, 0.0f, 1.0f));
        right = glm::normalize(right);

        m_PerspectiveCamera->setPosition(m_PerspectiveCamera->getPosition() + right * dir * m_moveSpeed * deltaTime);
		m_PerspectiveCamera->setTarget(m_PerspectiveCamera->getPosition() + m_PerspectiveCamera->getDirection());
        break;
    }
    }

}

void OnYuu::EditorCamera::moveVertical(float dir, float deltaTime)
{
    switch (m_currentCameraType) {
    case CameraType::Orthographic:
        m_Orthocamera->setPosition(m_Orthocamera->getPosition() + glm::vec3(0.0f, dir * m_moveSpeed * deltaTime, 0.0f));
        break;
    case CameraType::Perspective:
    {
        const glm::vec3 forward = glm::normalize(m_PerspectiveCamera->getDirection());
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::cross(forward, worldUp);
        if (glm::length2(right) < 0.0001f)
            right = glm::cross(forward, glm::vec3(0.0f, 0.0f, 1.0f));
        right = glm::normalize(right);
        const glm::vec3 up = glm::normalize(glm::cross(right, forward));

		m_PerspectiveCamera->setPosition(m_PerspectiveCamera->getPosition() + up * dir * m_moveSpeed * deltaTime);
        m_PerspectiveCamera->setTarget(m_PerspectiveCamera->getPosition() + m_PerspectiveCamera->getDirection());
        break;
    }
    }
}

void OnYuu::EditorCamera::rotate(float deltaX, float deltaY)
{
    if (m_currentCameraType != CameraType::Perspective)
        return;

    // deltaX/Y sono già pixel di spostamento — scala con sensitivity
    const float sensitivity = 0.1f;
    m_yaw += deltaX * sensitivity;
    m_pitch -= deltaY * sensitivity;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    front.y = std::sin(glm::radians(m_pitch));
    front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));

    m_PerspectiveCamera->setRotation(glm::vec3(m_pitch, m_yaw, 0.0f));
    m_PerspectiveCamera->setDirection(front);
}

