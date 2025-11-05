#pragma once
#include "Engine.h"
#include "GameScene.h"
#include "PlayerScripts.h"
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
		ImGui::Begin("Game Layer");
		ImGui::Text("FPS: %.1f", 1.0f / _deltaTime);
		ImGui::Text("Delta Time: %.4f seconds", _deltaTime);
		ImGui::End();
		ImGui::SetNextWindowPos(ImVec2(50, Application::getInstance()->getWindow()->getHeight() * 0.9)); // Imposta posizione finestra successiva (50, 1050)
		ImGui::Begin("Impostazioni Sfondo", NULL,  // Apre finestra "Impostazioni"
			ImGuiWindowFlags_NoResize |      // Impedisce ridimensionamento utente
			ImGuiWindowFlags_AlwaysAutoResize | // Ridimensionamento automatico al contenuto
			ImGuiWindowFlags_NoBackground |  // Sfondo trasparente
			ImGuiWindowFlags_NoTitleBar |    // Nasconde barra titolo
			ImGuiWindowFlags_NoMove           // Impedisce lo spostamento finestra
		);
		const int vita = gameScene->player.getComponent<PlayerScripts>().getVite();
		ImGui::Text("Punteggio");
		ImGui::Separator();
		ImGui::Text("vite: %d",vita) ; // Sostituisci 0 con variabile punteggio reale
		ImGui::End();
	};
private:
	Scene* scene = nullptr;
	float _deltaTime = 0.0f;
	bool showGameInfo = true;
};