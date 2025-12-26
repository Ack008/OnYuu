#include "PhysicsEngine.h"
#include <iostream>
#include "Core/Model/GameObject.h"
namespace OnYuu {
	void PhysicsEngine::update(float dt) {
		//bruteForceCollisionDetection();
		//collisionForPlayerAndBallOnly();
	}

	void PhysicsEngine::bruteForceCollisionDetection()
	{
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

	void PhysicsEngine::collisionForPlayerAndBallOnly()
	{
		//Trovo il giocatore e la palla
		std::vector<Collider*> objectForCollisionTestOnly;
		for (int i = 0; i < colliders.size(); i++) {
			GameObject* obj = colliders[i]->obj.get();
			if (obj->getComponent<TagComponent>().tag == "Player" || obj->getComponent<TagComponent>().tag == "Ball")
			{
				objectForCollisionTestOnly.push_back(&(obj->getComponent<Collider>()));
			}
		}

		//Testo le collisioni tra questi due e gli altri oggetti
		for (size_t i = 0; i < objectForCollisionTestOnly.size(); ++i)
		{
			for (size_t j = 0; j < colliders.size(); ++j)
			{
				if (colliders[j] == objectForCollisionTestOnly[i]) { continue; }
				auto collisionPair = std::minmax(objectForCollisionTestOnly[i], colliders[j]);
				if (checkCollision(objectForCollisionTestOnly[i], colliders[j])) {
					if (collisionPairs.find(collisionPair) == collisionPairs.end()) {
						// New collision detected
						objectForCollisionTestOnly[i]->obj->onCollisionEnter(colliders[j]);
						colliders[j]->obj->onCollisionEnter(objectForCollisionTestOnly[i]);
						collisionPairs.insert(collisionPair);
					}
					objectForCollisionTestOnly[i]->obj->onCollisionStay(colliders[j]);
					colliders[j]->obj->onCollisionStay(objectForCollisionTestOnly[i]);
				}
				else {
					if (collisionPairs.find(collisionPair) != collisionPairs.end()) {
						// Collision ended
						objectForCollisionTestOnly[i]->obj->onCollisionEnd(colliders[j]);
						colliders[j]->obj->onCollisionEnd(objectForCollisionTestOnly[i]);
						collisionPairs.erase(collisionPair);
					}
				}

			}
		}
	}
} // namespace OnYuu