#include "CubeMap.h"
#include "Platform/OpenGL/OpenGLCubeMap.h"
#include "Platform/API.h"
std::shared_ptr<CubeMap> CubeMap::createCubeMap(const std::vector<std::string>& faces) {
	switch(apiInUse)
	{
		case API::OpenGL:
			return std::make_shared<OpenGLCubeMap>(faces);
		default:
			break;
	}
}