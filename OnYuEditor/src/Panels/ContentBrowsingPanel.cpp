#include "ContentBrowsingPanel.h"
namespace OnYuu {

	ContentBrowsingPanel::ContentBrowsingPanel()
	{
		m_currentDirectory = Project::getInstance().getAssetsPath();
		m_folderIcon = Texture::createTexture("resources/icons/ContentBrowser/DirectoryIcon.png");
		m_fileIcon = Texture::createTexture("resources/icons/ContentBrowser/FileIcon.png");
		m_folderIconWrapper = ImGuiTextureWrapper::create(m_folderIcon);
		m_fileIconWrapper = ImGuiTextureWrapper::create(m_fileIcon);
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
		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;
		ImGui::Columns(columnCount,0,false);
		for (auto& entry : std::filesystem::directory_iterator(m_currentDirectory))
		{
			std::filesystem::path path = entry.path();
			auto relativePath = std::filesystem::relative(path, Project::getInstance().getAssetsPath());

			std::string relativePathString = relativePath.string();
			std::string  filename = path.filename().string();
			std::shared_ptr<ImGuiTextureWrapper> icon = entry.is_directory() ? m_folderIconWrapper : m_fileIconWrapper;
			ImGui::ImageButton(filename.c_str(), icon->getTextureID(), {thumbnailSize, thumbnailSize});
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
				if (entry.is_directory())
				{
					m_currentDirectory /= path.filename();
				}
				else {
				}
			}
			ImGui::TextWrapped(relativePathString.c_str());

			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16.0f, 256.0f);
		ImGui::SliderFloat("Padding", &padding, 0.0f, 32.0f);
		ImGui::End();
	}
}