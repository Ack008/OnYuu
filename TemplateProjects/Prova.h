#pragma once
#include "Engine.h"
class Prova : public Scene {
public:
	Prova();
	~Prova() = default;
	void OnResize(uint32_t width, uint32_t height) override;
private:
	GameObject Player = createEntity();
	GameObject Asteroide = createEntity();
	GameObject Camera = createEntity();
	GameObject background = createEntity();
	
};