#pragma once
#include "Component.h"
#include "GameObject.h" 
#include "TagComponent.h"
class ProjectileComponent : public Component {
	void update(float dt) override {
		time += dt;
		if (time >= 5.0f) {
			obj->Destroy();
			std::cout << "Projectile destroyed after 5 seconds\n";
		}
		Trasform& transform = obj->getComponent<Trasform>();
		transform.position += vel * 1000.0f * dt;

	}
	void start() override {
		
	}
	void onCollisionEnter(Collider* other) override {
		if (other->obj->hasComponent<TagComponent>()) {
			TagComponent& tagComp = other->obj->getComponent<TagComponent>();
			if (tagComp.tag == "Asteroide") {
				std::cout << "Projectile collided with Asteroide\n";
				obj->Destroy();
			}
		}
		
	}
	void onCollisionStay(Collider* other) override {

	}
private:
	float time = 0.0f;
	glm::vec3 vel = glm::vec3(1.0f, 0.0f, 0.0f);
};