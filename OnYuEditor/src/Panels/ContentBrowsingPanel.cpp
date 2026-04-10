#include "ContentBrowsingPanel.h"
#include <cstring>
#include <algorithm>
#include <cctype>
#include "json/json.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define Escape WinEscape
#include <windows.h>
#include <commdlg.h>
#undef Escape
#pragma comment(lib, "Comdlg32.lib")
#endif

namespace {
	using json = nlohmann::json;

	std::string toLowerCopy(std::string value)
	{
		std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return value;
	}

	std::string shaderTypeToMaterialType(const std::string& type)
	{
		const std::string t = toLowerCopy(type);
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

	bool isTextureLikeType(const std::string& type)
	{
		const std::string t = toLowerCopy(type);
		return t.find("sampler") != std::string::npos || t.find("texture") != std::string::npos || t.find("image") != std::string::npos;
	}

	bool readParamValue(const json& value, OnYuu::ContentBrowsingPanel::MaterialParamEditor& param)
	{
		const std::string type = shaderTypeToMaterialType(value.value("type", std::string{}));
		if (!value.contains("value")) return false;

		param.type = type;
		param.isTexture = false;

		const json& v = value["value"];
		if (type == "Int") {
			if (!v.is_number_integer()) return false;
			param.intValue = v.get<int>();
			return true;
		}
		if (type == "Float") {
			if (!v.is_number()) return false;
			param.floatValue = v.get<float>();
			return true;
		}
		if (type == "Bool") {
			if (!v.is_boolean()) return false;
			param.boolValue = v.get<bool>();
			return true;
		}
		if (type == "Vec2") {
			if (!v.is_array() || v.size() != 2) return false;
			param.vec2[0] = v[0].get<float>();
			param.vec2[1] = v[1].get<float>();
			return true;
		}
		if (type == "Vec3") {
			if (!v.is_array() || v.size() != 3) return false;
			for (size_t i = 0; i < 3; ++i) param.vec3[i] = v[i].get<float>();
			return true;
		}
		if (type == "Vec4") {
			if (!v.is_array() || v.size() != 4) return false;
			for (size_t i = 0; i < 4; ++i) param.vec4[i] = v[i].get<float>();
			return true;
		}
		if (type == "Mat3") {
			if (!v.is_array() || v.size() != 3) return false;
			for (size_t r = 0; r < 3; ++r) {
				if (!v[r].is_array() || v[r].size() != 3) return false;
				for (size_t c = 0; c < 3; ++c) param.mat3[r * 3 + c] = v[r][c].get<float>();
			}
			return true;
		}
		if (type == "Mat4") {
			if (!v.is_array() || v.size() != 4) return false;
			for (size_t r = 0; r < 4; ++r) {
				if (!v[r].is_array() || v[r].size() != 4) return false;
				for (size_t c = 0; c < 4; ++c) param.mat4[r * 4 + c] = v[r][c].get<float>();
			}
			return true;
		}
		return false;
	}

#ifdef _WIN32
	std::filesystem::path pickPngFile(const std::filesystem::path& initialDir)
	{
		char fileBuffer[MAX_PATH] = {};
		OPENFILENAMEA ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = fileBuffer;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = "PNG Images\0*.png\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

		std::string initialDirStr = initialDir.empty() ? std::string{} : initialDir.string();
		if (!initialDirStr.empty()) {
			ofn.lpstrInitialDir = initialDirStr.c_str();
		}

		if (GetOpenFileNameA(&ofn)) {
			return std::filesystem::path(fileBuffer);
		}
		return {};
	}

	std::filesystem::path pickShaderFile(const std::filesystem::path& initialDir)
	{
		char fileBuffer[MAX_PATH] = {};
		OPENFILENAMEA ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = fileBuffer;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = "Shader Files\0*.shader\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

		std::string initialDirStr = initialDir.empty() ? std::string{} : initialDir.string();
		if (!initialDirStr.empty()) {
			ofn.lpstrInitialDir = initialDirStr.c_str();
		}

		if (GetOpenFileNameA(&ofn)) {
			return std::filesystem::path(fileBuffer);
		}
		return {};
	}
#else
	std::filesystem::path pickPngFile(const std::filesystem::path&)
	{
		return {};
	}

	std::filesystem::path pickShaderFile(const std::filesystem::path&)
	{
		return {};
	}
#endif
}

namespace OnYuu {

