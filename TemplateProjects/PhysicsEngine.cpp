#include "PhysicsEngine.h"
#include <iostream>
#include "GameObject.h"
void PhysicsEngine::update(float dt) {
	for (size_t i = 0; i < colliders.size(); ++i) {
		for (size_t j = i + 1; j < colliders.size(); ++j) {
			auto collisionPair = std::minmax(colliders[i], colliders[j]);
			if (checkCollision(colliders[i], colliders[j])) {
				if (collisionPairs.find(collisionPair) == collisionPairs.end()) {
					// New collision detected
					colliders[i]->obj->onCollisionEnter(colliders[j]);
					colliders[j]->obj->onCollisionEnter(colliders[i]);
					collisionPairs.insert(collisionPair);
				} 
				colliders[i]->obj->onCollisionStay(colliders[j]);
				colliders[j]->obj->onCollisionStay(colliders[i]);
			}
			else {
				if (collisionPairs.find(collisionPair) != collisionPairs.end()) {
					// Collision ended
					colliders[i]->obj->onCollisionEnd(colliders[j]);
					colliders[j]->obj->onCollisionEnd(colliders[i]);
					collisionPairs.erase(collisionPair);
				}
			}
		}
	}
}