#include "SceneHierarchyPanel.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>
#include <algorithm>
#include <type_traits>

/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace OnYuu {

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			for (auto entityID : m_Context->reg->view<entt::entity>())
			{
				GameObject entity{ entityID , m_Context.get() };
				DrawEntityNode(entity);
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			if (ImGui::BeginPopupContextWindow("SceneHierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					m_Context->createEntity().getComponent<TagComponent>().tag = "New Entity";
				}
				if (ImGui::MenuItem("Create sphere"))
				{
					GameObject obj = m_Context->createEntity();
					obj.getComponent<TagComponent>().tag = "New Sphere";
					auto& renderMeshComponent = obj.addComponent<RenderMeshComponent>();
					renderMeshComponent.mesh = AssetManager::instance().getMeshPtr("sphere");
					renderMeshComponent.material = AssetManager::instance().getMaterialPtr("default");
					renderMeshComponent.renderingType = RenderingTypeEnum::TRIANGLE;
				}
				if (ImGui::MenuItem("Create cube"))
				{
					GameObject obj = m_Context->createEntity();
					obj.getComponent<TagComponent>().tag = "New Cube";
					auto& renderMeshComponent = obj.addComponent<RenderMeshComponent>();
					renderMeshComponent.mesh = AssetManager::instance().getMeshPtr("cube");
					renderMeshComponent.material = AssetManager::instance().getMaterialPtr("default");
					renderMeshComponent.renderingType = RenderingTypeEnum::TRIANGLE;
				}
				if (ImGui::MenuItem("Create quad"))
				{
					GameObject obj = m_Context->createEntity();
					obj.getComponent<TagComponent>().tag = "New Quad";
					auto& renderMeshComponent = obj.addComponent<RenderMeshComponent>();
					renderMeshComponent.mesh = AssetManager::instance().getMeshPtr("quad");
					renderMeshComponent.material = AssetManager::instance().getMaterialPtr("default");
					renderMeshComponent.renderingType = RenderingTypeEnum::TRIANGLE;
				}
				
				ImGui::EndPopup();
			}
		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
			DrawComponents(m_SelectionContext);
		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(GameObject entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(GameObject entity)
	{
		auto& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.getID(), flags, tag.c_str());
		if (ImGui::IsItemClicked())
			m_SelectionContext = entity;

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			ImGui::EndPopup();
		}

		if (opened)
			ImGui::TreePop();

		if (entityDeleted)
		{
			entity.Destroy();
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFrameHeight();
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		 values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		 values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, GameObject entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		if (entity.hasComponent<T>())
		{
			auto& component = entity.getComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			const float lineHeight = ImGui::GetFrameHeight();
			const float buttonWidth = lineHeight * 1.5f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SetNextItemAllowOverlap();
			if(ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Remove component"))
				{
					entity.removeComponent<T>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Component options");

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}
		}
	}

	void SceneHierarchyPanel::DrawComponents(GameObject entity)
	{
		if (entity.hasComponent<TagComponent>())
		{
			auto& tag = entity.getComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer) - 1);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!m_SelectionContext.hasComponent<Orthographic>())
			{
				if (ImGui::MenuItem("Orthographic Camera"))
				{
					m_SelectionContext.addComponent<Orthographic>(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.hasComponent<Perspective>())
			{
				if (ImGui::MenuItem("Perspective Camera"))
				{
					m_SelectionContext.addComponent<Perspective>(45.0f, 1.778f, 0.01f, 1000.0f);
					ImGui::CloseCurrentPopup();
				}
			}

			DisplayAddComponentEntry<RigidBody>("Rigidbody");
			DisplayAddComponentEntry<BoxCollider>("Box Collider");
			DisplayAddComponentEntry<RenderMeshComponent>("Mesh Renderer");

			if (!m_SelectionContext.hasComponent<CircleCollider>())
			{
				if (ImGui::MenuItem("Circle Collider"))
				{
					m_SelectionContext.addComponent<CircleCollider>(0.5f);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<Trasform>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Position", component.position);
				DrawVec3Control("Rotation", component.rotation);
				DrawVec3Control("Scale", component.scale, 1.0f);
			});

		DrawComponent<Orthographic>("Orthographic Camera", entity, [](auto& component)
			{
				bool isActive = component.getActive();
				if (ImGui::Checkbox("Active", &isActive))
					component.setActive(isActive);
			});

		DrawComponent<Perspective>("Perspective Camera", entity, [](auto& component)
			{
				bool isActive = component.getActive();
				if (ImGui::Checkbox("Active", &isActive))
					component.setActive(isActive);
			});
		DrawComponent<RenderMeshComponent>("Mesh Renderer", entity, [](auto& component)
			{
				ImGui::Text("Mesh: %s", component.mesh ? "sphere" : "None");
				if (!component.mesh) {
					ImGui::Button("Load Mesh");
				}
				else {
					if (ImGui::BeginCombo("Rendering Type", component.renderingType == RenderingTypeEnum::TRIANGLE ? "Triangle" : "Line")) {
						if (ImGui::Selectable("Triangle", component.renderingType == RenderingTypeEnum::TRIANGLE)) {
						 component.renderingType = RenderingTypeEnum::TRIANGLE;
						}
						if (ImGui::Selectable("Line", component.renderingType == RenderingTypeEnum::LINE)) {
							component.renderingType = RenderingTypeEnum::LINE;
						}
						if (ImGui::Selectable("Triangle Fan", component.renderingType == RenderingTypeEnum::TRIANGLE_FAN)) {
							component.renderingType = RenderingTypeEnum::TRIANGLE_FAN;
						}
						if (ImGui::Selectable("Triangle Strip", component.renderingType == RenderingTypeEnum::TRIANGLE_STRIP)) {
							component.renderingType = RenderingTypeEnum::TRIANGLE_STRIP;
						}
						
						ImGui::EndCombo();
					}
					std::string materialLabel = "Material: " + std::string(component.material ? "Loaded" : "None");
					ImGui::Button(materialLabel.c_str());
					if (ImGui::Button("UnLoad Mesh")) {
						component.mesh = nullptr;
						component.material = nullptr;
					}
					if(component.material)
					{

						if (ImGui::BeginCombo("Material", "Values")) {
							auto material = component.material;
							auto& uniforms = material->getUniforms();
							for (auto& uniform : uniforms) {
								auto& name = uniform.first;
								auto& value = uniform.second;

								if (auto* pInt = std::get_if<int>(&value)) {
									ImGui::DragInt(name.c_str(), pInt);
								}
								else if (auto* pFloat = std::get_if<float>(&value)) {
									ImGui::DragFloat(name.c_str(), pFloat);
								}
								else if (auto* pVec2 = std::get_if<glm::vec2>(&value)) {
									ImGui::DragFloat2(name.c_str(), glm::value_ptr(*pVec2));
								}
								else if (auto* pVec3 = std::get_if<glm::vec3>(&value)) {
									ImGui::DragFloat3(name.c_str(), glm::value_ptr(*pVec3));
								}
								else if (auto* pVec4 = std::get_if<glm::vec4>(&value)) {
									ImGui::DragFloat4(name.c_str(), glm::value_ptr(*pVec4));
								}
								else if (std::get_if<glm::mat3>(&value)) {
									ImGui::Text("%s (mat3)", name.c_str());
								}
								else if (std::get_if<glm::mat4>(&value)) {
									ImGui::Text("%s (mat4)", name.c_str());
								}
								else if (std::get_if<std::shared_ptr<Texture>>(&value)) {
									ImGui::Button(name.c_str());
								}
							}

							ImGui::EndCombo();
						}
					}
				}
			});
		if (entity.hasComponent<ScriptingSystem>()) {
			auto& scriptingSystem = entity.getComponent<ScriptingSystem>();
			for (const auto& script : scriptingSystem.scripts) {
				if (ImGui::TreeNodeEx((void*)script.get(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth, "%s", typeid(*script).name())) {
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Remove component"))
						{
							scriptingSystem.scripts.erase(std::remove(scriptingSystem.scripts.begin(), scriptingSystem.scripts.end(), script), scriptingSystem.scripts.end());
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
					ImGui::TreePop();
				}
			}
		}


	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.hasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.addComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

}