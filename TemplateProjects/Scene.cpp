#include <iostream>
#include "Scene.h"
#include "MeshComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Transform.h"
#include "TreeComponent.h"
#include "Renderer.h"
#include "ScriptingSystem.h"
#include "AssetManager.h"
GameObject Scene::createEntity()
{
	entt::entity id = reg->create();
	reg->emplace<Trasform>(id);
	reg->emplace<TreeComponent>(id);
	return { id,this };
}
Trasform getAbsoluteTrasform(TreeComponent parent) {
	Trasform tras;
	if (!parent.father) {
		tras.position = { 0 ,0,0 };
		tras.rotation = { 0,0,0 };
		tras.scale = { 1,1,1 };
	}
	else {
		tras += getAbsoluteTrasform(parent.father->getComponent<TreeComponent>());
	}
	
	return tras;
}
void Scene::update(float dt)
{
	//scripts
	auto scriptsView = reg->view<ScriptingSystem>();
	for (auto [entity, script] : scriptsView.each()) {
		script.update(dt);
	}

	
	auto cameraView = reg->view<Orthographic>();
	for (auto [entity, camera] : cameraView.each()) {
		if (camera.getActive()) {
			Render::getInstance()->setCameraMatrix(camera.getVPMatrix());
		}
		break; // Use the first found camera
	}
	calculateCollisions(dt);
	sendToRender();
	Render::getInstance()->draw();
	Render::getInstance()->clear();
	if (!toDestroy.empty()) {
		destroyEntities();
	}
	toDestroy.clear();
}
void Scene::sendToRender(){
	auto meshView = reg->view<RenderMeshComponent, Trasform, TreeComponent>();
	for (auto [entity, meshComp, transform, treeComp] : meshView.each()) {
		Trasform absoluteTransform = transform;
		if (treeComp.father) {
			absoluteTransform += getAbsoluteTrasform(treeComp.father->getComponent<TreeComponent>());
		}
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, absoluteTransform.position);
		model = glm::rotate(model, absoluteTransform.rotation.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, absoluteTransform.rotation.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, absoluteTransform.rotation.z, glm::vec3(0, 0, 1));
		model = glm::scale(model, absoluteTransform.scale);
		Render::getInstance()->addMeshRender(&meshComp, model);
	}
}
void Scene::destroyEntities()
{
	for (int i = 0; i < toDestroy.size(); i++) {
		auto obj = toDestroy[i];
		if (obj->hasComponent<ScriptingSystem>()) {
			auto& scriptList = obj->getComponent<ScriptingSystem>();
			for (auto& script : scriptList.scripts) {
				auto comp = script.get();
				componentsList.erase(std::remove(componentsList.begin(), componentsList.end(), comp), componentsList.end());
				if (auto collider = dynamic_cast<Collider*>(comp)) {
					this->physicsEngine.removeCollider(collider);
				}
			}
		}
			
		this->reg->destroy(obj->id);
	}
}
void Scene::start()
{
	
}

void Scene::calculateCollisions(float dt)
{
	physicsEngine.update(dt);
}

Scene::Scene()
{
	std::shared_ptr<Mesh> squareMesh = std::make_shared<Mesh>(Mesh{
		std::vector<glm::vec3>{
			{-1, -1, 0},
			{1,-1,0 },
			{1,1,0 },
			{-1,1,0 }
		},
		std::vector<glm::vec4>{
			{ 1,1,1,1  },
			{ 1,1,1,1 },
			{ 1,1,1,1 },
			{ 1,1,1,1 }
		},
		std::vector<uint32_t>{
			0,1,2,
			2,3,0
		}
		});
	std::shared_ptr<Mesh> triangoloMesh = std::make_shared<Mesh>(Mesh{
		std::vector<glm::vec3>{
			{-1.0, -1.0, 0},
			{-1.0,1,0 },
			{1.0,0,0 }
		},
		std::vector<glm::vec4>{
			{ 1,0,0,1  },
			{ 0,1,0,1 },
			{ 0,0,1,1 },
		}
		});
	std::shared_ptr <Shader> shader = Shader::create("vertexShaderC.glsl", "fragmentShaderC.glsl");
	AssetManager::instance().addMesh("squareMesh", squareMesh);
	AssetManager::instance().addMesh("triangoloMesh", triangoloMesh);
	AssetManager::instance().addMaterial("defaultMaterial", std::make_shared<Material>(shader));
}

Scene::~Scene()
{
	free(reg);
}


