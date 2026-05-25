#include "SceneHierarchyPanel.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include "ImGuiFileDialog.h"

#include <glm/gtc/type_ptr.hpp>

#include <cstring>
#include <algorithm>
#include <type_traits>
#include <filesystem>

#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif
namespace OnYuu {

    namespace {
        constexpr const char* kImportMeshDialogId = "ImportMeshObjDialog";
        constexpr const char* kImportShaderDialogId = "ImportMeshShaderDialog";
    }

    // ─── Palette ────────────────────────────────────────────────────────────────
    namespace Theme {
        // Backgrounds
        static constexpr ImVec4 BgDeep = { 0.10f, 0.10f, 0.12f, 1.00f };
        static constexpr ImVec4 BgPanel = { 0.13f, 0.13f, 0.16f, 1.00f };
        static constexpr ImVec4 BgItem = { 0.17f, 0.17f, 0.21f, 1.00f };
        static constexpr ImVec4 BgItemHover = { 0.22f, 0.22f, 0.28f, 1.00f };
        static constexpr ImVec4 BgItemActive = { 0.26f, 0.26f, 0.33f, 1.00f };
        // Accents
        static constexpr ImVec4 Accent = { 0.38f, 0.62f, 1.00f, 1.00f };  // blue
        static constexpr ImVec4 AccentDim = { 0.28f, 0.45f, 0.75f, 1.00f };
        static constexpr ImVec4 AccentActive = { 0.50f, 0.75f, 1.00f, 1.00f };
        static constexpr ImVec4 Danger = { 0.85f, 0.25f, 0.25f, 1.00f };
        static constexpr ImVec4 DangerHover = { 1.00f, 0.35f, 0.35f, 1.00f };
        static constexpr ImVec4 Success = { 0.25f, 0.75f, 0.45f, 1.00f };
        // Text
        static constexpr ImVec4 TextPrimary = { 0.92f, 0.92f, 0.95f, 1.00f };
        static constexpr ImVec4 TextDim = { 0.55f, 0.55f, 0.62f, 1.00f };
        static constexpr ImVec4 TextSelected = { 1.00f, 1.00f, 1.00f, 1.00f };
        // Separator
        static constexpr ImVec4 Border = { 0.25f, 0.25f, 0.32f, 1.00f };
    }

