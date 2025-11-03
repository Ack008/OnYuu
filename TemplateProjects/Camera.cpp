#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
void Camera::updateViewMatrix()
{
	view = glm::mat4(1.0f);
	view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	view = glm::translate(view, -position);
}
Perspective::Perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	updateViewMatrix();
}
void Perspective::OnResize(uint32_t width, uint32_t height)
{
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

Orthographic::Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	projection = glm::ortho(left, right, bottom, top,nearPlane,farPlane);
	this->updateViewMatrix();
}


void Orthographic::OnResize(uint32_t width, uint32_t height)
{
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	float orthoHeight = 10.0f; // Altezza fissa
	float orthoWidth = orthoHeight * aspectRatio;
		// glm::ortho richiede float, non uint32_t, e i parametri devono essere coerenti
	projection = glm::ortho(
		0.0f, static_cast<float>(width),
		0.0f, static_cast<float>(height)
	);
	updateViewMatrix();
}
