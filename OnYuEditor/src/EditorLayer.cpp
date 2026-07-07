#include "EditorLayer.h"

namespace OnYuu {

	static const char* rendererToString(API api)
	{
		switch (api)
		{
		case API::OpenGL: return "OpenGL";
		case API::DirectX11: return "DirectX11";
		case API::DirectX12: return "DirectX12";
		case API::Vulkan: return "Vulkan";
		case API::Metal: return "Metal";
		default: return "None";
		}
	}

	static bool isRendererSupported(API api)
	{
		return api == API::OpenGL || api == API::Vulkan;
	}

	// ============================================================================
	// MOVING AVERAGE IMPLEMENTATION
	// ============================================================================

	void EditorLayer::updateStatsHistory()
	{
		// Cattura i dati del frame corrente
		FrameData currentFrame{
			.fps = ImGui::GetIO().Framerate,
			.frameTimeMs = 1000.0f / ImGui::GetIO().Framerate,
			.indirectDrawCalls = Render::getStats().indirectDrawCalls,
			.totalBatches = Render::getStats().totalBatches
		};

		// Inserisci nel circular buffer
		frameHistory_[frameHistoryIndex_] = currentFrame;

		// Avanza l'indice (wrap around)
		frameHistoryIndex_ = (frameHistoryIndex_ + 1) % FRAME_HISTORY_SIZE;
	}

	EditorLayer::FrameData EditorLayer::getAverageStats() const
	{
		FrameData average{};

		// Somma tutti i frame della history
		for (const auto& frame : frameHistory_) {
			average.fps += frame.fps;
			average.frameTimeMs += frame.frameTimeMs;
			average.indirectDrawCalls += frame.indirectDrawCalls;
			average.totalBatches += frame.totalBatches;
		}

		// Dividi per ottenere la media
		average.fps /= FRAME_HISTORY_SIZE;
		average.frameTimeMs /= FRAME_HISTORY_SIZE;
		average.indirectDrawCalls /= FRAME_HISTORY_SIZE;
		average.totalBatches /= FRAME_HISTORY_SIZE;

		return average;
	}

	// ============================================================================
	// MAIN LAYER IMPLEMENTATION
	// ============================================================================

	void EditorLayer::onUpdate(float deltaTime)
	{
		m_scene->update(deltaTime);
		std::shared_ptr<Camera> activeCamera = m_editorCamera.getCamera();
		m_scene->render(activeCamera.get(), m_renderTarget);
		m_ViewportPanel.update(deltaTime);

		// ⭐ MOVING AVERAGE: Accumula i dati ogni frame
		updateStatsHistory();
	}

	void EditorLayer::onEvent()
	{
	}

	void EditorLayer::onImGuiRender()
	{
		createMainMenu();

		// ⭐ MOVING AVERAGE: Calcola la media degli ultimi 60 frame
		auto avgStats = getAverageStats();

		ImGui::Begin("Stats");
		ImGui::Text("Renderer: %s", rendererToString(Render::getAPI()));
		ImGui::Text("FPS: %.1f", avgStats.fps);                          // ← MEDIA SMOOTH
		ImGui::Text("Frame Time: %.3f ms", avgStats.frameTimeMs);        // ← MEDIA SMOOTH
		ImGui::Text("Indirect Draw Calls: %u", (uint32_t)avgStats.indirectDrawCalls);  // ← MEDIA SMOOTH
		ImGui::Text("Total Batches: %u", (uint32_t)avgStats.totalBatches);             // ← MEDIA SMOOTH
		ImGui::Separator();
		ImGui::End();

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// Fullscreen dockspace: practically the same as calling DockSpaceOverViewport();
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Window with a DockSpace", nullptr, window_flags);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Submit the DockSpace widget inside our window
		// - Note that the id here is different from the one used by DockSpaceOverViewport(), so docking state won't get transfered between "Basic" and "Advanced" demos.
		// - If we made the ShowExampleAppDockSpaceBasic() calculate its own ID and pass it to DockSpaceOverViewport() the ID could easily match.
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		ImGui::End();

		m_ViewportPanel.onImGuiRender();
		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowsingPanel.OnImGuiRender();
	}

	void EditorLayer::createMainMenu()
	{
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Renderer")) {
				if (ImGui::BeginMenu("Seleziona renderer")) {
					ImGui::BeginChild("##RendererSelectionScroll", ImVec2(220.0f, 130.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

					const API renderers[] = { API::OpenGL, API::DirectX11, API::DirectX12, API::Vulkan, API::Metal };
					const API currentApi = Render::getAPI();

					for (API selectedApi : renderers) {
						const bool isSelected = selectedApi == currentApi;
						if (ImGui::Selectable(rendererToString(selectedApi), isSelected)) {
							API targetApi = selectedApi;
							if (!isRendererSupported(selectedApi)) {
								targetApi = (currentApi == API::OpenGL) ? API::Vulkan : API::OpenGL;
								if (!isRendererSupported(targetApi)) {
									targetApi = API::OpenGL;
								}
							}

							if (targetApi != currentApi) {
								Application::requestRendererChange(targetApi);
							}
						}
					}

					ImGui::EndChild();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void EditorLayer::onAttach()
	{
		m_scene = std::make_shared<Scene>();
		m_perspectiveCamera = std::make_shared<Perspective>(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
		m_renderTarget = RenderTarget::create(2560, 1400);
		m_SceneHierarchyPanel.SetContext(m_scene);
		m_ViewportPanel.setLayer(this);
	}

	void EditorLayer::onDetach()
	{
	}

}