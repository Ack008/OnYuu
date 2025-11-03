#pragma once
#include <glm/glm.hpp>
#include "Component.h"
#include "Transform.h"
#include "GameObject.h"
#define GRAVITY_ACCELERATION 9.81f
class RigidBody : public Component {
	public:
	enum class BodyType {
		STATIC,
		DYNAMIC,
		KINEMATIC
	};
	RigidBody(BodyType type = BodyType::DYNAMIC, float mass = 1.0f)
		: bodyType(type), mass(mass), velocity(0.0f), acceleration(type == BodyType::DYNAMIC ? glm::vec3(0.0f) : glm::vec3(0,-9.81,0)) {
	}
	virtual void update(float dt) override {
		if (bodyType == BodyType::DYNAMIC) {
			if(_useGravity)
				acceleration += glm::vec3(0.0f, -GRAVITY_ACCELERATION, 0.0f); // Apply gravity
			velocity += acceleration * dt;
			obj->getComponent<Trasform>().position += velocity * dt;
			acceleration = glm::vec3(0.0f); // Reset acceleration after each update
		}
	}
	virtual void onCollisionEnter(Collider* other) override {
		// Simple collision response: invert velocity
		if (bodyType == BodyType::DYNAMIC) {
			velocity = -velocity * 0.5f; // simple bounce with damping
		}
	}
	void applyForce(const glm::vec3& force) {
		if (bodyType == BodyType::DYNAMIC) {
			acceleration += force / mass;
		}
	}
	BodyType getBodyType() const { return bodyType; }
	float getMass() const { return mass; }
	glm::vec3 getVelocity() const { return velocity; }
	void setVelocity(const glm::vec3& vel) { velocity = vel; }
	void setUseGravity(bool useGravity) 
		
	{
		useGravity = useGravity;
	}
private:
	BodyType bodyType;
	float mass;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	bool _useGravity = false;
	

	// Ereditato tramite Component
	void start() {};

};