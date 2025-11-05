#include "BallScript.h"

void BallScript::start()
{
	Trasform& transform = obj->getComponent<Trasform>();
	transform.position = glm::vec3(0, 5, 0);
}

void BallScript::update(float dt)
{
	if (hasCollided) {
		//ritorna alla posizione iniziale
		Trasform& transform = obj->getComponent<Trasform>();
		transform.position = glm::vec3(0, 0, 0);
		RigidBody& rb = obj->getComponent<RigidBody>();
		rb.setVelocity(glm::vec3(0, 0, 0));
		staerted = false;
		hasCollided = false;
	}
	else {

		if (Input::isKeyPressed(KeyCode::Space) && !staerted)
		{
			RigidBody& rb = obj->getComponent<RigidBody>();
			rb.setVelocity(initialVelocity);
			staerted = true;
		}
		Trasform& transform = obj->getComponent<Trasform>();
		RigidBody& rb = obj->getComponent<RigidBody>();
		glm::vec3 vel = rb.getVelocity();
		if (transform.position.x > 50.0f || transform.position.x < -50.0f) 
		{
			vel.x = -vel.x;
			transform.position.x = glm::clamp(transform.position.x, -48.0f, 49.f);
		}
		if (transform.position.y >= 50)
		{
			vel.y = -vel.y;
			transform.position.y = glm::clamp(transform.position.y, -48.0f, 49.f);
		}
		vel.x = glm::clamp(vel.x, -40.0f, 40.0f);
		vel.y = glm::clamp(vel.y, -40.0f, 40.0f);
		rb.setVelocity(vel);
	}
}

void BallScript::onCollisionEnter(Collider* other)
{
	if (other->obj->getComponent<TagComponent>().tag == "Pavimento")
	{
		hasCollided = true;
	} else if(other->obj->getComponent<TagComponent>().tag == "Player")
	{	RigidBody& rb = obj->getComponent<RigidBody>();
		Trasform& ballTransform = obj->getComponent<Trasform>();
		Trasform& playerTransform = other->obj->getComponent<Trasform>();
		glm::vec3 vel = rb.getVelocity();
		float hitPos = (ballTransform.position.x - playerTransform.position.x) / (playerTransform.scale.x / 2.0f);
		if (hitPos < -0.5f) 
		{
			rb.applyForce(glm::vec3(-15.0f, 100, 0.0f));
		}
		else {
			rb.applyForce(glm::vec3(15.0f, 100, 0.0f));
		}
	}
}
