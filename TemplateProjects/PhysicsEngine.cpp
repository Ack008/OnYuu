#include "PhysicsEngine.h"
#include <iostream>
#include "GameObject.h"
void PhysicsEngine::update(float dt) {
	for (size_t i = 0; i < colliders.size(); ++i) {
		for (size_t j = i + 1; j < colliders.size(); ++j) {
			if (checkCollision(colliders[i], colliders[j])) {
				colliders[i]->obj->onCollosion(colliders[j]);
				colliders[j]->obj->onCollosion(colliders[i]);
			}
		}
	}
}