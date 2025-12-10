#pragma once
#include "Core/Shader.h"
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "Render/Buffer.h"
// VulkanShader: implementazione concreta di Shader per Vulkan. Gestisce la
// creazione, il binding e l'upload degli uniform per i programmi shader Vulkan.
class VulkanShader : public Shader
{
	public:
	VulkanShader(const char* vertexfilename, const char* fragmentfilename);
	virtual ~VulkanShader() override;
	virtual void useShader() override;
	virtual void setUniformMat4(const char* name, const float* value) override;
	virtual void setUniformInt(const char* name, int value) override;
	virtual void setUniformFloat(const char* name, float value) override;
	virtual void setUniformVec2(const char* name, const float* value) override;
	virtual void setUniformVec3(const char* name, const float* value) override;
	virtual void setUniformVec4(const char* name, const float* value) override;
	virtual void setUniformMat3(const char* name, const float* value) override;
	virtual void setUniformMat4(const char* name, const float* value, int count) override;
	virtual void flushCostants() override;
	virtual void shutdown() override;
	virtual bool isBatchingSupported() const override;
private:
	std::vector<char> readFile(const char* filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void printMappingInfo();
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	// Mappa per memorizzare gli uniform e i loro offset/posizione
	std::unordered_map<std::string, size_t> uniformData;
	std::vector<uint8_t> uniformBuffer;
	std::vector<std::shared_ptr<UniformBuffer>> materialBufferObject;
	bool initialized = true;
public:
	VkShaderModule getVertexShaderModule() const { return vertexShaderModule; }
	VkShaderModule getFragmentShaderModule() const { return fragmentShaderModule; }
	std::vector < std::shared_ptr<UniformBuffer>> getMaterialBufferObject() const { return materialBufferObject; }
	std::vector<uint8_t>& getUniformBuffer() { return uniformBuffer; }
};