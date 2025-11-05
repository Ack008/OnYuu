#pragma once
#include "Engine.h"
#include "PlayerScripts.h"
#include <random>
class ControllerScript : public Component 
{
	public:
	ControllerScript(PlayerScripts& player) : player(player) {}
	// Ereditato tramite Component
	virtual void start() override
	{
	};
	virtual void update(float dt) override
	{
		srand((unsigned int)time);
		time += dt;
		if (time >= respawnInterval)
		{
			if (rand() % 2 == 0)
				obj->istantiatePrefab(new Enemy1());
			else
				obj->istantiatePrefab(new Enemy2());
			time = 0;
		}
	};
private:
	float respawnInterval = 7.0f;
	float time = 0;
	PlayerScripts& player;
};