	std::filesystem::path ContentBrowsingPanel::openPngFilePicker(const std::filesystem::path& initialDir)
	{
		return pickPngFile(initialDir);
	}

	std::filesystem::path ContentBrowsingPanel::openShaderFilePicker(const std::filesystem::path& initialDir)
	{
		return pickShaderFile(initialDir);
	}

	std::string ContentBrowsingPanel::normalizeMaterialType(std::string type)
	{
		const std::string lower = toLowerCopy(type);
		if (lower == "int" || lower == "ivec2" || lower == "ivec3" || lower == "ivec4" || lower == "uint") return "Int";
		if (lower == "float" || lower == "double" ) return "Float";
		if (lower == "bool") return "Bool";
		if (lower == "vec2" || lower == "dvec2" || lower == "uvec2" || lower == "ivec2") return "Vec2";
		if (lower == "vec3" || lower == "dvec3" || lower == "uvec3" || lower == "ivec3") return "Vec3";
		if (lower == "vec4" || lower == "dvec4" || lower == "uvec4" || lower == "ivec4") return "Vec4";
		if (lower == "mat3" || lower == "dmat3") return "Mat3";
		if (lower == "mat4" || lower == "dmat4") return "Mat4";
		return type;
	}

	bool ContentBrowsingPanel::isTextureLikeType(const std::string& type)
	{
		return ::isTextureLikeType(type);
	}

	ContentBrowsingPanel::ContentBrowsingPanel()
	{
		m_currentDirectory = Project::getInstance().getAssetsPath();
		m_folderIcon = Texture::createTexture("resources/icons/ContentBrowser/DirectoryIcon.png");
		m_fileIcon = Texture::createTexture("resources/icons/ContentBrowser/FileIcon.png");
		m_folderIconWrapper = ImGuiTextureWrapper::create(m_folderIcon);
		m_fileIconWrapper = ImGuiTextureWrapper::create(m_fileIcon);
	}

	ContentBrowsingPanel::~ContentBrowsingPanel()
	{
		m_fileIcon->shutdown();
		m_folderIcon->shutdown();
	}

	void ContentBrowsingPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		if (m_currentDirectory != std::filesystem::path(Project::getInstance().getAssetsPath()))
		{
			if (ImGui::Button("<-")) {
				m_currentDirectory = m_currentDirectory.parent_path();
			}
		}

		contentBrowserContextMenu();
		createFolderPopup();
		createShaderPopup();

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);
		for (auto& entry : std::filesystem::directory_iterator(m_currentDirectory))
		{
			std::filesystem::path path = entry.path();
			auto relativePath = std::filesystem::relative(path, Project::getInstance().getAssetsPath());

			std::shared_ptr<ImGuiTextureWrapper> icon = entry.is_directory() ? m_folderIconWrapper : m_fileIconWrapper;

			ImGui::PushID(path.string().c_str());
			ImGui::ImageButton("##icon", icon->getTextureID(), { thumbnailSize, thumbnailSize });
			openItemContextMenu(path, entry.is_directory());

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (entry.is_directory()) {
					m_currentDirectory /= path.filename();
				}
				else if (path.extension() == ".mat") {
					if (loadMaterialEditor(path)) {
						m_openEditMaterialPopup = true;
					}
				}
			}

			ImGui::TextWrapped("%s", relativePath.string().c_str());
			ImGui::PopID();
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		if (m_openRenamePopup) {
			ImGui::OpenPopup("RenamePopup");
			m_openRenamePopup = false;
		}

