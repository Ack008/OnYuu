#include "ChoosingShaderState.h"
#include "IdleState.h"
#include "Core/Engine.h"
#include "ImGuiFileDialog.h"
#include <filesystem>
#include "ChoosingMaterialNameState.h"
void ChoosingShaderState::onEnter()
{
	IGFD::FileDialogConfig config;
	config.path = Project::getInstance().getAssetsPath();
	ImGuiFileDialog::Instance()->OpenDialog("ChooseShader", "Select Shader", ".shader", config);	
}
void ChoosingShaderState::onExit()
{
}
void ChoosingShaderState::onImGuiRender()
{
	if (ImGuiFileDialog::Instance()->Display("ChooseShader"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// apro una finestra di dialogo per scegliere il nome del materiale e la posizione, precompilata con lo stesso nome dello shader e estensione .mat

			std::string shaderPath = ImGuiFileDialog::Instance()->GetFilePathName();
			std::filesystem::path shaderId = std::filesystem::relative(shaderPath, Project::getInstance().getAssetsPath());

			
			//MaterialFileWriter::createMaterial(materialPath.string(), shaderPath);
			m_stateMachine->changeState(new ChoosingMaterialNameState(m_stateMachine, shaderPath, shaderId.string()));
		}
		ImGuiFileDialog::Instance()->Close();
	}
}