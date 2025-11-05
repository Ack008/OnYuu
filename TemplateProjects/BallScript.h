#pragma once
#include "Engine.h"
class BallScript : public Component {
	// Ereditato tramite Component
	void start() override;
	void update(float dt) override;
	void onCollisionEnter(Collider* other) override;
public:
	void reset() {
		staerted = false;
		hasCollided = false;
	}
private:
	glm::vec3 initialVelocity = glm::vec3(20, -10, 0.0f);
	bool staerted = false;
	bool hasCollided = false;
};