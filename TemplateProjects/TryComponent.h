#pragma once
#include "Component.h"
class TryComponent : public Component
{
	// Ereditato tramite Component
	void update(float dt) override;
	void start() override;
};

