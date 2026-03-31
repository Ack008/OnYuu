#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"
#include <memory>
#include "Platform/API.h"
#include "Render/Renderer.h"
namespace OnYuu {
std::shared_ptr<Shader> Shader::create(const char* vertexfilename, const char* fragmentfilename, bool isSource)
{
	switch (Render::getAPI()) {
	case API::OpenGL:
		return std::make_shared<OpenGLShader>(vertexfilename, fragmentfilename, isSource);
		break;
	case API::Vulkan:
		// Implementazione VulkanShader da aggiungere
		return std::make_shared<VulkanShader>(vertexfilename, fragmentfilename, isSource);
		break;
	default:
		return nullptr;
	}
} 
}