#pragma once
#include "Core/CubeMap.h"
#include <vector>
#include <string>
class OpenGLCubeMap : public CubeMap {
	public:
	OpenGLCubeMap(const std::vector<std::string>& faces);
	virtual void bind(unsigned int unit = 0) override;
private:
	unsigned int ID;
};