#include "Shader.h"
#include "OpenGLShader.h"
#include <memory>
#include "API.h"
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