#include "ContentBrowsingPanel.h"
#include <cstring>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include "json/json.hpp"
#include "ImGuiFileDialog.h"
#include "../FileCreatorStates/ChoosingShaderState.h"
#include "../FileCreatorStates/ChoosingMaterialNameState.h"
#include "../FileCreatorStates/IdleState.h"
#include "../ShaderEditorState/ShaderEditingState.h"
#include "../FileCreatorStates/ShaderCreationChoosingShaderName.h"


namespace OnYuu {
	void ContentBrowsingPanel::openContextualMenu()
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Material")) {
				fileCreatorMachine.changeState(new ChoosingShaderState(&fileCreatorMachine));
			}
			if (ImGui::MenuItem("Create Shader")) {
				fileCreatorMachine.changeState(new ShaderFileCreationState(&fileCreatorMachine, m_currentDirectory));
			}
			ImGui::EndPopup();
		}
		
	}

	ContentBrowsingPanel::ContentBrowsingPanel()
	{
		m_currentDirectory = Project::getInstance().getAssetsPath();
		m_folderIcon = Texture::createTexture("resources/icons/ContentBrowser/DirectoryIcon.png");
		m_fileIcon = Texture::createTexture("resources/icons/ContentBrowser/FileIcon.png");
		m_folderIconWrapper = ImGuiTextureWrapper::create(m_folderIcon);
		m_fileIconWrapper = ImGuiTextureWrapper::create(m_fileIcon);
		fileCreatorMachine.changeState(new IdleState(&fileCreatorMachine));
		materialEditorStateMachine.changeState(new IdleState(&materialEditorStateMachine));
		shaderEditorStateMachine.changeState(new IdleState(&shaderEditorStateMachine));
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
		openContextualMenu();


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

			if (!entry.is_directory() && path.extension() == ".mat") {
				const std::string materialId = relativePath.string();
				// rimpiazza \\ con / per uniformità (e perché ImGui drag-and-drop sembra trattare i percorsi come stringhe normali, non come path)
				std::string normalizedMaterialId = materialId;
				std::replace(normalizedMaterialId.begin(), normalizedMaterialId.end(), '\\', '/');
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					ImGui::SetDragDropPayload("ASSET_MATERIAL", normalizedMaterialId.c_str(), normalizedMaterialId.size() + 1);
					ImGui::Text("Material: %s", normalizedMaterialId.c_str());
					ImGui::EndDragDropSource();
				}
			}


			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (entry.is_directory()) {
					m_currentDirectory /= path.filename();
				}
				else if (path.extension() == ".mat") {
					materialEditorStateMachine.changeState(new ChoosingMaterialNameState(&materialEditorStateMachine, path, relativePath.string()));
				}
				else if (path.extension() == ".shader") {
					shaderEditorStateMachine.changeState(new ShaderEditingState(&shaderEditorStateMachine, path, relativePath.string()));
				}
			}

			ImGui::TextWrapped("%s", relativePath.string().c_str());
			ImGui::PopID();
			ImGui::NextColumn();
		}
		ImGui::Columns(1);



		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16.0f, 256.0f);
		ImGui::SliderFloat("Padding", &padding, 0.0f, 32.0f);
		ImGui::End();
		fileCreatorMachine.update(0.0f);
		materialEditorStateMachine.update(0.0f);
		shaderEditorStateMachine.update(0.0f);
	}
}