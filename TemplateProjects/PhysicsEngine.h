#pragma once
#include <vector>
#include <set>
#include "Collider.h"
class PhysicsEngine {
public:
	struct CollisionInfo {
		Collider* other;
		glm::vec3 normal; // direzione dalla quale l’altro collider “spinge”
		float penetrationDepth;
	};
	static bool checkCollision(Collider* a, Collider* b) {
		return a->hasCollided(b);
	}
	void addCollider(Collider* collider) {
		colliders.push_back(collider);
	}
	void removeCollider(Collider* collider) {
		colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
	}
	void update(float dt);

private:
	std::vector<Collider*> colliders;
	std::set<std::pair<Collider*, Collider*>> collisionPairs;
	
};
