#pragma once
#include <string>
#include <memory>
#include "Core/CubeMap.h"
namespace OnYuu {

struct SkyBoxComponent
{
	std::shared_ptr<CubeMap> cubeMap;
};
}