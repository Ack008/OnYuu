#pragma once
#include <string>
#include <memory>
#include "Core/CubeMap.h"
struct SkyBoxComponent
{
	std::shared_ptr<CubeMap> cubeMap;
};