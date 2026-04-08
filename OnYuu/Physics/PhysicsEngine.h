#pragma once
#include <vector>
#include <set>
#include "Core/Model/Components/Collider.h"
namespace OnYuu {
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

	std::vector<Collider*> getColliders() const { return colliders; }
	

	void update(float dt);

private:
	void bruteForceCollisionDetection();
	void collisionForPlayerAndBallOnly();
private:
	std::vector<Collider*> colliders;
	std::set<std::pair<Collider*, Collider*>> collisionPairs;
	
};

} // namespace OnYuu