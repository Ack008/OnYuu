#include "VulkanMetaShader.h"
OnYuu::VulkanMetaShader::VulkanMetaShader(const std::string& filename, bool isShaderSource)
	: MetaShader(filename, isShaderSource)
{
	vKVisitor = VulkanVisitor(&getSemanticVisitor());
	vKVisitor.produceShaders(getGLSLVisitor().getShaderInfo());
	std::cout << "Vertex Shader Code:\n" << vKVisitor.getVertexShaderCode() << std::endl;
	std::cout << "Fragment Shader Code:\n" << vKVisitor.getFragmentShaderCode() << std::endl;
	setShader(Shader::create(vKVisitor.getVertexShaderCode().c_str(), vKVisitor.getFragmentShaderCode().c_str(),true));
}

void OnYuu::VulkanMetaShader::setUniformMat4(const char* name, const float* value)
{
	getShader()->setUniformMat4(getUniformName(name).c_str(), value);	
}

void OnYuu::VulkanMetaShader::setUniformInt(const char* name, int value)
{
	getShader()->setUniformInt(getUniformName(name).c_str(), value);	
}

void OnYuu::VulkanMetaShader::setUniformFloat(const char* name, float value)
{
	getShader()->setUniformFloat(getUniformName(name).c_str(), value);
}

void OnYuu::VulkanMetaShader::setUniformVec2(const char* name, const float* value)
{
	getShader()->setUniformVec2(getUniformName(name).c_str(), value);
}

void OnYuu::VulkanMetaShader::setUniformVec3(const char* name, const float* value)
{
	getShader()->setUniformVec3(getUniformName(name).c_str(), value);
}

void OnYuu::VulkanMetaShader::setUniformVec4(const char* name, const float* value)
{
	getShader()->setUniformVec4(getUniformName(name).c_str(), value);
}

void OnYuu::VulkanMetaShader::setUniformMat3(const char* name, const float* value)
{
	getShader()->setUniformMat3(getUniformName(name).c_str(), value);
}

void OnYuu::VulkanMetaShader::setUniformMat4(const char* name, const float* value, int count)
{
	getShader()->setUniformMat4(getUniformName(name).c_str(), value, count);
}

std::string OnYuu::VulkanMetaShader::getUniformName(const std::string& originalName)
{
#ifdef DEBUG
	std::cout << "Used Randomized Names:" << std::endl;
	for (const auto& name : vKVisitor.getUsedRandomizedNames()) {
		std::cout << " - " << name << std::endl;
	}

#endif // DEBUG

	if (vKVisitor.getRandomizedNames().count(originalName) > 0) {
		return vKVisitor.getRandomizedNames().at(originalName);
	}
	return originalName;
}


