#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include "Core/Engine.h"
namespace OnYuu {
	class MaterialFileWriter
	{
	public:
		static bool writeMaterialToFile(const std::string& path, const std::string& shaderPath, std::shared_ptr<MetaShader> shader);
		static bool createMaterial(const std::string& materialPath, const std::string& shaderPath, const std::string& materialId, const std::string& shaderId);
	};
} // namespace OnYuu