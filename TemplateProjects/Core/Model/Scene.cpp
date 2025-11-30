#include "Core/Engine.h"
GameObject Scene::createEntity()
{
	entt::entity id = reg->create();
	reg->emplace<TagComponent>(id, "GameObject");
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
	//instantiating prefabs
	instantiatePrefabs();
	//scripts
	auto scriptsView = reg->view<ScriptingSystem>();
	for (auto [entity, script] : scriptsView.each()) {
		script.update(dt);
	}

	//rendering background
	auto backgroundView = reg->view<Background2DRender>();
	Render::getInstance()->setCameraMatrix(editorCamera->getVPMatrix());
	for (auto [entity, background] : backgroundView.each()) {
		RenderMeshComponent backgroundMeshComp;
		backgroundMeshComp.mesh = AssetManager::instance().getMesh("squareMesh");
		backgroundMeshComp.material = background.material;
		Render::getInstance()->addMeshRender(&backgroundMeshComp, glm::mat4(1.0f));
		break;
	}
	Render::getInstance()->draw();
	Render::getInstance()->clear();
	//rendering scene cameras
	bool cameraFound = false;
	auto cameraView = reg->view<Orthographic>();
	for (auto [entity, camera] : cameraView.each()) {
		if (camera.getActive()) {
			Render::getInstance()->setCameraMatrix(camera.getVPMatrix());
			cameraFound = true;
		}
		break; // Use the first found camera
	}
	if (!cameraFound) 
	{
		auto cameraViewPer = reg->view<Perspective>();
		for (auto [entity, camera] : cameraViewPer.each()) {
			if (camera.getActive()) {
				Render::getInstance()->setCameraMatrix(camera.getVPMatrix());
				cameraFound = true;
			}
			break; // Use the first found camera
		}
	}
	if (!cameraFound) {
		Render::getInstance()->setCameraMatrix(editorCamera->getVPMatrix());
	}

	calculateCollisions(dt);
	sendToRender();
	Render::getInstance()->draw();
	Render::getInstance()->clear();
	if (!toDestroy.empty()) {
		destroyEntities();
		toDestroy.clear();
	}
}
void Scene::instantiatePrefabs()
{
	if (toInstantiate.empty()) return;
	for (int i = 0; i < toInstantiate.size(); i++) {
		Prefab* prefab = toInstantiate[i];
		prefab->istantiateObject(this);
		free(prefab);
	}
	toInstantiate.clear();
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
				script->onDestroy();
			}
		}
	}
	for (int i = 0; i < toDestroy.size(); i++) {
		auto obj = toDestroy[i];
		if (obj->hasComponent<ScriptingSystem>()) {
			auto& scriptList = obj->getComponent<ScriptingSystem>();
			auto componentsList = scriptList.scripts;
			for (auto& script : scriptList.scripts) {
				auto comp = script.get();
				if (auto collider = dynamic_cast<Collider*>(comp)) {
					this->physicsEngine.removeCollider(collider);
				}
			}
			scriptList.scripts.clear();
		}
			
		this->reg->destroy(obj->id);
		delete obj;
	}
}

void Scene::start()
{
	initializeMaterials();
	initializeScene();
}

void Scene::calculateCollisions(float dt)
{
	physicsEngine.update(dt);
}



Scene::Scene()
{
	editorCamera = new Orthographic(-1,1,-1,1,-1.0,1);
	
}

Scene::~Scene()
{
	free(reg);
	free(editorCamera);
}


