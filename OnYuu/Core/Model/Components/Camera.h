#pragma once
#include <glm/glm.hpp>
namespace OnYuu {

	class Camera
	{
	public:
		Camera()
			: position(0.0f, 0.0f, 0.0f),
			rotation(0.0f, 0.0f, 0.0f),
			projection(1.0f),
			view(1.0f)
		{
		}
		virtual ~Camera() = default;
		virtual void OnResize(uint32_t width, uint32_t height) = 0;
		glm::mat4 getProjectionMatrix() const { return projection; }
		glm::mat4 getViewMatrix() const { return view; }
		glm::mat4 getVPMatrix() const { return projection * view; }
		void setPosition(const glm::vec3& pos) { position = pos; updateViewMatrix(); }
		void setRotation(const glm::vec3& rot) { rotation = rot; updateViewMatrix(); }
		void setActive(bool active) { isActive = active; }
		bool getActive() const { return isActive; }
		void setTarget(const glm::vec3& tgt) { target = tgt; updateViewMatrix(); }
		void setUpVector(const glm::vec3& up) { upVector = up; updateViewMatrix(); }
		void setDirection(const glm::vec3& dir) { direction = glm::normalize(dir); target = position + direction; updateViewMatrix(); }
		glm::vec3 getPosition() const { return position; }
		glm::vec3 getRotation() const { return rotation; }
		glm::vec3 getTarget() const { return target; }
		glm::vec3 getUpVector() const { return upVector; }
		glm::vec3 getDirection() const { return direction; }
	protected:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f); // Punto verso cui la camera è puntata
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f); // Vettore che indica la direzione "up" della camera
		glm::vec3 direction = glm::normalize(target - position); // Vettore che indica la direzione di visione della camera
		virtual void updateViewMatrix() = 0;
	private:
		bool isActive = true;
	};

	class Perspective : public Camera
	{
	public:
		Perspective(float fov, float aspectRatio, float nearPlane, float farPlane);
		virtual void OnResize(uint32_t width, uint32_t height) override;
		virtual void updateViewMatrix() override;
		//gestione proiezione
		typedef struct {
			float fovY; // Campo visivo verticale in gradi
			float aspect; // Rapporto tra larghezza e altezza del viewport
			float near_plane; // Distanza del piano di clipping vicino
			float far_plane; // Distanza del piano di clipping lontano
		} Proj;

		Proj perspectiveProjInfo = { };

	};

	class Orthographic : public Camera
	{
	public:
		Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		virtual void OnResize(uint32_t width, uint32_t height) override;
		virtual void updateViewMatrix() override;
	};
}