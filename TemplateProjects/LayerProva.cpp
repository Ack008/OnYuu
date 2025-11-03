#include "LayerProva.h"
#include <iostream>
#include <ImGui/imgui.h>
void LayerProva::onEvent()
{
}

void LayerProva::onImGuiRender()
{
	ImGui::Begin("Layer Prova");
	ImGui::Text("This is the Layer Prova window.");
	ImGui::End();
}

void LayerProva::onAttach()
{
	std::cout << "LayerProva attached!" << std::endl;
	
}

void LayerProva::onDetach()
{
	delete currentScene;
	std::cout << "LayerProva detached!" << std::endl;
}
void LayerProva::onUpdate(float deltaTime)
{
	currentScene->update(deltaTime);
}
