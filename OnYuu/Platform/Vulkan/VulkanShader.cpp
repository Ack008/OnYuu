#include "VulkanShader.h"
#include <stdexcept>
#include <fstream>
#include "Platform/Vulkan/VulkanRender.h"
#include "Render/Renderer.h"
#include "spirv_cross/spirv_cross.hpp"
#include "spirv_cross/spirv_glsl.hpp"
#include <iostream>
#include <shaderc/shaderc.hpp>
#ifdef _DEBUG
#define LOG(x) std::cout << x
#else
#define LOG(x)
#endif
namespace OnYuu {
	std::vector<char> compileToSPIRV_char(const std::string& source, shaderc_shader_kind kind) {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);

		auto result = compiler.CompileGlslToSpv(source, kind, "shader.glsl", options);

		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			std::cerr << "ERRORE DI COMPILAZIONE SHADERC:\n" << result.GetErrorMessage() << std::endl;
			throw std::runtime_error(result.GetErrorMessage());

		}

		// Reinterpreta i uint32_t come bytes
		size_t size_in_bytes = (result.cend() - result.cbegin()) * sizeof(uint32_t);
		std::vector<char> buffer(size_in_bytes);
		std::memcpy(buffer.data(), result.cbegin(), size_in_bytes);

		return buffer;
	}
	std::vector<char> VulkanShader::readFile(const char* filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	VkShaderModule VulkanShader::createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(((VulkanRender*)Render::getInstance().get())->getInit().device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
		return shaderModule;
	}

	void VulkanShader::printMappingInfo()
	{
		for (const auto& [name, offset] : uniformData)
		{
			LOG( "Uniform: " << name << " Offset: " << offset << "\n");
		}

	}

	VulkanShader::VulkanShader(const char* vertexfilename, const char* fragmentfilename, bool isSource)
	{
		std::vector<char> vertShaderCode;
		std::vector<char> fragShaderCode;
		// Caricamento del codice SPIR-V dai file
		if (isSource)
		{
			vertShaderCode = compileToSPIRV_char(vertexfilename, shaderc_vertex_shader);
			fragShaderCode = compileToSPIRV_char(fragmentfilename, shaderc_fragment_shader);
		}
		else {
			vertShaderCode = readFile(vertexfilename);
			fragShaderCode = readFile(fragmentfilename);

		}
		// Creazione dei moduli shader
		vertexShaderModule = createShaderModule(vertShaderCode);
		fragmentShaderModule = createShaderModule(fragShaderCode);

		// Converti il buffer char (byte) in vector<uint32_t> per SPIRV-Cross
		if (vertShaderCode.size() % 4 != 0) {
			throw std::runtime_error("SPIR-V binary size is not a multiple of 4");
		}
		std::vector<uint32_t> spirv_binary(vertShaderCode.size() / 4);
		std::memcpy(spirv_binary.data(), vertShaderCode.data(), vertShaderCode.size());

		// Costruisci il CompilerGLSL passando puntatore e dimensione per evitare problemi di ABI e heap di std::vector tra diverse versioni del Vulkan SDK
		spirv_cross::Compiler glsl(spirv_binary.data(), spirv_binary.size());
		spirv_cross::ShaderResources resources = glsl.get_shader_resources();
		uint32_t targetSet = 2;   // oppure 3
		size_t materialBufferSize = 0;
		for (const auto& ub : resources.uniform_buffers)
		{
			uint32_t set = glsl.get_decoration(ub.id, spv::DecorationDescriptorSet);
			uint32_t binding = glsl.get_decoration(ub.id, spv::DecorationBinding);
			LOG( "SPIR-V UBO: name=\"" << ub.name << "\" set=" << set << " binding=" << binding
				<< " type_id=" << ub.type_id << " base_type_id=" << ub.base_type_id << "\n");

			if (set == 1)
			{
				LOG( "UBO: name=\"" << ub.name << "\" type_id=" << ub.type_id << " base_type_id=" << ub.base_type_id << "\n");

				// usa base_type_id se presente: è il struct reale che contiene i nomi dei membri
				uint32_t inspect_id = ub.base_type_id ? ub.base_type_id : ub.type_id;
				auto type = glsl.get_type(inspect_id);
				LOG( "  member count = " << type.member_types.size() << "\n");
				size_t bufferSize = glsl.get_declared_struct_size(type);
				LOG( "Buffer size = " << bufferSize << " bytes\n");
				uniformBuffer.resize(bufferSize, 0);
				for (uint32_t member_index = 0; member_index < type.member_types.size(); ++member_index)
				{
					uint32_t member_type_id = type.member_types[member_index];
					auto member_type = glsl.get_type(member_type_id);
					std::string member_name = glsl.get_member_name(inspect_id, member_index); // <-- usa inspect_id
					size_t offset = glsl.type_struct_member_offset(type, member_index);
					LOG("  Membro " << member_index
						<< " nome: \"" << member_name << "\""
						<< " type_id=" << member_type_id
						<< " basetype=" << static_cast<int>(member_type.basetype)
						<< " vecsize=" << member_type.vecsize
						<< " columns=" << member_type.columns
						<< " array dims=" << member_type.array.size()
						<< " nested members=" << member_type.member_types.size()
						<< " offset=" << offset
						<< "\n");
					uniformData[member_name] = offset;
					// aggiorno la size dei material


					// se è una struct annidata (o array di struct) puoi ispezionare i suoi membri così:
					if (member_type.member_types.size() > 0)
					{
						auto nested_id = member_type_id;
						auto nested_type = glsl.get_type(nested_id);
						for (uint32_t n = 0; n < nested_type.member_types.size(); ++n)
						{
							LOG("    nested [" << n << "] name=\"" << glsl.get_member_name(nested_id, n) << "\"\n");
						}
					}
					// Calcola la dimensione del buffer materiale
					// (semplice esempio, non copre tutti i casi)

				}
			}
		}
		printMappingInfo();
		int frames_in_flight = ((VulkanRender*)(Render::getInstance().get()))->getSwapchain()->getImageCount();
		
		flushCostants();
	}
	void VulkanShader::shutdown()
	{
		if (initialized)
		{
			
			LOG( "Destroying VulkanShader\n");
			// Distruzione dei moduli shader
			if (vertexShaderModule != VK_NULL_HANDLE) {
				vkDestroyShaderModule(((VulkanRender*)Render::getInstance().get())->getInit().device, vertexShaderModule, nullptr);
			}
			if (fragmentShaderModule != VK_NULL_HANDLE) {
				vkDestroyShaderModule(((VulkanRender*)Render::getInstance().get())->getInit().device, fragmentShaderModule, nullptr);
			}
		}
		initialized = false;
	}
	VulkanShader::~VulkanShader()
	{


	}

	void VulkanShader::useShader()
	{
	}

	void VulkanShader::setUniformMat4(const char* name, const float* value)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, value, 16 * sizeof(float));
		}
		else {
			LOG( "Warning: uniform '" << name << "' not found in shader.\n");
		}

	}

	void VulkanShader::setUniformInt(const char* name, int value)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, &value, sizeof(int));
		}
		else {
			LOG( "Warning: uniform '" << name << "' not found in shader.\n");
		}

	}

	void VulkanShader::setUniformFloat(const char* name, float value)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, &value, sizeof(float));
		}
		else {
			LOG( "Warning: uniform '" << name << "' not found in shader.\n");
		}
	}

	void VulkanShader::setUniformVec2(const char* name, const float* value)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, value, 2 * sizeof(float));
		}
		else {
			LOG( "Warning: uniform '" << name << "' not found in shader.\n");
		}
	}

	void VulkanShader::setUniformVec3(const char* name, const float* value)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, value, 3 * sizeof(float));
		}
		else {
			LOG("Warning: uniform '" << name << "' not found in shader.\n");
		}
	}

	void VulkanShader::setUniformVec4(const char* name, const float* value)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, value, 4 * sizeof(float));
		}
		else {
			LOG("Warning: uniform '" << name << "' not found in shader.\n");
		}
	}

	void VulkanShader::setUniformMat3(const char* name, const float* value)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, value, 9 * sizeof(float));
		}
		else {
			LOG( "Warning: uniform '" << name << "' not found in shader.\n");
		}
	}

	void VulkanShader::setUniformMat4(const char* name, const float* value, int count)
	{
		if (uniformData.find(name) != uniformData.end())
		{
			size_t offset = uniformData[name];
			std::memcpy(uniformBuffer.data() + offset, value, count * 16 * sizeof(float));
		}
		else {
			LOG( "Warning: uniform '" << name << "' not found in shader.\n");
		}
	}

	void VulkanShader::flushCostants()
	{
		int index = ((VulkanRender*)(Render::getInstance().get()))->getCurrentFrame();
	}

	bool VulkanShader::isBatchingSupported() const
	{
		return false;
	}
}