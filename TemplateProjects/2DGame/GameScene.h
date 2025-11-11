#pragma once
#include "Core/Engine.h"
class GameScene : public Scene {
public:
	GameScene();
	// Ereditato tramite Scene
	void OnResize(uint32_t width, uint32_t height) override;
private:
	GameObject camera = createEntity();
	GameObject player = createEntity();
	GameObject background = createEntity();
	GameObject pavimento = createEntity();
	GameObject controller = createEntity();
	GameObject ball = createEntity();
	uint32_t width = Application::getInstance()->getWindow()->getWidth();
	uint32_t height = Application::getInstance()->getWindow()->getHeight();
	friend class GameLayer;
};