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
	};
}