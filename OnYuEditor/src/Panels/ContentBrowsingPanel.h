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
		std::filesystem::path m_currentDirectory;
		std::shared_ptr<Texture> m_folderIcon;
		std::shared_ptr<Texture> m_fileIcon;
		std::shared_ptr<ImGuiTextureWrapper> m_folderIconWrapper;
		std::shared_ptr<ImGuiTextureWrapper> m_fileIconWrapper;
		
	};
}