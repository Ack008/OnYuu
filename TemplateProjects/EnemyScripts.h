#pragma once
#include "Engine.h"
class EnemyScripts : public Component {
public:
	EnemyScripts() = default;
	// Ereditato tramite Component
	virtual void start() override
	{
		Trasform& transform = obj->getComponent<Trasform>();
		transform.position.x = 50.0f;
		transform.position.y = rand() % 20 - 10;
	};
	virtual void update(float dt) override
	{
		Trasform& transform = obj->getComponent<Trasform>();
		transform.position.y += amplitude * sinf(frequency * Application::getInstance()->getWindow()->getTime()) * dt;
		transform.position.x += amplitude * cosf(frequency * Application::getInstance()->getWindow()->getTime()) * dt;
		transform.position.x += -5.0f * dt;
		if (transform.position.x < -50.0f) {
			obj->Destroy();
		}
	};
	void setFrequency(float freq) {
		frequency = freq;
	}
	void setAmplitude(float amp) {
		amplitude = amp;
	}
	void onCollisionEnter(Collider* other) override
	{
		if (other->obj->hasComponent<TagComponent>())
		{
			TagComponent& tagComp = other->obj->getComponent<TagComponent>();
			if (tagComp.tag == "Player")
			{
				obj->Destroy();
			}
		}
	}
	
private:
	float frequency = 2.0f;
	float amplitude = 5.0f;
private:
};