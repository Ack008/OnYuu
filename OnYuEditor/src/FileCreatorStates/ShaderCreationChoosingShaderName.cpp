#include "ShaderCreationChoosingShaderName.h"
#include "IdleState.h"
#include "Core/Engine.h"
void ShaderFileCreationState::onEnter()
{
	shouldClose = false;
	m_openPopupNextFrame = true;

	
}
void ShaderFileCreationState::onExit()
{
}
void ShaderFileCreationState::onImGuiRender()
{
	if (m_openPopupNextFrame) {
		ImGui::OpenPopup("shader_name");
		m_openPopupNextFrame = false;
	}

	ImGui::SetNextWindowSize(ImVec2(700.0f, 260.0f), ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("shader_name", NULL, ImGuiWindowFlags_NoResize)) {
		ImGui::InputText("Shader Name", shaderNameBuffer, sizeof(shaderNameBuffer));
		if (ImGui::Button("Close")) {
			shouldClose = true;
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Create")) {
			std::string shaderName(shaderNameBuffer);
			if (shaderName.empty()) {
				std::cerr << "Shader name cannot be empty.\n";
				m_openPopupNextFrame = true;
			}
			else {
				std::filesystem::path shaderPath = m_folderPath / (shaderName + ".shader");
				if (std::filesystem::exists(shaderPath)) {
					std::cerr << "A file with the name '" << shaderPath << "' already exists.\n";
					m_openPopupNextFrame = true;
				}
				else {
					std::ofstream shaderFile(shaderPath);
					if (shaderFile.is_open()) {
						shaderFile << "// New shader created with OnYuu Editor\n";
						shaderFile.close();
						shouldClose = true;
					}
					else {
						std::cerr << "Failed to create shader file: " << shaderPath << "\n";
						m_openPopupNextFrame = true;
					}
				}
			}
		}
		ImGui::EndPopup();
	}
	if (shouldClose) {
		m_stateMachine->changeState(new IdleState(m_stateMachine));
	}

}