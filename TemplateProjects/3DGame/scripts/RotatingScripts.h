#pragma once
#include "Core/Engine.h"
class RotatingScripts : public Component {
	public:
	RotatingScripts(glm::vec3 rotSpeed)
		: rotationSpeed(rotSpeed) {}
	void start() override {
		// Inizializzazione se necessaria
	}
	void update(float deltaTime) override {
		auto& transform = obj->getComponent<Trasform>();
		transform.rotation += rotationSpeed * deltaTime;
	}
private:
	glm::vec3 rotationSpeed;
};

