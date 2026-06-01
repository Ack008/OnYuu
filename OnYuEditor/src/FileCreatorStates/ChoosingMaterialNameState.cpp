#include "ChoosingMaterialNameState.h"
#include "Core/Engine.h"
#include "IdleState.h"
#include <filesystem>
#include "../Helpers/MaterialFileWriter.h"
void ChoosingMaterialNameState::onEnter()
{
}
void ChoosingMaterialNameState::onExit()
{
	MaterialFileWriter::createMaterial(m_materialPath.string(), m_shaderPath.string(), m_materialId, m_shaderId);
}
void ChoosingMaterialNameState::onImGuiRender()
{
	ImGui::Begin("Create Material");
	ImGui::InputText("Material Name", m_materialNameBuffer, sizeof(m_materialNameBuffer));
	if (ImGui::Button("Create")) {
		std::string nameStr = std::string(m_materialNameBuffer);
		if (nameStr.empty()) {
			ImGui::OpenPopup("ErrorPopup");
		} else {
			m_materialPath = std::filesystem::path(m_shaderPath).parent_path() / (nameStr + ".mat");
			m_materialId = std::filesystem::relative(m_materialPath, Project::getInstance().getAssetsPath()).string();
			m_stateMachine->changeState(new IdleState(m_stateMachine));
		}
	}
	if (ImGui::BeginPopupModal("ErrorPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Failed to create material. Please ensure the name is valid and try again.");
		if (ImGui::Button("OK")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::End();
}
