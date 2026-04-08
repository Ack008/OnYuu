#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace OnYuu {
Perspective::Perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	this->perspectiveProjInfo.fovY = fov;
	this->perspectiveProjInfo.aspect = aspectRatio;
	this->perspectiveProjInfo.near_plane = nearPlane;
	this->perspectiveProjInfo.far_plane = farPlane;
	updateViewMatrix();
}
void Perspective::OnResize(uint32_t width, uint32_t height)
{
	if (height == 0)
		return;

	perspectiveProjInfo.aspect = static_cast<float>(width) / static_cast<float>(height);
	projection = glm::perspective(
		glm::radians(perspectiveProjInfo.fovY),
		perspectiveProjInfo.aspect,
		perspectiveProjInfo.near_plane,
		perspectiveProjInfo.far_plane
	);
}

void Perspective::updateViewMatrix()
{
	view = glm::lookAt(glm::vec3(position), glm::vec3(target), glm::vec3(upVector));
}

Orthographic::Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	this->updateViewMatrix();
}


void Orthographic::OnResize(uint32_t width, uint32_t height)
{
	if (height == 0)
		return;

	const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	const float orthoHeight = 10.0f;
	const float orthoWidth = orthoHeight * aspectRatio;
	projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.1f, 100.0f);
	updateViewMatrix();
}

void Orthographic::updateViewMatrix()
{
	view = glm::mat4(1.0f);
	view = glm::rotate(view, glm::radians(-rotation.z), glm::vec3(0, 0, 1));
	view = glm::rotate(view, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
	view = glm::rotate(view, glm::radians(-rotation.x), glm::vec3(1, 0, 0));
	view = glm::translate(view, -position);
}
} // namespace OnYuu