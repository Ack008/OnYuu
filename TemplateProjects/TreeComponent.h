#pragma once
#include <vector>
#include "GameObject.h"
#include <optional>
struct TreeComponent {
	std::vector<GameObject*> obj;
	GameObject* father = nullptr;
};