
#include "MaterialEditingState.h"
#include "Core/Engine.h"
#include "imgui.h"
#include <string.h>
#include "ImGuiFileDialog.h"
#include "Application/AssetManager.h"
#include "../Helpers/MaterialFileWriter.h"
#include "../FileCreatorStates/IdleState.h"
#include "json/json.hpp"

#include <fstream>

using namespace OnYuu;

namespace {
	bool readFloatArrayValue(const nlohmann::json& value, std::vector<float>& out)
	{
		if (!value.is_array()) {
			return false;
		}
		out.clear();
		out.reserve(value.size());
		for (const auto& item : value) {
			if (!item.is_number()) {
				return false;
			}
			out.push_back(item.get<float>());
		}
		return true;
	}
}

void MaterialEditingState::onEnter()
{
	// Load material data from file and populate m_materialNameBuffer and parameter editors
	// Extract material name from ID or use ID as name if not available
	std::string name = m_materialId;

	// Copia id safely into buffer
	strncpy_s(m_materialNameBuffer, sizeof(m_materialNameBuffer), name.c_str(), _TRUNCATE);

	m_paramEditors.clear();
	
	if (!m_materialPath.empty() && std::filesystem::exists(m_materialPath)) {
		std::ifstream file(m_materialPath);
		if (file.is_open()) {
			nlohmann::json j;
			try {
				file >> j;
				
				if (j.contains("params") && j["params"].is_object()) {
					for (auto it = j["params"].begin(); it != j["params"].end(); ++it) {
						MaterialParamEditor editor;
						editor.name = it.key();
						editor.isTexture = false;
						
						const auto& paramJson = it.value();
						if (paramJson.is_object() && paramJson.contains("type") && paramJson.contains("value")) {
							std::string typeName = paramJson.value("type", "");
							const auto& value = paramJson["value"];
							editor.type = typeName;
							
							if (typeName == "Int" && value.is_number_integer()) {
								editor.intValue = value.get<int>();
							} else if (typeName == "Float" && value.is_number()) {
								editor.floatValue = value.get<float>();
							} else if (typeName == "Bool" && value.is_boolean()) {
								editor.boolValue = value.get<bool>();
							} else {
								std::vector<float> f;
								if (readFloatArrayValue(value, f)) {
									if (typeName == "Vec2" && f.size() == 2) {
										editor.vec2[0] = f[0]; editor.vec2[1] = f[1];
									} else if (typeName == "Vec3" && f.size() == 3) {
										editor.vec3[0] = f[0]; editor.vec3[1] = f[1]; editor.vec3[2] = f[2];
									} else if (typeName == "Vec4" && f.size() == 4) {
										editor.vec4[0] = f[0]; editor.vec4[1] = f[1]; editor.vec4[2] = f[2]; editor.vec4[3] = f[3];
									}
								}
							}
						}
						m_paramEditors.push_back(editor);
					}
				}
				
				if (j.contains("textures") && j["textures"].is_object()) {
					for (auto it = j["textures"].begin(); it != j["textures"].end(); ++it) {
						MaterialParamEditor editor;
						editor.name = it.key();
						editor.isTexture = true;
						if (it.value().is_string()) {
							editor.texturePath = it.value().get<std::string>();
							strncpy_s(editor.texturePathBuffer, sizeof(editor.texturePathBuffer), editor.texturePath.c_str(), _TRUNCATE);
						}
						m_paramEditors.push_back(editor);
					}
				}
			} catch (const std::exception& e) {
				std::cerr << "Error parsing material JSON: " << e.what() << std::endl;
			}
		}
	} else {
		std::cout << "Warning: Material file not found for " << m_materialId << std::endl;
	}
	std::cout << "Loaded material '" << m_materialId << "' with " << m_paramEditors.size() << " parameters." << std::endl;
}

