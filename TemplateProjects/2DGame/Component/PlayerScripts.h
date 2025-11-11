#pragma once
#include "Core/Engine.h"
#include "../Prefab/Enemy1.h"
class PlayerScripts : public Component {
public:
	PlayerScripts() = default;
	// Ereditato tramite Component
	virtual void start() override
	{
		Trasform& transform = obj->getComponent<Trasform>();
		transform.position = glm::vec3(0, -35, 0);
	};
	virtual void update(float dt) override
	{
		Trasform& transform = obj->getComponent<Trasform>();
		glm::vec3 pos = transform.position;
		if (Input::isKeyPressed(KeyCode::A))
		{
			if (transform.position.x > -45)
				transform.position.x -= 100 * dt;
		}
		if (Input::isKeyPressed(KeyCode::D))
		{
			if (transform.position.x < 45)
				transform.position.x += 100 * dt;
		}
		if (isGrounded)
		{
			
		}
		isGrounded = false;
	};

	virtual void onCollisionStay(Collider* other) override
	{
		Trasform& otherTransform = other->obj->getComponent<Trasform>();
		if (otherTransform.position.y < obj->getComponent<Trasform>().position.y)
		{
			isGrounded = true;
		}
	};

	virtual void onCollisionEnter(Collider* other) override
	{
		
	}
	int getVite() const {
		return vite;
	}
private:
	int vite = 3.0f;
	bool isGrounded = false;
};