    // ─── Style helpers ──────────────────────────────────────────────────────────
    static void PushPanelStyle()
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::BgPanel);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::BgDeep);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, Theme::BgItem);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Theme::BgItemHover);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Theme::BgItemActive);
        ImGui::PushStyleColor(ImGuiCol_Header, Theme::BgItemHover);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Theme::BgItemActive);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, Theme::AccentDim);
        ImGui::PushStyleColor(ImGuiCol_Button, Theme::BgItem);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::BgItemHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Theme::BgItemActive);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, Theme::BgDeep);
        ImGui::PushStyleColor(ImGuiCol_Separator, Theme::Border);
        ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, Theme::AccentDim);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Theme::BgDeep);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, Theme::BgDeep);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, Theme::BgDeep);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, Theme::BgItemActive);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, Theme::AccentDim);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, Theme::Accent);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, Theme::Accent);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, Theme::AccentActive);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextPrimary);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.f, 4.f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.f, 5.f));
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 16.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 3.f);
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 4.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    }

    static void PopPanelStyle()
    {
        ImGui::PopStyleVar(9);
        ImGui::PopStyleColor(23);
    }

    // Draws a thin colored left-border accent before a header line
    static void DrawAccentBar(float height, ImVec4 color = Theme::Accent)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            { p.x - 4.f, p.y },
            { p.x - 1.f, p.y + height },
            ImGui::ColorConvertFloat4ToU32(color), 1.f
        );
    }

    // Small colored badge text (no newline)
    static void BadgeText(const char* label, ImVec4 bg, ImVec4 fg = Theme::TextSelected)
    {
        ImVec2 size = ImGui::CalcTextSize(label);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            { p.x - 3.f, p.y - 1.f },
            { p.x + size.x + 3.f, p.y + size.y + 1.f },
            ImGui::ColorConvertFloat4ToU32(bg), 3.f
        );
        ImGui::TextColored(fg, "%s", label);
    }

    static bool ResolveMaterialAssetPath(const std::string& materialName, std::filesystem::path& outFilePath, std::string& outMaterialKey)
    {
        const std::filesystem::path assetsRoot = Project::getInstance().getAssetsPath();
        std::filesystem::path requested(materialName);

        auto normalizeKey = [&](const std::filesystem::path& p) {
            std::filesystem::path keyPath = p;
           
            return keyPath.string();
        };

        std::filesystem::path candidate = assetsRoot / requested;
        

        if (std::filesystem::exists(candidate)) {
            outFilePath = candidate;
            outMaterialKey = normalizeKey(requested);
            return true;
        }

        const std::string requestedStem = requested.string();
        if (requestedStem.empty()) {
            return false;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsRoot)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".mat") {
                continue;
            }

            if (entry.path().stem() == requestedStem) {
                outFilePath = entry.path();
                outMaterialKey = normalizeKey(std::filesystem::relative(entry.path(), assetsRoot));
                return true;
            }
        }

        return false;
    }

    // ─── Panel ──────────────────────────────────────────────────────────────────
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
        PushPanelStyle();

        // ── Scene Hierarchy ────────────────────────────────────────────────────
        ImGui::Begin("  \xef\x89\xb3  Scene Hierarchy");  // 󉳣 unicode fallback: plain icon

        if (m_Context)
        {
            // Search bar
            ImGui::SetNextItemWidth(-1.f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, Theme::BgDeep);
            static char searchBuf[128] = "";
            ImGui::InputTextWithHint("##search", "\xef\x80\x82  Search entities...", searchBuf, sizeof(searchBuf));
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            const std::string filter(searchBuf);

            for (auto entityID : m_Context->reg->view<entt::entity>())
            {
                GameObject entity{ entityID, m_Context.get() };
				if (entity.getComponent<TreeComponent>().father)
                    continue;  
                const auto& tag = entity.getComponent<TagComponent>().tag;
                if (!filter.empty())
                {
                    std::string lower = tag;
                    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                    std::string lowerFilter = filter;
                    std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);
                    if (lower.find(lowerFilter) == std::string::npos)
                        continue;
                }
                DrawEntityNode(entity);
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                m_SelectionContext = {};

            // Context menu — create entities
            if (ImGui::BeginPopupContextWindow("SceneHierarchyContext",
                ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                ImGui::TextColored(Theme::TextDim, "CREATE");
                ImGui::Separator();
                ImGui::Spacing();

                auto makeObj = [&](const char* label, const char* meshKey, const char* tag) {
                    if (ImGui::MenuItem(label)) {
                        GameObject obj = m_Context->createEntity();
                        obj.getComponent<TagComponent>().tag = tag;
                        auto& rmc = obj.addComponent<RenderMeshComponent>();
                        rmc.setMesh(AssetManager::instance().getMeshPtr(meshKey));
                        rmc.setMaterialID("default");
                        rmc.setRenderingType(RenderingTypeEnum::TRIANGLE);

                    }
                    };

                if (ImGui::MenuItem("\xef\x86\xb0  Empty Entity"))
                    m_Context->createEntity().getComponent<TagComponent>().tag = "New Entity";

                ImGui::Spacing();
                ImGui::TextColored(Theme::TextDim, "PRIMITIVES");
                ImGui::Separator();
                makeObj("○  Sphere", "sphere", "New Sphere");
                makeObj("□  Cube", "cube", "New Cube");
                makeObj("△  Quad", "quad", "New Quad");
                if (ImGui::MenuItem("Import Mesh")) {
                    IGFD::FileDialogConfig config;
                    config.path = Project::getInstance().getAssetsPath();
                    ImGuiFileDialog::Instance()->OpenDialog(kImportMeshDialogId, "Import Mesh", ".obj", config);
                }
                ImGui::EndPopup();
            }

            static std::filesystem::path pendingImportedMeshPath;
            static bool openShaderDialogNextFrame = false;

            if (ImGuiFileDialog::Instance()->Display(kImportMeshDialogId)) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    pendingImportedMeshPath = ImGuiFileDialog::Instance()->GetFilePathName();
                    openShaderDialogNextFrame = !pendingImportedMeshPath.empty();
                }
                ImGuiFileDialog::Instance()->Close();
            }

            if (openShaderDialogNextFrame) {
                IGFD::FileDialogConfig shaderConfig;
                shaderConfig.path = Project::getInstance().getAssetsPath();
                ImGuiFileDialog::Instance()->OpenDialog(kImportShaderDialogId, "Select Shader", ".shader", shaderConfig);
                openShaderDialogNextFrame = false;
            }

            if (ImGuiFileDialog::Instance()->Display(kImportShaderDialogId)) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::filesystem::path shaderPath = ImGuiFileDialog::Instance()->GetFilePathName();
                    auto metaShader = AssetManager::instance().addShader(shaderPath.string());
                    if (metaShader && metaShader->getShader() && !pendingImportedMeshPath.empty()) {
                        GameObject importedRoot = MeshImporter::instance().importMesh(pendingImportedMeshPath.string(), m_Context.get(), shaderPath.string());
                        if (importedRoot) {
                            m_SelectionContext = importedRoot;
                        }
                    }
                    pendingImportedMeshPath.clear();
                }
                ImGuiFileDialog::Instance()->Close();
            }

            // Footer: entity count
            ImGui::Spacing();
            ImGui::Separator();
            int count = (int)m_Context->reg->view<entt::entity>().size();
            ImGui::TextColored(Theme::TextDim, "  %d entit%s", count, count == 1 ? "y" : "ies");
        }

        ImGui::End();

        // ── Properties ─────────────────────────────────────────────────────────
        ImGui::Begin("  \xef\x87\xbc  Properties");
        if (m_SelectionContext)
            DrawComponents(m_SelectionContext);
        else
        {
            ImGui::Spacing();
            const char* hint = "Select an entity\nto inspect its components";
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImVec2 textSize = ImGui::CalcTextSize(hint, nullptr, false, avail.x);
            ImGui::SetCursorPos({
                (avail.x - textSize.x) * 0.5f + ImGui::GetStyle().WindowPadding.x,
                avail.y * 0.35f
                });
            ImGui::TextDisabled("%s", hint);
        }
        ImGui::End();

        PopPanelStyle();
    }

    void SceneHierarchyPanel::SetSelectedEntity(GameObject entity)
    {
        m_SelectionContext = entity;
    }

    // ─── Entity node ────────────────────────────────────────────────────────────
    void SceneHierarchyPanel::DrawEntityNode(GameObject entity)
    {
        auto& tag = entity.getComponent<TagComponent>().tag;
        bool selected = (m_SelectionContext == entity);

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_FramePadding;
        if (selected) flags |= ImGuiTreeNodeFlags_Selected;

        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Header, Theme::AccentDim);
        else
            ImGui::PushStyleColor(ImGuiCol_Header, { 0,0,0,0 });

        // Icon prefix based on components
        const char* icon = entity.hasComponent<RenderMeshComponent>() ? "\xef\x83\xa1 " :
            entity.hasComponent<Perspective>() ? "\xef\x80\xb0 " :
            "\xef\x86\xb0 ";

        std::string label = std::string(icon) + tag;
        bool opened = ImGui::TreeNodeEx(
            (void*)(uint64_t)(uint32_t)entity.getID(), flags, "%s", label.c_str());

        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
            m_SelectionContext = entity;

        // Hover highlight text
        if (ImGui::IsItemHovered() && !selected)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("\xef\x87\xb4  Duplicate"))
            { /* TODO */
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Create Empty Child"))
            {
                GameObject child = m_Context->createEntity();
                child.getComponent<TagComponent>().tag = "New Child";
                child.setFather(entity);
            }
            if (ImGui::MenuItem("Create sphere ")) {
                GameObject child = m_Context->createEntity();
				child.setFather(entity);
                child.getComponent<TagComponent>().tag = "New Sphere";
                auto& rmc = child.addComponent<RenderMeshComponent>();
                rmc.setMesh(AssetManager::instance().getMeshPtr("sphere"));
                rmc.setMaterialID("default");
				rmc.setRenderingType(RenderingTypeEnum::TRIANGLE);
            }
            if (ImGui::MenuItem("Create cube ")) {
                GameObject child = m_Context->createEntity();
                child.setFather(entity);
                child.getComponent<TagComponent>().tag = "New Cube";
                auto& rmc = child.addComponent<RenderMeshComponent>();
                rmc.setMesh(AssetManager::instance().getMeshPtr("cube"));
                rmc.setMaterialID("default");
            }

            if (ImGui::MenuItem("Create quad ")) {
                GameObject child = m_Context->createEntity();
                child.setFather(entity);
                child.getComponent<TagComponent>().tag = "New Quad";
                auto& rmc = child.addComponent<RenderMeshComponent>();
                rmc.setMesh(AssetManager::instance().getMeshPtr("quad"));
                rmc.setMaterialID("default");
			}

            ImGui::PushStyleColor(ImGuiCol_Text, Theme::Danger);
            if (ImGui::MenuItem("\xef\x87\x97  Delete Entity"))
                entityDeleted = true;
            ImGui::PopStyleColor();
            
            ImGui::EndPopup();
        }

        if (opened) {
            auto& children = entity.getComponent<TreeComponent>().obj;
            for (std::size_t i = 0; i < children.size(); ++i)
            {
                DrawEntityNode(children[i]);
            }
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            entity.Destroy();
            if (m_SelectionContext == entity)
                m_SelectionContext = {};
        }
    }

    // ─── Vec3 control ───────────────────────────────────────────────────────────
    static void DrawVec3Control(const char* label, glm::vec3& v,
        float resetValue = 0.0f, float columnWidth = 90.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts.Size > 0 ? io.Fonts->Fonts[0] : io.Fonts->Fonts[0];

        ImGui::PushID(label);

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::TextColored(Theme::TextDim, "%s", label);
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 0.f });
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);

        const float lineH = ImGui::GetFrameHeight();
        const ImVec2 btnSz = { lineH + 2.f, lineH };

        struct Axis { const char* lbl; float* val; ImVec4 col; ImVec4 colH; };
        Axis axes[3] = {
            { "X", &v.x, { 0.80f,0.18f,0.18f,1.f }, { 1.00f,0.30f,0.30f,1.f } },
            { "Y", &v.y, { 0.20f,0.65f,0.25f,1.f }, { 0.30f,0.80f,0.35f,1.f } },
            { "Z", &v.z, { 0.15f,0.35f,0.85f,1.f }, { 0.25f,0.50f,1.00f,1.f } },
        };
        const char* dragIDs[3] = { "##X", "##Y", "##Z" };

        for (int i = 0; i < 3; ++i) {
            if (i > 0) ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, axes[i].col);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, axes[i].colH);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, axes[i].col);
            ImGui::PushFont(boldFont);
            if (ImGui::Button(axes[i].lbl, btnSz))
                *axes[i].val = resetValue;
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(dragIDs[i], axes[i].val, 0.1f, 0.f, 0.f, "%.2f");
            ImGui::PopItemWidth();
        }

        ImGui::PopStyleVar(2);
        ImGui::Columns(1);
        ImGui::PopID();
    }

    // ─── Component block ────────────────────────────────────────────────────────
    template<typename T, typename UIFunction>
    static void DrawComponent(const char* icon, const char* name,
        GameObject entity, UIFunction uiFunction,
        ImVec4 accentColor = Theme::Accent)
    {
        if (!entity.hasComponent<T>()) return;

        auto& component = entity.getComponent<T>();

        const ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_FramePadding;

        ImGui::Spacing();

        // Accent left bar
        float nodeH = ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y * 2.f;
        DrawAccentBar(nodeH, accentColor);

        ImGui::PushStyleColor(ImGuiCol_Header, { 0.18f,0.18f,0.23f,1.f });
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.22f,0.22f,0.28f,1.f });
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 0.25f,0.25f,0.32f,1.f });
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.f, 5.f });

        std::string fullName = std::string(icon) + "  " + name;
        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, "%s", fullName.c_str());

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::SetNextItemAllowOverlap();
        if (ImGui::BeginPopupContextItem())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::Danger);
            if (ImGui::MenuItem("\xef\x87\x97  Remove Component")) {
                entity.removeComponent<T>();
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor();
            ImGui::EndPopup();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Right-click for options");

        if (open)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.14f,0.14f,0.18f,1.f });
            ImGui::Indent(4.f);
            ImGui::Spacing();
            uiFunction(component);
            ImGui::Spacing();
            ImGui::Unindent(4.f);
            ImGui::PopStyleColor();
            ImGui::TreePop();
        }
    }

    // ─── Properties panel ───────────────────────────────────────────────────────
    void SceneHierarchyPanel::DrawComponents(GameObject entity)
    {
        // ── Tag / name ──────────────────────────────────────────────────────────
        if (entity.hasComponent<TagComponent>())
        {
            auto& tag = entity.getComponent<TagComponent>().tag;

            ImGui::PushStyleColor(ImGuiCol_FrameBg, Theme::BgDeep);
            ImGui::SetNextItemWidth(-1.f);
            char buffer[256] = {};
            strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer) - 1);
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
                tag = buffer;
            ImGui::PopFont();
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();

        // ── Add Component button ────────────────────────────────────────────────
        {
            const float btnW = ImGui::GetContentRegionAvail().x;
            ImGui::PushStyleColor(ImGuiCol_Button, Theme::AccentDim);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Accent);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, Theme::AccentActive);
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextSelected);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
            if (ImGui::Button("\xef\x81\x99  Add Component", { btnW, 28.f }))
                ImGui::OpenPopup("AddComponent");
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(4);
        }

        if (ImGui::BeginPopup("AddComponent"))
        {
            ImGui::TextColored(Theme::TextDim, "CAMERA");
            ImGui::Separator();
            if (!m_SelectionContext.hasComponent<Orthographic>())
                if (ImGui::MenuItem("\xef\x80\xb0  Orthographic Camera")) {
                    m_SelectionContext.addComponent<Orthographic>(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
                    ImGui::CloseCurrentPopup();
                }
            if (!m_SelectionContext.hasComponent<Perspective>())
                if (ImGui::MenuItem("\xef\x80\xb0  Perspective Camera")) {
                    m_SelectionContext.addComponent<Perspective>(45.f, 1.778f, 0.01f, 1000.f);
                    ImGui::CloseCurrentPopup();
                }

            ImGui::Spacing();
            ImGui::TextColored(Theme::TextDim, "PHYSICS");
            ImGui::Separator();
            DisplayAddComponentEntry<RigidBody>("\xef\x89\xa6  Rigidbody");
            DisplayAddComponentEntry<BoxCollider>("\xe2\x96\xa1  Box Collider");
            if (!m_SelectionContext.hasComponent<CircleCollider>())
                if (ImGui::MenuItem("\xe2\x97\x8b  Circle Collider")) {
                    m_SelectionContext.addComponent<CircleCollider>(0.5f);
                    ImGui::CloseCurrentPopup();
                }

            ImGui::Spacing();
            ImGui::TextColored(Theme::TextDim, "RENDERING");
            ImGui::Separator();
            DisplayAddComponentEntry<RenderMeshComponent>("\xef\x83\xa1  Mesh Renderer");

            ImGui::EndPopup();
        }

        ImGui::Spacing();

        // ── Transform ──────────────────────────────────────────────────────────
        DrawComponent<Transform>("\xef\x81\xb6", "Transform", entity, [](auto& c)
            {
                glm::vec3 position = c.getPosition();
                glm::vec3 rotation = c.getRotation();
                glm::vec3 scale = c.getScale();
                DrawVec3Control("Position", position);
                DrawVec3Control("Rotation", rotation);
                DrawVec3Control("Scale", scale, 1.0f);
                c.setPosition(position);
                c.setRotation(rotation);
                c.setScale(scale);
            }, { 0.55f, 0.88f, 0.45f, 1.f });

        // ── Cameras ────────────────────────────────────────────────────────────
        DrawComponent<Orthographic>("\xef\x80\xb0", "Orthographic Camera", entity, [](auto& c)
            {
                bool active = c.getActive();
                if (ImGui::Checkbox("Active", &active)) c.setActive(active);
            }, { 0.55f, 0.75f, 1.f, 1.f });

        DrawComponent<Perspective>("\xef\x80\xb0", "Perspective Camera", entity, [](auto& c)
            {
                bool active = c.getActive();
                if (ImGui::Checkbox("Active", &active)) c.setActive(active);
            }, { 0.55f, 0.75f, 1.f, 1.f });

        // ── Mesh Renderer ──────────────────────────────────────────────────────
        DrawComponent<RenderMeshComponent>("\xef\x83\xa1", "Mesh Renderer", entity, [](auto& c)
            {
                // Mesh row
                ImGui::TextColored(Theme::TextDim, "Mesh");
                ImGui::SameLine(96.f);
                if (c.getMesh()) {
                    BadgeText("  Loaded  ", { 0.22f,0.55f,0.28f,1.f });
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, Theme::Danger);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::DangerHover);
                    if (ImGui::SmallButton("Unload")) {
                        c.setMesh(nullptr);
                        c.setMaterialID("");
                        c.setRenderingType(RenderingTypeEnum::TRIANGLE);    
                    }
                    ImGui::PopStyleColor(2);
                }
                else {
                    BadgeText("  None  ", { 0.45f,0.20f,0.20f,1.f });
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Load")) { /* TODO */ }
                }

                if (!c.getMesh()) return;

                ImGui::Spacing();

                // Rendering type
                const char* rtLabel =
                    c.getRenderingType() == RenderingTypeEnum::TRIANGLE ? "Triangle" :
                    c.getRenderingType() == RenderingTypeEnum::LINE ? "Line" :
                    c.getRenderingType() == RenderingTypeEnum::TRIANGLE_FAN ? "Triangle Fan" :
                    c.getRenderingType() == RenderingTypeEnum::TRIANGLE_STRIP ? "Triangle Strip" : "Unknown";
                ImGui::TextColored(Theme::TextDim, "Topology");
                ImGui::SameLine(80.f);
                ImGui::SetNextItemWidth(-1.f);
                if (ImGui::BeginCombo("##RenderingType", rtLabel))
                {
                    auto sel = [&](const char* label, RenderingTypeEnum val) {
                        bool s = c.getRenderingType() == val;
                        if (ImGui::Selectable(label, s)) c.setRenderingType(val);
                        if (s) ImGui::SetItemDefaultFocus();
                        };
                    sel("Triangle", RenderingTypeEnum::TRIANGLE);
                    sel("Line", RenderingTypeEnum::LINE);
                    sel("Triangle Fan", RenderingTypeEnum::TRIANGLE_FAN);
                    sel("Triangle Strip", RenderingTypeEnum::TRIANGLE_STRIP);
                    ImGui::EndCombo();
                }

                // Material
                ImGui::Spacing();
                ImGui::Button( "Material");
                ImGui::SameLine(150.f);
                if (c.getMaterialID().empty()) {
                    BadgeText("  None  ", { 0.45f,0.20f,0.20f,1.f });
                    // DRAG AND DROP TARGET
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MATERIAL"))
                        {
                            std::string payloadMaterial((const char*)payload->Data, payload->DataSize);
                            while (!payloadMaterial.empty() && payloadMaterial.back() == '\0') {
                                payloadMaterial.pop_back();
                            }

                            std::filesystem::path materialPath;
                            std::string materialKey;
                            if (ResolveMaterialAssetPath(payloadMaterial, materialPath, materialKey))
                            {
                                if (!AssetManager::instance().getMaterialMetadata(materialKey))
                                {
                                    AssetManager::instance().importMaterialMetadataFromJson(materialPath.string(), materialKey);
                                }

                                if (AssetManager::instance().createMaterialFromMetadata(materialKey))
                                {
                                    c.setMaterialID(materialKey);
                                }
                            }
                            else
                            {
                                std::cerr << "[SceneHierarchyPanel] Unable to resolve material asset '" << payloadMaterial << "'" << std::endl;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    return;
                }
                BadgeText(c.getMaterialID().c_str(), {0.22f,0.45f,0.65f,1.f});
                // DRAG AND DROP TARGET
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MATERIAL"))
                    {
                        std::string payloadMaterial((const char*)payload->Data, payload->DataSize);
                        while (!payloadMaterial.empty() && payloadMaterial.back() == '\0') {
                            payloadMaterial.pop_back();
                        }

                        std::filesystem::path materialPath;
                        std::string materialKey;
                        if (ResolveMaterialAssetPath(payloadMaterial, materialPath, materialKey))
                        {
                            std::cout << "[SceneHierarchyPanel] Resolved material asset '" << payloadMaterial << "' to '" << materialPath.string() << "' with key '" << materialKey << "'" << std::endl;
                            if (!AssetManager::instance().getMaterialMetadata(materialKey))
                            {
                                AssetManager::instance().importMaterialMetadataFromJson(materialPath.string(), materialKey);
                            }

                            if (AssetManager::instance().createMaterialFromMetadata(materialKey))
                            {
                                c.setMaterialID(materialKey);
                            }
                        }
                        else
                        {
                            std::cerr << "[SceneHierarchyPanel] Unable to resolve material asset '" << payloadMaterial << "'" << std::endl;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Spacing();
                ImGui::SetNextItemWidth(-1.f);
                if (ImGui::BeginCombo("##MaterialValues", "\xef\x83\xa6  Material Properties"))
                {
                    auto  mat = AssetManager::instance().getMaterialPtr(c.getMaterialID());
                    auto& uniforms = mat->getUniforms();
                    for (auto& [name, value] : uniforms)
                    {
                        ImGui::PushID(name.c_str());
                        //ImGui::SetNextItemWidth(-1.f);
                        if (auto* p = std::get_if<int>(&value))
                            ImGui::DragInt(name.c_str(), p);
                        else if (auto* p = std::get_if<float>(&value))
                            ImGui::DragFloat(name.c_str(), p, 0.01f);
                        else if (auto* p = std::get_if<glm::vec2>(&value))
                            ImGui::DragFloat2(name.c_str(), glm::value_ptr(*p), 0.01f);
                        else if (auto* p = std::get_if<glm::vec3>(&value))
                            ImGui::ColorEdit3(name.c_str(), glm::value_ptr(*p));
                        else if (auto* p = std::get_if<glm::vec4>(&value))
                            ImGui::ColorEdit4(name.c_str(), glm::value_ptr(*p));
                        else if (std::get_if<glm::mat3>(&value))
                            ImGui::TextColored(Theme::TextDim, "%s  (mat3)", name.c_str());
                        else if (std::get_if<glm::mat4>(&value))
                            ImGui::TextColored(Theme::TextDim, "%s  (mat4)", name.c_str());
                        else if (std::get_if<std::shared_ptr<Texture>>(&value))
                        {
                            ImGui::TextColored(Theme::TextDim, "Texture");
                            ImGui::SameLine();
                            ImGui::Button(name.c_str(), { 180.f, 0.f });
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndCombo();
                }
            }, { 0.80f, 0.55f, 0.25f, 1.f });

        // ── Scripts ────────────────────────────────────────────────────────────
        if (entity.hasComponent<ScriptingSystem>())
        {
            auto& ss = entity.getComponent<ScriptingSystem>();
            std::shared_ptr<Component> scriptToRemove;
            for (const auto& script : ss.scripts)
            {
                const char* typeName = typeid(*script).name();

                ImGui::Spacing();
                float nodeH2 = ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y * 2.f;
                DrawAccentBar(nodeH2, { 0.80f, 0.45f, 0.85f, 1.f });

                ImGui::PushStyleColor(ImGuiCol_Header, { 0.18f,0.18f,0.23f,1.f });
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.22f,0.22f,0.28f,1.f });
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.f, 5.f });

                const ImGuiTreeNodeFlags sf =
                    ImGuiTreeNodeFlags_DefaultOpen |
                    ImGuiTreeNodeFlags_Framed |
                    ImGuiTreeNodeFlags_SpanAvailWidth |
                    ImGuiTreeNodeFlags_FramePadding;

                bool sOpen = ImGui::TreeNodeEx(
                    (void*)script.get(), sf, "\xef\x84\x9b  %s", typeName);

                ImGui::PopStyleVar();
                ImGui::PopStyleColor(2);

                if (ImGui::BeginPopupContextItem())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, Theme::Danger);
                    if (ImGui::MenuItem("\xef\x87\x97  Remove Script")) {
                        scriptToRemove = script;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopStyleColor();
                    ImGui::EndPopup();
                }
                if (sOpen) ImGui::TreePop();
            }

            if (scriptToRemove)
            {
                if (dynamic_cast<BoxCollider*>(scriptToRemove.get()))
                    scriptToRemove->obj->removeComponent<BoxCollider>();
                else if (dynamic_cast<CircleCollider*>(scriptToRemove.get()))
                    scriptToRemove->obj->removeComponent<CircleCollider>();
                else
                    ss.scripts.erase(
                        std::remove(ss.scripts.begin(), ss.scripts.end(), scriptToRemove),
                        ss.scripts.end());
            }
        }
    }

    // ─── Helpers ────────────────────────────────────────────────────────────────
    template<typename T>
    void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
    {
        if (!m_SelectionContext.hasComponent<T>())
            if (ImGui::MenuItem(entryName.c_str())) {
                m_SelectionContext.addComponent<T>();
                ImGui::CloseCurrentPopup();
            }
    }

} // namespace OnYuu