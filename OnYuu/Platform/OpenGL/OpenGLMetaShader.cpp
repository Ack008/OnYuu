#include "OpenGLMetaShader.h"
OnYuu::OpenGLMetaShader::OpenGLMetaShader(const std::string& filename)
	: MetaShader(filename)
{
	glVisitor = OpenGLVisitor(&getSemanticVisitor());
	glVisitor.produceShaders(getGLSLVisitor().getShaderInfo());
	std::cout << "Vertex Shader Code:\n" << glVisitor.getVertexShaderCode() << std::endl;
	std::cout << "Fragment Shader Code:\n" << glVisitor.getFragmentShaderCode() << std::endl;
	setShader(Shader::create(glVisitor.getVertexShaderCode().c_str(), glVisitor.getFragmentShaderCode().c_str()));
}

void OnYuu::OpenGLMetaShader::setUniformMat4(const char* name, const float* value)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformMat4(transformedName.c_str(), value);
	}
}

void OnYuu::OpenGLMetaShader::setUniformInt(const char* name, int value)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformInt(transformedName.c_str(), value);
	}
}

void OnYuu::OpenGLMetaShader::setUniformFloat(const char* name, float value)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformFloat(transformedName.c_str(), value);
	}
}

void OnYuu::OpenGLMetaShader::setUniformVec2(const char* name, const float* value)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformVec2(transformedName.c_str(), value);
	}
}

void OnYuu::OpenGLMetaShader::setUniformVec3(const char* name, const float* value)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformVec3(transformedName.c_str(), value);
	}
}

void OnYuu::OpenGLMetaShader::setUniformVec4(const char* name, const float* value)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformVec4(transformedName.c_str(), value);
	}
}

void OnYuu::OpenGLMetaShader::setUniformMat3(const char* name, const float* value)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformMat3(transformedName.c_str(), value);
	}
}

void OnYuu::OpenGLMetaShader::setUniformMat4(const char* name, const float* value, int count)
{
	if (glVisitor.getUsedRandomizedNames().count(name) > 0) {
		std::string transformedName = glVisitor.getRandomizedNames().at(name).c_str();
		getShader()->setUniformMat4(transformedName.c_str(), value, count);
	}
}
