#include "ShaderEditingState.h"
#include "Core/Engine.h"
#include "imgui.h"
#include <string.h>
#include "ImGuiFileDialog.h"
#include "Application/AssetManager.h"
#include "../Helpers/MaterialFileWriter.h"
#include "../MaterialCreationStateMachines/IdleState.h"
#include "ErrorWindowState.h"
#include "json/json.hpp"
#include <filesystem>
#include <fstream>
void ShaderEditingState::onEnter()
{
	errorWindowStateMachine.changeState(new IdleState(&errorWindowStateMachine));
	std::ifstream shaderFile(m_shaderPath);
	if (shaderFile.is_open()) {
		std::string shaderCode((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
		strncpy_s(m_shaderNameBuffer, sizeof(m_shaderNameBuffer), shaderCode.c_str(), sizeof(m_shaderNameBuffer) - 1);
		m_shaderNameBuffer[sizeof(m_shaderNameBuffer) - 1] = '\0';
		shaderFile.close();
	}
	else {
		std::cerr << "Failed to open shader file: " << m_shaderPath << std::endl;
	}
}

void ShaderEditingState::onExit()
{
	
}

void ShaderEditingState::onImGuiRender()
{
	ImGui::Begin("Shader Editor");
	ImGui::InputTextMultiline("##shaderCode", m_shaderNameBuffer, sizeof(m_shaderNameBuffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
	if (ImGui::Button("Save")) {
		char tempBuffer[64 * 1024];
		std::shared_ptr<MetaShader> tempMeta = MetaShader::create(m_shaderNameBuffer, true);
		if (!tempMeta || tempMeta->hasErrors()) {
			std::cerr << "Shader code has errors, not saving. Please fix the errors before saving.\n";
			errorWindowStateMachine.changeState(new ErrorWindowState(&errorWindowStateMachine, "Shader code has errors, not saving. Please fix the errors before saving."));
		}
		else {
			std::ofstream shaderFile(m_shaderPath);
			if (shaderFile.is_open()) {
				shaderFile << m_shaderNameBuffer;
				shaderFile.close();
				// Capture old shader pointer so renderer can invalidate pipelines bound to it
				auto oldMeta = AssetManager::instance().getShaderPtr(m_shaderId);
				auto oldShaderPtr = oldMeta ? oldMeta->getShader() : nullptr;

				// Try to compile the shader by reloading it in the AssetManager
				auto meta = AssetManager::instance().addShader(m_shaderId);
				if (!meta) {
					std::cerr << "Shader compilation failed: could not create MetaShader for '" << m_shaderPath << "'\n";
				}
				else if (meta->hasErrors()) {
					std::cerr << "Shader compilation reported errors for '" << m_shaderPath << "'\n";
				}

				else modifyMaterials(meta);
			}
			else {
				std::cerr << "Failed to open shader file for writing: " << m_shaderPath << std::endl;
			}
		}
		tempMeta->shutdown();
	}
	if (ImGui::Button("Close")) {
		m_stateMachine->changeState(new IdleState(m_stateMachine));
	}
	ImGui::End();
	errorWindowStateMachine.update(0.0f);
}

void ShaderEditingState::modifyMaterials(std::shared_ptr<OnYuu::MetaShader>& meta)
{
	{
		// Compilation OK: search all material files in the assets directory and rewrite those that depend on this shader
		std::string shaderName = m_shaderId;

		auto assetsRoot = Project::getInstance().getAssetsPath();
		try {
			for (auto& entry : std::filesystem::recursive_directory_iterator(assetsRoot)) {
				if (!entry.is_regular_file()) continue;
				auto ext = entry.path().extension().string();
				if (ext == ".mat" || ext == ".MAT") {
					// Open and inspect shaderName inside the material JSON
					std::ifstream in(entry.path());
					if (!in.is_open()) continue;
					nlohmann::json j;
					try { in >> j; }
					catch (...) { continue; }
					std::string matShader = j.value("shaderName", std::string{});
					std::string matSource = j.value("sourcePath", std::string{});
					bool matches = false;
					if (!matShader.empty() && matShader == shaderName) matches = true;
					if (!matches && !matSource.empty()) {
						std::filesystem::path sp(matSource);
						if (sp.string() == shaderName) matches = true;
					}
					if (matches) {
						std::cout << "Updating material '" << entry.path() << "' to reflect changes in shader '" << shaderName << "'\n";
						OnYuu::MaterialFileWriter::overwriteMaterialfromJSON(entry.path().string());
						std::string  materialId = std::filesystem::relative(entry.path(), assetsRoot).string();
						std::cout << "Reloading material '" << materialId << "' in AssetManager\n";
						auto mat = AssetManager::instance().getMaterialPtr(materialId);
						if (mat) {
							std::cout << "Updating material in AssetManager: " << materialId << " " << entry.path() << std::endl;
							AssetManager::instance().importMaterialMetadataFromJson(entry.path().string(), materialId);
							AssetManager::instance().createMaterialFromMetadata(materialId);
							mat->setShaderByID(m_shaderPath.string());
						}
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Error while scanning materials: " << e.what() << std::endl;
		}
		// reload in-memory materials
		AssetManager::instance().reloadMaterialsUsingShader(m_shaderId);

		// Update in-memory Material objects to use the new shader and notify renderer
		auto newShaderPtr = meta->getShader();
		if (newShaderPtr) {
			// Update material instances
			auto matNames = AssetManager::instance().getMaterialsUsingShader(m_shaderId);
			for (const auto& matName : matNames) {
				auto matPtr = AssetManager::instance().getMaterialPtr(matName);
				if (matPtr) {
					matPtr->setShader(newShaderPtr);
				}
			}

			// Ask renderer to invalidate pipelines/resources by shader name (more robust)
			auto renderer = Render::getInstance();
			if (renderer) {
				renderer->invalidateShaderByName(m_shaderId);
			}
		}
	}
}