		openRenamePopup();
		createMaterialPopup();
		openMaterialPopup();

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16.0f, 256.0f);
		ImGui::SliderFloat("Padding", &padding, 0.0f, 32.0f);
		ImGui::End();
	}

	void ContentBrowsingPanel::openRenamePopup()
	{
		if (ImGui::BeginPopupModal("RenamePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("New name", m_renameBuffer, sizeof(m_renameBuffer));

			if (ImGui::Button("OK"))
			{
				if (m_contextItemPath.has_parent_path() && std::strlen(m_renameBuffer) > 0) {
					std::filesystem::path newPath = m_contextItemPath.parent_path() / m_renameBuffer;
					std::error_code ec;
					std::filesystem::rename(m_contextItemPath, newPath, ec);
					if (!ec) {
						if (m_currentDirectory == m_contextItemPath) {
							m_currentDirectory = newPath;
						}
						m_contextItemPath = newPath;
					}
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void ContentBrowsingPanel::createShaderPopup()
	{
		if (m_openCreateShaderPopup) {
			ImGui::OpenPopup("CreateShaderPopup");
			m_openCreateShaderPopup = false;
		}
		if (ImGui::BeginPopupModal("CreateShaderPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Shader Name", m_createShaderBuffer, sizeof(m_createShaderBuffer));
			if (ImGui::Button("Create"))
			{
				std::filesystem::path shaderPath = m_currentDirectory / (std::string(m_createShaderBuffer) + ".shader");
				std::ofstream shaderFile(shaderPath);
				if (shaderFile.is_open()) {
					shaderFile << "// New shader created by OnYuu Editor\n";
					shaderFile.close();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	bool ContentBrowsingPanel::loadMaterialEditor(const std::filesystem::path& materialPath)
	{
		std::ifstream in(materialPath);
		if (!in.is_open()) {
			return false;
		}

		json j;
		try {
			in >> j;
		}
		catch (...) {
			return false;
		}

		m_materialEditor = {};
		m_materialEditor.materialPath = materialPath;
		m_materialEditor.shaderPath = j.value("sourcePath", std::string{});
		if (m_materialEditor.shaderPath.empty()) {
			m_materialEditor.shaderPath = j.value("shaderPath", std::string{});
		}

		if (j.contains("params") && j["params"].is_object()) {
			for (auto it = j["params"].begin(); it != j["params"].end(); ++it) {
				MaterialParamEditor param;
				param.name = it.key();
				if (it.value().is_object()) {
					if (!readParamValue(it.value(), param)) {
						continue;
					}
				}
				m_materialEditor.params.push_back(std::move(param));
			}
		}

		if (j.contains("textures") && j["textures"].is_object()) {
			for (auto it = j["textures"].begin(); it != j["textures"].end(); ++it) {
				MaterialParamEditor param;
				param.name = it.key();
				param.type = "Texture";
				param.isTexture = true;
				if (it.value().is_string()) {
					param.texturePath = it.value().get<std::string>();
					strncpy_s(param.texturePathBuffer, param.texturePath.c_str(), _TRUNCATE);
				}
				m_materialEditor.params.push_back(std::move(param));
			}
		}

		if (!m_materialEditor.shaderPath.empty()) {
			strncpy_s(m_editMaterialShaderBuffer, m_materialEditor.shaderPath.c_str(), _TRUNCATE);
		}
		else {
			m_editMaterialShaderBuffer[0] = '\0';
		}

		AssetManager::instance().importMaterialMetadataFromJson(materialPath.string(), materialPath.stem().string());
		m_materialToEditPath = materialPath;
		m_materialEditorLoaded = true;
		return true;
	}

	void ContentBrowsingPanel::rebuildMaterialParamsFromShader()
	{
		std::shared_ptr<MetaShader> shader = AssetManager::instance().getShaderPtr(m_materialEditor.shaderPath);
		if (!shader && !m_materialEditor.shaderPath.empty()) {
			AssetManager::instance().addShader(m_materialEditor.shaderPath);
			shader = AssetManager::instance().getShaderPtr(m_materialEditor.shaderPath);
		}
		if (!shader) {
			return;
		}

		std::unordered_map<std::string, MaterialParamEditor> previous;
		for (const auto& param : m_materialEditor.params) {
			previous[param.name] = param;
		}

		std::vector<MaterialParamEditor> rebuilt;
		for (const auto& [uniformName, uniformType] : shader->getUniformNameTypeMap()) {
			MaterialParamEditor param;
			param.name = uniformName;
			param.type = normalizeMaterialType(uniformType);
			param.isTexture = isTextureLikeType(uniformType);

			auto it = previous.find(uniformName);
			if (it != previous.end() && it->second.type == param.type && it->second.isTexture == param.isTexture) {
				param = it->second;
				param.name = uniformName;
				param.type = normalizeMaterialType(uniformType);
				param.isTexture = isTextureLikeType(uniformType);
			}
			else if (param.isTexture) {
				param.texturePath.clear();
				param.texturePathBuffer[0] = '\0';
			}

			rebuilt.push_back(std::move(param));
		}

		m_materialEditor.params = std::move(rebuilt);
	}

	bool ContentBrowsingPanel::saveMaterialEditor()
	{
		if (m_materialEditor.materialPath.empty()) {
			return false;
		}

		json materialJson;
		materialJson["shaderName"] = std::filesystem::path(m_materialEditor.shaderPath).stem().string();
		materialJson["sourcePath"] = m_materialEditor.shaderPath;
		materialJson["version"] = 1u;
		materialJson["params"] = json::object();
		materialJson["textures"] = json::object();

		for (const auto& param : m_materialEditor.params) {
			if (param.isTexture || isTextureLikeType(param.type)) {
				materialJson["textures"][param.name] = param.texturePath.empty() ? std::string(param.texturePathBuffer) : param.texturePath;
				continue;
			}

			if (param.type == "Int") {
				materialJson["params"][param.name] = { {"type", "Int"}, {"value", param.intValue} };
			}
			else if (param.type == "Float") {
				materialJson["params"][param.name] = { {"type", "Float"}, {"value", param.floatValue} };
			}
			else if (param.type == "Bool") {
				materialJson["params"][param.name] = { {"type", "Bool"}, {"value", param.boolValue} };
			}
			else if (param.type == "Vec2") {
				materialJson["params"][param.name] = { {"type", "Vec2"}, {"value", { param.vec2[0], param.vec2[1] }} };
			}
			else if (param.type == "Vec3") {
				materialJson["params"][param.name] = { {"type", "Vec3"}, {"value", { param.vec3[0], param.vec3[1], param.vec3[2] }} };
			}
			else if (param.type == "Vec4") {
				materialJson["params"][param.name] = { {"type", "Vec4"}, {"value", { param.vec4[0], param.vec4[1], param.vec4[2], param.vec4[3] }} };
			}
			else if (param.type == "Mat3") {
				materialJson["params"][param.name] = {
					{ "type", "Mat3" },
					{ "value", {
						{ param.mat3[0], param.mat3[1], param.mat3[2] },
						{ param.mat3[3], param.mat3[4], param.mat3[5] },
						{ param.mat3[6], param.mat3[7], param.mat3[8] }
					} }
				};
			}
			else if (param.type == "Mat4") {
				materialJson["params"][param.name] = {
					{ "type", "Mat4" },
					{ "value", {
						{ param.mat4[0], param.mat4[1], param.mat4[2], param.mat4[3] },
						{ param.mat4[4], param.mat4[5], param.mat4[6], param.mat4[7] },
						{ param.mat4[8], param.mat4[9], param.mat4[10], param.mat4[11] },
						{ param.mat4[12], param.mat4[13], param.mat4[14], param.mat4[15] }
					} }
				};
			}
		}

		std::ofstream out(m_materialEditor.materialPath);
		if (!out.is_open()) {
			return false;
		}

		out << materialJson.dump(4);
		out.close();
		AssetManager::instance().importMaterialMetadataFromJson(m_materialEditor.materialPath.string(), m_materialEditor.materialPath.stem().string());
		return true;
	}

	void ContentBrowsingPanel::drawMaterialParamEditor(MaterialParamEditor& param)
	{
		ImGui::PushID(param.name.c_str());
		ImGui::TextUnformatted(param.name.c_str());
		ImGui::SameLine(200.0f);

		if (param.isTexture || isTextureLikeType(param.type)) {
			if (param.texturePathBuffer[0] == '\0' && !param.texturePath.empty()) {
				strncpy_s(param.texturePathBuffer, param.texturePath.c_str(), _TRUNCATE);
			}
			const float buttonWidth = ImGui::CalcTextSize("...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
			float inputWidth = ImGui::GetContentRegionAvail().x - buttonWidth - ImGui::GetStyle().ItemInnerSpacing.x;
			if (inputWidth < 100.0f) inputWidth = 100.0f;
			ImGui::SetNextItemWidth(inputWidth);
			ImGui::InputText("##texture", param.texturePathBuffer, sizeof(param.texturePathBuffer), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("...")) {
				std::filesystem::path picked = pickPngFile(param.texturePath.empty() ? m_currentDirectory : std::filesystem::path(param.texturePath).parent_path());
				if (!picked.empty()) {
					param.texturePath = picked.string();
					strncpy_s(param.texturePathBuffer, param.texturePath.c_str(), _TRUNCATE);
				}
			}
		}
		else if (param.type == "Int") {
			ImGui::SetNextItemWidth(220.0f);
			ImGui::InputInt("##int", &param.intValue);
		}
		else if (param.type == "Float") {
			ImGui::SetNextItemWidth(220.0f);
			ImGui::InputFloat("##float", &param.floatValue);
		}
		else if (param.type == "Bool") {
			ImGui::Checkbox("##bool", &param.boolValue);
		}
		else if (param.type == "Vec2") {
			ImGui::SetNextItemWidth(220.0f);
			ImGui::InputFloat2("##vec2", param.vec2);
		}
		else if (param.type == "Vec3") {
			ImGui::SetNextItemWidth(500);
			ImGui::InputFloat3("##vec3", param.vec3);
		}
		else if (param.type == "Vec4") {
			ImGui::SetNextItemWidth(600);
			ImGui::InputFloat4("##vec4", param.vec4);
		}
		else if (param.type == "Mat3") {
			for (int row = 0; row < 3; ++row) {
				char label[16];
				std::snprintf(label, sizeof(label), "r%d", row);
				ImGui::SetNextItemWidth(220.0f);
				ImGui::InputFloat3(label, &param.mat3[row * 3]);
			}
		}
		else if (param.type == "Mat4") {
			for (int row = 0; row < 4; ++row) {
				char label[16];
				std::snprintf(label, sizeof(label), "r%d", row);
				ImGui::SetNextItemWidth(220.0f);
				ImGui::InputFloat4(label, &param.mat4[row * 4]);
			}
		}
		else {
			ImGui::TextUnformatted(param.type.c_str());
		}

		ImGui::PopID();
	}

	void ContentBrowsingPanel::openMaterialPopup()
	{
		if (m_openEditMaterialPopup) {
			ImGui::OpenPopup("EditMaterialPopup");
			m_openEditMaterialPopup = false;
		}

		ImGui::SetNextWindowSize(ImVec2(720.0f, 480.0f), ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal("EditMaterialPopup", nullptr))
		{
			ImGui::Text("Material: %s", m_materialEditor.materialPath.string().c_str());
			const float buttonWidth = ImGui::CalcTextSize("...").x + ImGui::GetStyle().FramePadding.x ;
			float shaderInputWidth = ImGui::GetContentRegionAvail().x - buttonWidth - ImGui::GetStyle().ItemInnerSpacing.x - 150.0f;
			if (shaderInputWidth < 90.0f) shaderInputWidth = 90;
			ImGui::SetNextItemWidth(shaderInputWidth);
			ImGui::InputText("Shader Path", m_editMaterialShaderBuffer, sizeof(m_editMaterialShaderBuffer), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("...")) {
				std::filesystem::path picked = openShaderFilePicker(m_materialEditor.shaderPath.empty() ? m_currentDirectory : std::filesystem::path(m_materialEditor.shaderPath).parent_path());
				if (!picked.empty()) {
					m_materialEditor.shaderPath = picked.string();
					strncpy_s(m_editMaterialShaderBuffer, m_materialEditor.shaderPath.c_str(), _TRUNCATE);
				}
			}
			if (ImGui::Button("Apply Shader")) {
				m_materialEditor.shaderPath = m_editMaterialShaderBuffer;
				for (auto& param : m_materialEditor.params) {
					if (param.isTexture || isTextureLikeType(param.type)) {
						param.texturePath = param.texturePathBuffer;
					}
				}
				rebuildMaterialParamsFromShader();
			}
			ImGui::SameLine();
			if (ImGui::Button("Reload from Shader")) {
				rebuildMaterialParamsFromShader();
			}

			ImGui::Separator();

			ImGui::BeginChild("MaterialParams", ImVec2(ImGui::GetContentRegionAvail().x, 280), true);
			for (auto& param : m_materialEditor.params) {
				drawMaterialParamEditor(param);
			}
			ImGui::EndChild();

			if (ImGui::Button("Save")) {
				m_materialEditor.shaderPath = m_editMaterialShaderBuffer;
				for (auto& param : m_materialEditor.params) {
					if (param.isTexture || isTextureLikeType(param.type)) {
						param.texturePath = param.texturePathBuffer;
					}
				}
				saveMaterialEditor();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void ContentBrowsingPanel::createMaterialPopup()
	{
		if(m_openCreateMaterialPopup) {

			ImGui::OpenPopup("CreateMaterialPopup");
			m_openCreateMaterialPopup = false;
		}

		if (ImGui::BeginPopupModal("CreateMaterialPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Material Name", m_createMaterialBuffer, sizeof(m_createMaterialBuffer));
			if (ImGui::Button("Create"))
			{
				if (AssetManager::instance().getShaderPtr(m_createMaterialShaderPath) == nullptr) {
					AssetManager::instance().addShader(m_createMaterialShaderPath);
					std::cout << "Created MetaShader for " << m_createMaterialShaderPath << std::endl;
					auto metaShader = AssetManager::instance().getShaderPtr(m_createMaterialShaderPath);
				}
				std::filesystem::path materialPath = m_currentDirectory / (std::string(m_createMaterialBuffer) + ".mat");
				m_materialFileWriter.writeMaterialToFile(materialPath.string(), m_createMaterialShaderPath, AssetManager::instance().getShaderPtr(m_createMaterialShaderPath));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void ContentBrowsingPanel::openItemContextMenu(const std::filesystem::path& itemPath, bool isDirectory)
	{
		ImGui::OpenPopupOnItemClick("item_ctx", ImGuiPopupFlags_MouseButtonRight);
		if (ImGui::BeginPopup("item_ctx"))
		{
			m_contextItemPath = itemPath;
			m_contextItemIsDirectory = isDirectory;
			std::memset(m_renameBuffer, 0, sizeof(m_renameBuffer));
			std::string name = itemPath.filename().string();
			strncpy_s(m_renameBuffer, name.c_str(), _TRUNCATE);

			if (ImGui::MenuItem("Open"))
			{
				if (isDirectory) {
					m_currentDirectory = itemPath;
				}
				else {
					std::string extension = itemPath.extension().string();
					if (extension == ".mat") {
						if (loadMaterialEditor(itemPath)) {
							m_openEditMaterialPopup = true;
						}
					}
				}
			}
			if (ImGui::MenuItem("Delete"))
			{
				std::error_code ec;
				std::filesystem::remove_all(itemPath, ec);
				if (!ec && m_currentDirectory == itemPath) {
					m_currentDirectory = itemPath.parent_path();
				}
			}
			if (ImGui::MenuItem("Rename"))
			{
				m_openRenamePopup = true;
			}

			std::string extension = itemPath.extension().string();
			if (extension == ".shader") {
				if (ImGui::MenuItem("Create Material"))
				{
					m_openCreateMaterialPopup = true;
					std::memset(m_createMaterialBuffer, 0, sizeof(m_createMaterialBuffer));
					strncpy_s(m_createMaterialBuffer, "New Material", _TRUNCATE);
					m_createMaterialShaderPath = itemPath.string();
				}
			}

			ImGui::EndPopup();
		}
	}

	void ContentBrowsingPanel::contentBrowserContextMenu()
	{
		if (ImGui::BeginPopupContextWindow("browser_ctx", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Folder"))
			{
				m_openCreateFolderPopup = true;
				std::memset(m_createFolderBuffer, 0, sizeof(m_createFolderBuffer));
				strncpy_s(m_createFolderBuffer, "New Folder", _TRUNCATE);
			}
			if (ImGui::MenuItem("Create shader"))
			{
				m_openCreateShaderPopup = true;
				std::memset(m_createShaderBuffer, 0, sizeof(m_createShaderBuffer));
				strncpy_s(m_createShaderBuffer, "New Shader", _TRUNCATE);

			}
			if (ImGui::MenuItem("Refresh"))
			{
				// No specific action needed as the directory will be re-read on the next render
			}
			ImGui::EndPopup();
		}
	}

	void ContentBrowsingPanel::createFolderPopup()
	{
		if (m_openCreateFolderPopup) {
			ImGui::OpenPopup("CreateFolderPopup");
			m_openCreateFolderPopup = false;
		}

		if (ImGui::BeginPopupModal("CreateFolderPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Folder Name", m_createFolderBuffer, sizeof(m_createFolderBuffer));
			if (ImGui::Button("Create"))
			{
				std::filesystem::path folderPath = m_currentDirectory / m_createFolderBuffer;
				std::error_code ec;
				std::filesystem::create_directory(folderPath, ec);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}