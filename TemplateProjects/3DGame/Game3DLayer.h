#pragma once
#include "Core/Engine.h"
#include "Scene3D.h"
#include "scripts/Controller.h"
class Game3DLayer : public Layer {
	public:
	Game3DLayer(Scene3D* scene)
		:Layer("Game3DLayer"), scene(scene)
	{}
	virtual void onUpdate(float dt) override {
		scene->update(dt);
	}
	virtual void onImGuiRender() override {
		Controller& controllerComp = scene->getController().getComponent<Controller>();
		GameObject selectedObj = controllerComp.getSelectedObject();
		if (selectedObj) {
			ImGui::Begin("Selected Object Properties");
			ImGui::Text(("Selected Object tag: " + selectedObj.getComponent<TagComponent>().tag).c_str());
			Trasform& transform = selectedObj.getComponent<Trasform>();
			ImGui::Text("Position");
			ImGui::InputFloat3("##Position", &transform.position[0]);
			ImGui::Text("Rotation");
			ImGui::InputFloat3("##Rotation", &transform.rotation[0]);
			ImGui::Text("Scale");
			ImGui::InputFloat3("##Scale", &transform.scale[0]);
			ImGui::End();
		}
	}
	virtual void onAttach() override {
	}
	virtual void onDetach() override {
	}
	virtual void onEvent(/*Event& event*/) override {
	}
private:
	Scene3D* scene;
};