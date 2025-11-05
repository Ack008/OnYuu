#pragma once
#include "Engine.h"
#include "GameScene.h"
#include "ControllerScript.h"
class GameLayer : public Layer {
public:
	GameLayer(Scene* scene) : Layer("Game Layer"), scene(scene) {};
	// Ereditato tramite Layer
	void onAttach() override {};
	void onDetach() override {};
	void onUpdate(float(deltaTime)) override {
		scene->update(deltaTime);
		_deltaTime = deltaTime;
	};
	void onEvent() override {};
	void onImGuiRender() override {
		GameScene* gameScene = dynamic_cast<GameScene*>(scene);
		
		ImGui::SetNextWindowPos(ImVec2(50, Application::getInstance()->getWindow()->getHeight() * 0.9)); // Imposta posizione finestra successiva (50, 1050)
		ImGui::Begin("Impostazioni Sfondo", NULL,  // Apre finestra "Impostazioni"
			ImGuiWindowFlags_NoResize |      // Impedisce ridimensionamento utente
			ImGuiWindowFlags_AlwaysAutoResize | // Ridimensionamento automatico al contenuto
			ImGuiWindowFlags_NoBackground |  // Sfondo trasparente
			ImGuiWindowFlags_NoTitleBar |    // Nasconde barra titolo
			ImGuiWindowFlags_NoMove           // Impedisce lo spostamento finestra
		);
		const int count = gameScene->controller.getComponent<ControllerScript>().getEnemyCount();
		bool victory = gameScene->controller.getComponent<ControllerScript>().getVictory();
		if (victory) {
			ImGui::SetNextWindowPos(ImVec2(Application::getInstance()->getWindow()->getWidth() * 0.3, Application::getInstance()->getWindow()->getHeight() * 0.5));
			ImGui::Begin("Vittoria", NULL,  // Apre finestra "Impostazioni"
				ImGuiWindowFlags_NoResize |      // Impedisce ridimensionamento utente
				ImGuiWindowFlags_AlwaysAutoResize | // Ridimensionamento automatico al contenuto
				ImGuiWindowFlags_NoBackground |  // Sfondo trasparente
				ImGuiWindowFlags_NoTitleBar |    // Nasconde barra titolo
				ImGuiWindowFlags_NoMove           // Impedisce lo spostamento finestra
			);
			ImGui::Text("Hai vinto! Premi ENTER per ricominciare.");
			ImGui::End();
		}
		ImGui::Text("Punteggio");
		ImGui::Separator();
		ImGui::Text("Nemici in piedi: %d",count) ; // Sostituisci 0 con variabile punteggio reale
		ImGui::End();
	};
private:
	Scene* scene = nullptr;
	float _deltaTime = 0.0f;
	bool showGameInfo = true;
};