#pragma once
#include "Engine.h"
class EnemyScripts : public Component {
public:
	EnemyScripts() = default;
	// Ereditato tramite Component
	virtual void start() override
	{
		
	};
	virtual void update(float dt) override
	{
		
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
private:
};