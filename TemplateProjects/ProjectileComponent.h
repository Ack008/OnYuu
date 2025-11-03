#pragma once
#include "Component.h"
#include "GameObject.h" // Aggiungi questa riga per includere la definizione completa di GameObject

class ProjectileComponent : public Component {
	void update(float dt) override {
		time += dt;
		if (time >= 5.0f) {
			obj->Destroy();
			std::cout << "Projectile destroyed after 5 seconds\n";
		}
	

	}
	void start() override {
		BoxCollider& col = obj->getComponent<BoxCollider>();
		auto min = col.getMinPoint();
		auto max = col.getMaxPoint();
		std::cout << "Min point: " << min.x << " " << min.y << " " << min.z;
		std::cout << "Max point: " << max.x << " " << max.y << " " << max.z;
		Trasform& transform = obj->getComponent<Trasform>();
		transform.scale = glm::vec3(200, 200, 1.0f);
	}
	void onCollisionEnter(Collider* other) override {
		
	}
	void onCollisionStay(Collider* other) override {
		obj->Destroy();
		std::cout << "Projectile destroyed on collision\n";
	}
private:
	float time = 0.0f;
};