#pragma once
#include "Core/Engine.h"
#include "Scene3D.h"
#include "scripts/Controller.h"
#include <queue>
#include <list>
#include <iostream>

class Game3DLayer : public Layer {
private:
	enum class MeshAddingState {
		None,
		AddingMesh,
		WaitingForMaterialSelection
	};
	MeshAddingState meshAddingState = MeshAddingState::None;
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
			ImGui::DragFloat3("##Position", &transform.position[0],0.05);
			ImGui::Text("Rotation");
			ImGui::DragFloat3("##Rotation", &transform.rotation[0],0.05);
			ImGui::Text("Scale");
			ImGui::DragFloat3("##Scale", &transform.scale[0],0.05);
			if (selectedObj.hasComponent<LightComponent>()) {
				LightComponent& lightComp = selectedObj.getComponent<LightComponent>();
				ImGui::Text("Light Color");
				ImGui::SliderFloat3("##LightColor", glm::value_ptr(lightComp.color),0,1);
				ImGui::Text("Light Intensity");
				ImGui::SliderFloat("##LightIntensity", &lightComp.intensity,0,1);
			}
			if (selectedObj.hasComponent<RenderMeshComponent>()) {
				RenderMeshComponent& renderComp = selectedObj.getComponent<RenderMeshComponent>();
				AssetManager& assetManager = AssetManager::instance();
				if(ImGui::TreeNode("All Materials"))
					{
					const auto& materials = assetManager.getMaterials();
					for (const auto& [name, matPtr] : materials) {
                        if (matPtr == renderComp.material 
							&& ImGui::TreeNodeEx((name + " selected").c_str(), 
								ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth)) {

							// Otteniamo gli uniform come riferimento modificabile per poterli editare.
							for (auto& [uniformName, uniformValue] : matPtr->getUniforms()) {
								// Etichette ImGui uniche usando nome uniform + nome materiale
								std::string labelBase = "Uniform: " + uniformName;
								ImGui::Text("%s", labelBase.c_str());
								std::string widgetLabel = uniformName + "##" + name;

								// Intero
								if (std::holds_alternative<int>(uniformValue)) {
									int& value = std::get<int>(uniformValue);
									ImGui::InputInt(widgetLabel.c_str(), &value);
									// eventualmente aggiungere limiti o DragInt se preferito
								}
								// Float (modificabile)
								else if (std::holds_alternative<float>(uniformValue)) {
									float& value = std::get<float>(uniformValue);
									ImGui::DragFloat(widgetLabel.c_str(), &value, 0.01f);
								}
								// vec2 (modificabile)
								else if (std::holds_alternative<glm::vec2>(uniformValue)) {
									glm::vec2& value = std::get<glm::vec2>(uniformValue);
									ImGui::DragFloat2(widgetLabel.c_str(), glm::value_ptr(value), 0.01f);
								}
								// vec3 (modificabile)
								else if (std::holds_alternative<glm::vec3>(uniformValue)) {
									glm::vec3& value = std::get<glm::vec3>(uniformValue);
									ImGui::DragFloat3(widgetLabel.c_str(), glm::value_ptr(value), 0.01f);
								}
								// vec4 (modificabile)
								else if (std::holds_alternative<glm::vec4>(uniformValue)) {
									glm::vec4& value = std::get<glm::vec4>(uniformValue);
									ImGui::DragFloat4(widgetLabel.c_str(), glm::value_ptr(value), 0.01f);
								}
								// mat3 (non editabile, solo visualizzazione)
								else if (std::holds_alternative<glm::mat3>(uniformValue)) {
									ImGui::Text("Value (mat3): [matrix]");
								}
								// mat4 (non editabile, solo visualizzazione)
								else if (std::holds_alternative<glm::mat4>(uniformValue)) {
									ImGui::Text("Value (mat4): [matrix]");
								}
								// Texture* (non editabile qui, mostra immagine e puntatore)
								else if (std::holds_alternative<Texture*>(uniformValue)) {
									Texture* tex = std::get<Texture*>(uniformValue);
									ImGui::Text("Value (Texture*): %p", static_cast<void*>(tex));
									if (tex)
										ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(tex->getID())), ImVec2(64, 64));
								}
							}
							ImGui::TreePop();
                        }else if (ImGui::Selectable(name.c_str())) {
                            renderComp.material = matPtr;
						}
					}
					ImGui::TreePop();
				}

			}
			ImGui::End();
		}
		createScenePanel();
	}
	void addMeshPanel()
	{
		static std::string filePath = "";
		if (meshAddingState == MeshAddingState::None && ImGui::Button("Add Mesh") )
		{
			meshAddingState = MeshAddingState::AddingMesh;
			ImGuiFileDialog::Instance()->OpenDialog(
				"ChooseMeshDlgKey",
				"Choose Mesh",
				"Mesh Files (*.obj *.fbx *.glb *.gltf){.obj,.fbx,.glb,.gltf}"
			);
		}

		if (ImGuiFileDialog::Instance()->Display("ChooseMeshDlgKey"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				filePath = ImGuiFileDialog::Instance()->GetFilePathName();
				std::cout << filePath << std::endl;
				meshAddingState = MeshAddingState::WaitingForMaterialSelection;
				ImGuiFileDialog::Instance()->Close();
			}
		}
		if (meshAddingState == MeshAddingState::WaitingForMaterialSelection)
		{
			ImGui::Begin("Select Material for New Mesh");
			AssetManager& assetManager = AssetManager::instance();
			const auto& materials = assetManager.getMaterials();
			for (const auto& [name, matPtr] : materials) {
				if (ImGui::Selectable(name.c_str())) {
					MeshImporter::instance().importMesh(filePath, scene, matPtr->getShader());
					meshAddingState = MeshAddingState::None;
					ImGui::End();
					return;
				}
			}
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
	void printGameObjectHierarchy(std::queue<GameObject> *queue ,std::list<GameObject> *visited, GameObject& obj, int depth = 0) {
		if (visited->end() != std::find(visited->begin(), visited->end(), obj)) {
			return;
		}
		visited->push_back(obj);
		std::string indent(depth * 2, ' ');
		std::string idStr = std::to_string(static_cast<uint32_t>(obj.getID()));
		const std::string label = indent + obj.getComponent<TagComponent>().tag + " " + idStr;
		bool nodeOpen = ImGui::TreeNodeEx(label.c_str(),
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth);
		// Se l'header è stato cliccato (label o altro nell'area header), seleziona l'oggetto.
		// Questo cattura i click indipendentemente dallo stato aperto/chiuso del nodo.
		if (ImGui::IsItemClicked(0))
			scene->getController().getComponent<Controller>().setSelectedObject(obj);

		if (nodeOpen)
		{
			auto& treeComp = obj.getComponent<TreeComponent>();
			for (auto& childPtr : treeComp.obj) {
					printGameObjectHierarchy(queue, visited, childPtr, depth + 1);
			}
			ImGui::TreePop();

		}
	}
	void createScenePanel() 
	{
		std::list<GameObject> visitedObject;
		std::queue<GameObject> toVisit;
		ImGui::Begin("Scene 3D Info");
		ImGui::Text("3D Scene Active");
		
		for (auto& obj : scene->getGameObjects()) {
			toVisit.push(obj);
		}
		while (!toVisit.empty()) {
			GameObject currentObj = toVisit.front();
			toVisit.pop();
			ImGui::Separator();
			if (currentObj.getComponent<TreeComponent>().father)
				continue;
			printGameObjectHierarchy(&toVisit, &visitedObject, currentObj, 0);
			visitedObject.push_back(currentObj);
		}
		addMeshPanel();

		ImGui::End();
	}
private:
	Scene3D* scene;
};