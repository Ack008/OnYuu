#include "ErrorWindowState.h"
#include "imgui.h"
#include "../MaterialCreationStateMachines/IdleState.h"

void ErrorWindowState::onEnter()
{
	m_openPopupNextFrame = true;
}

void ErrorWindowState::onExit()
{
}

void ErrorWindowState::onImGuiRender()
{
	if (m_openPopupNextFrame) {
		ImGui::OpenPopup("Error");
		m_openPopupNextFrame = false;
	}

	ImGui::SetNextWindowSize(ImVec2(700.0f, 260.0f), ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_NoResize)) {
		ImGui::TextWrapped("%s", m_errorMessage.c_str());
		if (ImGui::Button("Close")) {
			shouldClose = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (shouldClose) {
		m_stateMachine->changeState(new IdleState(m_stateMachine));
	}
}