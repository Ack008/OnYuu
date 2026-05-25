#pragma once
#include "Core/Engine.h"
#include "../Helpers/MaterialFileWriter.h"
#include "../StateMachine/StateMachine.h"
#include <filesystem>

namespace OnYuu {
	class ContentBrowsingPanel
	{
	public:
		ContentBrowsingPanel();
		~ContentBrowsingPanel();
		void OnImGuiRender();
		void openRenamePopup();


		struct ShaderEditorState {
			std::filesystem::path shaderPath;
			std::string content;
			bool isDirty = false;
		};

	private:
		void openContextualMenu();
	private:
		std::filesystem::path m_currentDirectory;
		std::filesystem::path m_contextItemPath;

		std::string m_createMaterialShaderPath;
		std::shared_ptr<Texture> m_folderIcon;
		std::shared_ptr<Texture> m_fileIcon;
		std::shared_ptr<ImGuiTextureWrapper> m_folderIconWrapper;
		std::shared_ptr<ImGuiTextureWrapper> m_fileIconWrapper;
		

		// Shader editor
		ShaderEditorState m_shaderEditor;
		bool m_shaderEditorOpen = false;
		static constexpr size_t SHADER_BUFFER_SIZE = 64 * 1024; // 64KB per shader text
		char m_shaderEditorBuffer[SHADER_BUFFER_SIZE] = {};
		StateMachine materialCreatorStateMachine;
		StateMachine materialEditorStateMachine;

	};
}