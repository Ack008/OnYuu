#pragma once
#include <vector>
#include "Core/Model/GameObject.h"
#include <optional>
struct TreeComponent {
	std::vector<GameObject> obj;
	GameObject father ;
};