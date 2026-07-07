#pragma once
#include "Core/Engine.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/ContentBrowsingPanel.h"
#include "EditorCamera.h"

namespace OnYuu {

class EditorLayer :
	public Layer
{
public:
	EditorLayer() : Layer("EditorLayer") {}
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(/*Event& event*/) override;
	virtual void onImGuiRender() override;
	void createMainMenu();
	virtual void onAttach() override;
	virtual void onDetach() override;

	std::shared_ptr<RenderTarget> getRenderTarget() const { return m_renderTarget; }
private:
    // ========================================================================
// MOVING AVERAGE - Frame History per Stats Smoothing
// ========================================================================

// Dati di un singolo frame
    struct FrameData {
        float fps = 0.0f;
        float frameTimeMs = 0.0f;
        uint32_t indirectDrawCalls = 0;
        uint32_t totalBatches = 0;
    };

    // Configurazione moving average
    static constexpr int FRAME_HISTORY_SIZE = 5000;  // 1 secondo @ 60Hz

    // Array circolare per la storia dei frame
    std::array<FrameData, FRAME_HISTORY_SIZE> frameHistory_{};

    // Indice corrente per il circular buffer
    int frameHistoryIndex_ = 0;

    // Helper functions
    void updateStatsHistory();      // Aggiungi frame corrente alla history
    FrameData getAverageStats() const;  // Calcola media degli ultimi FRAME_HISTORY_SIZE frame

private:
	std::shared_ptr<Scene> m_scene;
	EditorCamera m_editorCamera;
	std::shared_ptr<Perspective> m_perspectiveCamera;
	std::shared_ptr<RenderTarget> m_renderTarget;
	SceneHierarchyPanel m_SceneHierarchyPanel;
	ViewportPanel m_ViewportPanel;
	ContentBrowsingPanel m_ContentBrowsingPanel;
	friend class ViewportPanel;
};

}
