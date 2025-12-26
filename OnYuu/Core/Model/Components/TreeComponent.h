#pragma once
#include <vector>
#include "Core/Model/GameObject.h"
#include <optional>
namespace OnYuu {

struct TreeComponent {
	std::vector<GameObject> obj;
	GameObject father ;
};
}