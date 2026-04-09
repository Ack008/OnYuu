#pragma once
#include "Core/Engine.h"
#include <filesystem>

namespace OnYuu {
	class ContentBrowsingPanel
	{
	public:
		ContentBrowsingPanel();
		~ContentBrowsingPanel();
		void OnImGuiRender();
	private:
		void contentBrowserContextMenu();
		void openItemContextMenu(const std::filesystem::path& itemPath, bool isDirectory);
		void createFolderPopup();
		void createShaderPopup();
		void createMaterialPopup();
	private:
		std::filesystem::path m_currentDirectory;
		std::filesystem::path m_contextItemPath;
		bool m_contextItemIsDirectory = false;
		bool m_openRenamePopup = false;
		bool m_openCreateFolderPopup = false;
		bool m_openCreateShaderPopup = false;
		bool m_openCreateMaterialPopup = false;
		char m_renameBuffer[256] = {};
		char m_createFolderBuffer[256] = "New Folder";
		char m_createShaderBuffer[256] = "New Shader";
		char m_createMaterialBuffer[256] = "New Material";
		std::string m_createMaterialShaderPath;
		std::shared_ptr<Texture> m_folderIcon;
		std::shared_ptr<Texture> m_fileIcon;
		std::shared_ptr<ImGuiTextureWrapper> m_folderIconWrapper;
		std::shared_ptr<ImGuiTextureWrapper> m_fileIconWrapper;
		
	};
}