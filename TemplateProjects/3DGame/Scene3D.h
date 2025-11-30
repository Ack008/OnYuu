#pragma once
#include "Core/Engine.h"
class Scene3D : public Scene {
	public:
	Scene3D();
	// Ereditato tramite Scene
	void OnResize(uint32_t width, uint32_t height) override;
	virtual void initializeMaterials() override;
	virtual void initializeScene()  override;
	GameObject getController() const {
		return controller;
	}
private:
	GameObject controller = createEntity();
	//Oggetti della scena
	GameObject camera = createEntity();
	GameObject cube = createEntity();
	GameObject sphere = createEntity();

	uint32_t width = Application::getInstance()->getWindow()->getWidth();
	uint32_t height = Application::getInstance()->getWindow()->getHeight();
	void createCube();
	void createSphere(glm::vec4 color);
	friend class Game3DLayer;
	struct Light {
		glm::vec3 position;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

};