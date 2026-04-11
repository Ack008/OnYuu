#pragma once
#include "Core/Engine.h"
#include "../Helpers/MaterialFileWriter.h"
#include <filesystem>

namespace OnYuu {
	class ContentBrowsingPanel
	{
	public:
		ContentBrowsingPanel();
		~ContentBrowsingPanel();
		void OnImGuiRender();
		void openRenamePopup();

		struct MaterialParamEditor {
			std::string name;
			std::string type;
			bool isTexture = false;
			std::string texturePath;
			char texturePathBuffer[512] = {};
			int intValue = 0;
			float floatValue = 0.0f;
			bool boolValue = false;
			float vec2[2] = { 0.0f, 0.0f };
			float vec3[3] = { 0.0f, 0.0f, 0.0f };
			float vec4[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			float mat3[9] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
			float mat4[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		};

		struct MaterialEditorState {
			std::filesystem::path materialPath;
			std::filesystem::path shaderPath;
			std::vector<MaterialParamEditor> params;
		};

		struct ShaderEditorState {
			std::filesystem::path shaderPath;
			std::string content;
			bool isDirty = false;
		};

	private:
		void contentBrowserContextMenu();
		void openItemContextMenu(const std::filesystem::path& itemPath, bool isDirectory);
		void createFolderPopup();
		void createShaderPopup();
		void createMaterialPopup();
		void openMaterialPopup();
		bool loadMaterialEditor(const std::filesystem::path& materialPath);
		bool saveMaterialEditor();
		void rebuildMaterialParamsFromShader();
		void drawMaterialParamEditor(MaterialParamEditor& param);
		bool loadShaderEditor(const std::filesystem::path& shaderPath);
		bool saveShaderEditor();
		void openShaderEditorWindow();
		static std::string normalizeMaterialType(std::string type);
		static bool isTextureLikeType(const std::string& type);
		static std::filesystem::path openPngFilePicker(const std::filesystem::path& initialDir = {});
		static std::filesystem::path openShaderFilePicker(const std::filesystem::path& initialDir = {});

	private:
		std::filesystem::path m_currentDirectory;
		std::filesystem::path m_contextItemPath;
		bool m_contextItemIsDirectory = false;
		bool m_openRenamePopup = false;
		bool m_openCreateFolderPopup = false;
		bool m_openCreateShaderPopup = false;
		bool m_openCreateMaterialPopup = false;
		bool m_openEditMaterialPopup = false;
		char m_renameBuffer[256] = {};
		char m_createFolderBuffer[256] = "New Folder";
		char m_createShaderBuffer[256] = "New Shader";
		char m_createMaterialBuffer[256] = "New Material";
		char m_editMaterialShaderBuffer[512] = {};
		std::string m_createMaterialShaderPath;
		std::shared_ptr<Texture> m_folderIcon;
		std::shared_ptr<Texture> m_fileIcon;
		std::shared_ptr<ImGuiTextureWrapper> m_folderIconWrapper;
		std::shared_ptr<ImGuiTextureWrapper> m_fileIconWrapper;
		MaterialFileWriter m_materialFileWriter;
		MaterialEditorState m_materialEditor;
		std::filesystem::path m_materialToEditPath;
		bool m_materialEditorLoaded = false;

		// Shader editor
		ShaderEditorState m_shaderEditor;
		bool m_shaderEditorOpen = false;
		static constexpr size_t SHADER_BUFFER_SIZE = 64 * 1024; // 64KB per shader text
		char m_shaderEditorBuffer[SHADER_BUFFER_SIZE] = {};
	};
}