void MaterialEditingState::onExit()
{
	if (m_materialPath.empty()) {
		std::cerr << "Error: Material path is empty for " << m_materialId << std::endl;
		return;
	}

	nlohmann::json root;

	// Try to load existing file to preserve other fields like shaderName, sourcePath, etc.
	std::ifstream ifs(m_materialPath);
	if (ifs.is_open()) {
		try {
			ifs >> root;
		} catch(const std::exception& e) {
			std::cerr << "Warning parsing material JSON on exit: " << e.what() << std::endl;
		}
		ifs.close();
	}

	nlohmann::json paramsObj;
	if (root.contains("params") && root["params"].is_object()) {
		paramsObj = root["params"];
	}

	nlohmann::json texObj;
	if (root.contains("textures") && root["textures"].is_object()) {
		texObj = root["textures"];
	}

	for (const auto& editor : m_paramEditors) {
		if (editor.isTexture) {
			texObj[editor.name] = editor.texturePath;
		} else {
			nlohmann::json p;
			p["type"] = editor.type;
			if (editor.type == "Int") p["value"] = editor.intValue;
			else if (editor.type == "Float") p["value"] = editor.floatValue;
			else if (editor.type == "Bool") p["value"] = editor.boolValue;
			else if (editor.type == "Vec2") p["value"] = { editor.vec2[0], editor.vec2[1] };
			else if (editor.type == "Vec3") p["value"] = { editor.vec3[0], editor.vec3[1], editor.vec3[2] };
			else if (editor.type == "Vec4") p["value"] = { editor.vec4[0], editor.vec4[1], editor.vec4[2], editor.vec4[3] };

			paramsObj[editor.name] = p;
		}
	}

	root["params"] = paramsObj;
	root["textures"] = texObj;

	std::ofstream ofs(m_materialPath);
	if (ofs.is_open()) {
		ofs << root.dump(4);
		ofs.flush();
		ofs.close();
	} else {
		std::cerr << "Error: Could not open file for writing: " << m_materialPath << std::endl;
	}
	auto mat = AssetManager::instance().getMaterialPtr(m_materialId);
	if (mat) {
	std::cout << "Updating material in AssetManager: " << m_materialId << " " << m_materialPath << std::endl;
		AssetManager::instance().importMaterialMetadataFromJson(m_materialPath.string(), m_materialId);
		AssetManager::instance().createMaterialFromMetadata(m_materialId);

	}
}

void MaterialEditingState::onImGuiRender()
{
	if(ImGui::Begin("Material Editor")) 
	{
		ImGui::InputText("Material Name", m_materialNameBuffer, sizeof(m_materialNameBuffer));
		ImGui::Separator();
		
		ImGui::Text("Material Properties");
		
		for (auto& editor : m_paramEditors) {
			ImGui::PushID(editor.name.c_str());
			if (editor.isTexture) {
				ImGui::Text("%s (Texture):", editor.name.c_str());
				ImGui::InputText("##Path", editor.texturePathBuffer, sizeof(editor.texturePathBuffer));
				ImGui::SameLine();
				if (ImGui::Button("Browse")) {
					IGFD::FileDialogConfig config;
					config.path = Project::getInstance().getAssetsPath(); 
					ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureDlgKey_" + editor.name, "Choose a Texture", ".png,.jpg,.jpeg", config);
				}
				
				if (ImGuiFileDialog::Instance()->Display("ChooseTextureDlgKey_" + editor.name)) {
					if (ImGuiFileDialog::Instance()->IsOk()) {
						std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
						strncpy_s(editor.texturePathBuffer, sizeof(editor.texturePathBuffer), filePathName.c_str(), _TRUNCATE);
						editor.texturePath = filePathName;
					}
					ImGuiFileDialog::Instance()->Close();
				}
				else {
					// Fallback to update path from string
					editor.texturePath = editor.texturePathBuffer;
				}
			} else {
				if (editor.type == "Int") {
					ImGui::InputInt(editor.name.c_str(), &editor.intValue);
				} else if (editor.type == "Float") {
					ImGui::InputFloat(editor.name.c_str(), &editor.floatValue);
				} else if (editor.type == "Bool") {
					ImGui::Checkbox(editor.name.c_str(), &editor.boolValue);
				} else if (editor.type == "Vec2") {
					ImGui::InputFloat2(editor.name.c_str(), editor.vec2);
				} else if (editor.type == "Vec3") {
					ImGui::ColorEdit3(editor.name.c_str(), editor.vec3);
				} else if (editor.type == "Vec4") {
					ImGui::ColorEdit4(editor.name.c_str(), editor.vec4);
				}
			}
			ImGui::PopID();
			
		}
		if (ImGui::Button("Save")) {
			m_stateMachine->changeState(new MaterialEditingState(m_stateMachine, m_materialPath, m_materialId));
		}
		if (ImGui::Button("Reset")) {
			onEnter(); // Reload from file
		}
		if (ImGui::Button("Chiudi")) {
			m_stateMachine->changeState(new IdleState(m_stateMachine)); // Go back to previous state
		}
	}
	ImGui::End();
}