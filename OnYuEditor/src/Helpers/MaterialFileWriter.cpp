#include "MaterialFileWriter.h"
#include "json/json.hpp"
#include <algorithm>
#include <cctype>

namespace {
	nlohmann::json defaultValueForType(const std::string& type)
	{
		const std::string t = [&]() {
			std::string s = type;
			std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}();

		if (t == "int" || t == "ivec2" || t == "ivec3" || t == "ivec4" || t == "uint") return 0;
		if (t == "float" || t == "double") return 0.0f;
		if (t == "bool") return false;
		if (t == "vec2" || t == "dvec2" || t == "ivec2" || t == "uvec2") return { 0.0f, 0.0f };
		if (t == "vec3" || t == "dvec3" || t == "ivec3" || t == "uvec3") return { 0.0f, 0.0f, 0.0f };
		if (t == "vec4" || t == "dvec4" || t == "ivec4" || t == "uvec4") return { 0.0f, 0.0f, 0.0f, 0.0f };
		if (t == "mat3" || t == "dmat3") return { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
		if (t == "mat4" || t == "dmat4") return { {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} };
		return nullptr;
	}

	bool isTextureLikeType(const std::string& type)
	{
		std::string t = type;
		std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return t.find("sampler") != std::string::npos ||
			t.find("texture") != std::string::npos ||
			t.find("image") != std::string::npos;
	}

	std::string toMaterialTypeName(const std::string& type)
	{
		std::string t = type;
		std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		if (t == "int" || t == "ivec2" || t == "ivec3" || t == "ivec4" || t == "uint") return "Int";
		if (t == "float" || t == "double") return "Float";
		if (t == "bool") return "Bool";
		if (t == "vec2" || t == "dvec2" || t == "ivec2" || t == "uvec2") return "Vec2";
		if (t == "vec3" || t == "dvec3" || t == "ivec3" || t == "uvec3") return "Vec3";
		if (t == "vec4" || t == "dvec4" || t == "ivec4" || t == "uvec4") return "Vec4";
		if (t == "mat3" || t == "dmat3") return "Mat3";
		if (t == "mat4" || t == "dmat4") return "Mat4";
		return type;
	}
}

bool OnYuu::MaterialFileWriter::writeMaterialToFile(const std::string& path, const std::string& shaderPath, std::shared_ptr<MetaShader> shader)
{
	nlohmann::json materialJson;
	const std::string shaderName = std::filesystem::path(shaderPath).stem().string();

	materialJson["shaderName"] = shaderName;
	materialJson["sourcePath"] = shaderPath;
	materialJson["version"] = 1u;
	materialJson["params"] = nlohmann::json::object();
	materialJson["textures"] = nlohmann::json::object();

	if (shader) {
		for (const auto& [uniformName, uniformType] : shader->getUniformNameTypeMap()) {
			if (isTextureLikeType(uniformType)) {
				materialJson["textures"][uniformName] = "";
				continue;
			}

			auto defaultValue = defaultValueForType(uniformType);
			if (!defaultValue.is_null()) {
				materialJson["params"][uniformName] = {
					{ "type", toMaterialTypeName(uniformType) },
					{ "value", defaultValue }
				};
			}
		}
	}

	std::ofstream materialFile(path);
	if (!materialFile.is_open()) {
		return false;
	}

	materialFile << materialJson.dump(4);
	return static_cast<bool>(materialFile);
}

bool OnYuu::MaterialFileWriter::createMaterial(const std::string& materialPath, const std::string& shaderPath, const std::string& materialId, const std::string& shaderId)
{

	nlohmann::json materialJson;

	materialJson["shaderName"] = shaderId;
	materialJson["sourcePath"] = shaderPath;
	materialJson["version"] = 1u;
	materialJson["params"] = nlohmann::json::object();
	materialJson["textures"] = nlohmann::json::object();
	auto shader = AssetManager::instance().getShaderPtr(shaderId);
	if (!shader) {
		// Try loading from shaderPath
		shader = AssetManager::instance().addShader(shaderPath);
		
	}
	if (shader) {
		for (const auto& [uniformName, uniformType] : shader->getUniformNameTypeMap()) {
			if (isTextureLikeType(uniformType)) {
				materialJson["textures"][uniformName] = "";
				continue;
			}

			auto defaultValue = defaultValueForType(uniformType);
			if (!defaultValue.is_null()) {
				materialJson["params"][uniformName] = {
					{ "type", toMaterialTypeName(uniformType) },
					{ "value", defaultValue }
				};
			}
		}
	}
	else {
		std::cout << "Failed to find shader with ID '" << shaderId << "'. Creating material with empty params and textures.\n";
	}

	std::ofstream materialFile(materialPath);
	if (!materialFile.is_open()) {
		return false;
	}

	materialFile << materialJson.dump(4);
	return static_cast<bool>(materialFile);
}
