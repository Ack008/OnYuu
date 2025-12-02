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
	//Skybox
	GameObject skybox = createEntity();
	GameObject controller = createEntity();
	//Oggetti della scena
	GameObject camera = createEntity();
	GameObject cube = createEntity();
	GameObject sphere = createEntity();
	GameObject toro = createEntity();

	//lights
	GameObject lightDirectional = createEntity();
	GameObject lightDirectional2 = createEntity();

	uint32_t width = Application::getInstance()->getWindow()->getWidth();
	uint32_t height = Application::getInstance()->getWindow()->getHeight();
	void createCube();
	void createSphere(glm::vec4 color, int stacks = 20, int slices = 20);
	void createToro(glm::vec4 color);
	void createPyramid();
	void settingLight();
	friend class Game3DLayer;


};