#include "ContentBrowsingPanel.h"
#include <cstring>
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

		contentBrowserContextMenu();
		createFolderPopup();
		createShaderPopup();

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
			openItemContextMenu(path, entry.is_directory());

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (entry.is_directory()) {
					m_currentDirectory /= path.filename();
				}
			}

			ImGui::TextWrapped("%s", relativePath.string().c_str());
			ImGui::PopID();
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		if (m_openRenamePopup) {
			ImGui::OpenPopup("RenamePopup");
			m_openRenamePopup = false;
		}

		if (ImGui::BeginPopupModal("RenamePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("New name", m_renameBuffer, sizeof(m_renameBuffer));

			if (ImGui::Button("OK"))
			{
				if (m_contextItemPath.has_parent_path() && std::strlen(m_renameBuffer) > 0) {
					std::filesystem::path newPath = m_contextItemPath.parent_path() / m_renameBuffer;
					std::error_code ec;
					std::filesystem::rename(m_contextItemPath, newPath, ec);
					if (!ec) {
						if (m_currentDirectory == m_contextItemPath) {
							m_currentDirectory = newPath;
						}
						m_contextItemPath = newPath;
					}
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		createMaterialPopup();

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16.0f, 256.0f);
		ImGui::SliderFloat("Padding", &padding, 0.0f, 32.0f);
		ImGui::End();
	}
	void ContentBrowsingPanel::createShaderPopup()
	{
		if (m_openCreateShaderPopup) {
			ImGui::OpenPopup("CreateShaderPopup");
			m_openCreateShaderPopup = false;
		}
		if (ImGui::BeginPopupModal("CreateShaderPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Shader Name", m_createShaderBuffer, sizeof(m_createShaderBuffer));
			if (ImGui::Button("Create"))
			{
				std::filesystem::path shaderPath = m_currentDirectory / (std::string(m_createShaderBuffer) + ".shader");
				std::ofstream shaderFile(shaderPath);
				if (shaderFile.is_open()) {
					shaderFile << "// New shader created by OnYuu Editor\n";
					shaderFile.close();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void ContentBrowsingPanel::createMaterialPopup()
	{
		if(m_openCreateMaterialPopup) {
			if (AssetManager::instance().getShaderPtr(m_createMaterialShaderPath) == nullptr) {
				AssetManager::instance().addShader(m_createMaterialShaderPath);
				std::cout << "Created MetaShader for " << m_createMaterialShaderPath << std::endl;
			}
		}
	}
	void ContentBrowsingPanel::openItemContextMenu(const std::filesystem::path& itemPath, bool isDirectory)
	{
		ImGui::OpenPopupOnItemClick("item_ctx", ImGuiPopupFlags_MouseButtonRight);
		if (ImGui::BeginPopup("item_ctx"))
		{
			m_contextItemPath = itemPath;
			m_contextItemIsDirectory = isDirectory;
			std::memset(m_renameBuffer, 0, sizeof(m_renameBuffer));
			std::string name = itemPath.filename().string();
			strncpy_s(m_renameBuffer, name.c_str(), _TRUNCATE);

			if (ImGui::MenuItem("Open"))
			{
				if (isDirectory) {
					m_currentDirectory = itemPath;
				}else {
					std::string extension = itemPath.extension().string();
					if (extension == ".shader") {
						// Open shader file
					}
				}
			}
			if (ImGui::MenuItem("Delete"))
			{
				std::error_code ec;
				std::filesystem::remove_all(itemPath, ec);
				if (!ec && m_currentDirectory == itemPath) {
					m_currentDirectory = itemPath.parent_path();
				}
			}
			if (ImGui::MenuItem("Rename"))
			{
				m_openRenamePopup = true;
			}

			std::string extension = itemPath.extension().string();
			if (extension == ".shader") {
				if (ImGui::MenuItem("Create Material"))
				{
					m_openCreateMaterialPopup = true;
					std::memset(m_createMaterialBuffer, 0, sizeof(m_createMaterialBuffer));
					strncpy_s(m_createMaterialBuffer, "New Material", _TRUNCATE);
					m_createMaterialShaderPath = itemPath.string();
				}
			}

			ImGui::EndPopup();
		}
	}

	void ContentBrowsingPanel::contentBrowserContextMenu()
	{
		if (ImGui::BeginPopupContextWindow("browser_ctx", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Folder"))
			{
				m_openCreateFolderPopup = true;
				std::memset(m_createFolderBuffer, 0, sizeof(m_createFolderBuffer));
				strncpy_s(m_createFolderBuffer, "New Folder", _TRUNCATE);
			}
			if (ImGui::MenuItem("Create shader"))
			{
				m_openCreateShaderPopup = true;
				std::memset(m_createShaderBuffer, 0, sizeof(m_createShaderBuffer));
				strncpy_s(m_createShaderBuffer, "New Shader", _TRUNCATE);

			}
			if (ImGui::MenuItem("Refresh"))
			{
				// No specific action needed as the directory will be re-read on the next render
			}
			ImGui::EndPopup();
		}
	}

	void ContentBrowsingPanel::createFolderPopup()
	{
		if (m_openCreateFolderPopup) {
			ImGui::OpenPopup("CreateFolderPopup");
			m_openCreateFolderPopup = false;
		}

		if (ImGui::BeginPopupModal("CreateFolderPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Folder Name", m_createFolderBuffer, sizeof(m_createFolderBuffer));
			if (ImGui::Button("Create"))
			{
				std::filesystem::path folderPath = m_currentDirectory / m_createFolderBuffer;
				std::error_code ec;
				std::filesystem::create_directory(folderPath, ec);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}