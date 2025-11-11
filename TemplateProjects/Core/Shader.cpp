#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <memory>
#include "Platform/API.h"
std::shared_ptr<Shader> Shader::create(const char* vertexfilename, const char* fragmentfilename)
{
	switch (apiInUse) {
	case API::OpenGL:
		return std::make_shared<OpenGLShader>(vertexfilename, fragmentfilename);
		break;
	default:
		return nullptr;
	}
}