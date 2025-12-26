#include "CubeMap.h"
#include "Platform/OpenGL/OpenGLCubeMap.h"
#include "Platform/API.h"
#include "Render/Renderer.h"
namespace OnYuu {
std::shared_ptr<CubeMap> CubeMap::createCubeMap(const std::vector<std::string>& faces) {
	switch(Render::getAPI())
	{
		case API::OpenGL:
			return std::make_shared<OpenGLCubeMap>(faces);
		default:
			break;
	}
}
}