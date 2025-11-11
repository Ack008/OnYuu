#pragma once
#include "Core/Engine.h"
class EnemyScripts : public Component {
public:
	EnemyScripts() = default;
	// Ereditato tramite Component
	virtual void start() override
	{
		Trasform& transform = obj->getComponent<Trasform>();
	};
	virtual void update(float dt) override
	{
		time += dt;
		Trasform& transform = obj->getComponent<Trasform>();
		transform.position.y += amplitude * sinf(6.28 * frequency * time + rand()) * dt;
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
			if (tagComp.tag == "Ball")
			{
				obj->Destroy();
			}
		}
	}
	
private:
	float frequency = 2.0f;
	float amplitude = 5.0f;
	float time = 0.0f;
private:
};