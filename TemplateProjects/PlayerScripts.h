#pragma once
#include "Engine.h"
#include "Enemy1.h"
class PlayerScripts : public Component {
public:
	PlayerScripts() = default;
	// Ereditato tramite Component
	virtual void start() override
	{
	};
	virtual void update(float dt) override
	{
		Trasform& transform = obj->getComponent<Trasform>();
		glm::vec3 pos = transform.position;
		if (Input::isKeyPressed(KeyCode::A))
		{
			if (transform.position.x > -45)
				obj->getComponent<Trasform>().position.x -= 20 * dt;
		}
		if (Input::isKeyPressed(KeyCode::D))
		{
			if (transform.position.x < 45)
				transform.position.x += 20 * dt;
		}
		if (isGrounded)
		{
			if (Input::isKeyPressedOnce(KeyCode::Space))
			{
				if (obj->hasComponent<RigidBody>())
					obj->getComponent<RigidBody>().applyForce(glm::vec3(0.0f, 5000, 0));
				for(GameObject& obj : obj->findGameObjectsByTag("Enemy")){
					obj.Destroy();
				}
			}
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
		if (other->obj->hasComponent<TagComponent>())
		{
			TagComponent& tagComp = other->obj->getComponent<TagComponent>();
			if (tagComp.tag == "Enemy")
			{
				vite -= 1.0f;
				obj->istantiatePrefab(new Enemy1());
				if (vite <= 0.0f)
				{
					std::cout << "Game Over!" << std::endl;
				}
			}
			
		}
	}
	int getVite() const {
		return vite;
	}
private:
	int vite = 3.0f;
	bool isGrounded = false;
};