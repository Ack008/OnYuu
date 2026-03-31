#include "OpenGLMetaShader.h"
OnYuu::OpenGLMetaShader::OpenGLMetaShader(const std::string& filename)
	: MetaShader(filename)
{
	glVisitor = OpenGLVisitor(&getSemanticVisitor());
	glVisitor.produceShaders(getGLSLVisitor().getShaderInfo());
	std::cout << "Vertex Shader Code:\n" << glVisitor.getVertexShaderCode() << std::endl;
	std::cout << "Fragment Shader Code:\n" << glVisitor.getFragmentShaderCode() << std::endl;
	setShader(Shader::create(glVisitor.getVertexShaderCode().c_str(), glVisitor.getFragmentShaderCode().c_str(),true));
}

void OnYuu::OpenGLMetaShader::setUniformMat4(const char* name, const float* value)
{
	getShader()->setUniformMat4(getUniformName(name).c_str(), value);	
}

void OnYuu::OpenGLMetaShader::setUniformInt(const char* name, int value)
{
	getShader()->setUniformInt(getUniformName(name).c_str(), value);	
}

void OnYuu::OpenGLMetaShader::setUniformFloat(const char* name, float value)
{
	getShader()->setUniformFloat(getUniformName(name).c_str(), value);
}

void OnYuu::OpenGLMetaShader::setUniformVec2(const char* name, const float* value)
{
	getShader()->setUniformVec2(getUniformName(name).c_str(), value);
}

void OnYuu::OpenGLMetaShader::setUniformVec3(const char* name, const float* value)
{
	getShader()->setUniformVec3(getUniformName(name).c_str(), value);
}

void OnYuu::OpenGLMetaShader::setUniformVec4(const char* name, const float* value)
{
	getShader()->setUniformVec4(getUniformName(name).c_str(), value);
}

void OnYuu::OpenGLMetaShader::setUniformMat3(const char* name, const float* value)
{
	getShader()->setUniformMat3(getUniformName(name).c_str(), value);
}

void OnYuu::OpenGLMetaShader::setUniformMat4(const char* name, const float* value, int count)
{
	getShader()->setUniformMat4(getUniformName(name).c_str(), value, count);
}

std::string OnYuu::OpenGLMetaShader::getUniformName(const std::string& originalName)
{
#ifdef DEBUG
	std::cout << "Used Randomized Names:" << std::endl;
	for (const auto& name : glVisitor.getUsedRandomizedNames()) {
		std::cout << " - " << name << std::endl;
	}

#endif // DEBUG

	if (glVisitor.getRandomizedNames().count(originalName) > 0) {
		return glVisitor.getRandomizedNames().at(originalName);
	}
	return originalName;
}
