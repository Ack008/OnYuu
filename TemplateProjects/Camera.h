#pragma once
#include <glm/glm.hpp>
class Camera
{
public:
	Camera()
		: position(0.0f, 0.0f, 0.0f),
		  rotation(0.0f, 0.0f, 0.0f),
		  projection(1.0f),
		  view(1.0f)
	{}
	virtual ~Camera() = default;
	virtual void OnResize(uint32_t width, uint32_t height) = 0;
	glm::mat4 getProjectionMatrix() const { return projection; }
	glm::mat4 getViewMatrix() const { return view; }
	glm::mat4 getVPMatrix() const { return projection * view; }
	void setPosition(const glm::vec3& pos) { position = pos; updateViewMatrix(); }
	void setRotation(const glm::vec3& rot) { rotation = rot; updateViewMatrix(); }

protected:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 projection;
	glm::mat4 view;
	virtual void updateViewMatrix();
private:
	bool isActive = false;
};

class Perspective : public Camera
{
public:
	Perspective(float fov, float aspectRatio, float nearPlane, float farPlane);
	virtual void OnResize(uint32_t width, uint32_t height) override;

};

class Orthographic : public Camera
{
public:
	Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
	virtual void OnResize(uint32_t width, uint32_t height) override;
};
