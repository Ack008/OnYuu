#include "Core/Engine.h"
#include <iostream>
#include "Components/SkyBoxComponent.h"
namespace OnYuu {
	GameObject Scene::createEntity()
	{
		entt::entity id = reg->create();
		reg->emplace<TagComponent>(id, "GameObject");
		reg->emplace<Trasform>(id);
		reg->emplace<TreeComponent>(id);
		return { id,this };
	}

	void Scene::loadLights()
	{
		auto view = reg->view<LightComponent, Trasform>();

		LightUBO uboData;
		uboData.numLights = 0;

		for (auto [entity, lightComp, transform] : view.each()) {
			if (uboData.numLights >= 16) break;

			LightData& l = uboData.lights[(int)uboData.numLights];

			l.position = glm::vec4(transform.position, 1.0f);
			l.color = glm::vec4(lightComp.color.r, lightComp.color.g, lightComp.color.b, 1.0f);
			l.intensity = lightComp.intensity;
			l.pad[0] = l.pad[1] = l.pad[2] = 0.0f;

			uboData.numLights++;
		}

		lightsUBO->bind();
		lightsUBO->resize(sizeof(LightUBO));
		lightsUBO->updateData(&uboData, sizeof(LightUBO), 0);

	}

	void Scene::loadActiveCamera()
	{
		CameraUBO camData;
		if (activeCamera) {
			camData.view = activeCamera->getViewMatrix();
			camData.projection = activeCamera->getProjectionMatrix();
			camData.position = glm::vec4(activeCamera->getPosition(), 1.0f);
		}
		else {
			camData.view = editorCamera->getViewMatrix();
			camData.projection = editorCamera->getProjectionMatrix();
			camData.position = glm::vec4(editorCamera->getPosition(), 1.0f);
		}
		cameraUBO->bind();
		cameraUBO->resize(sizeof(CameraUBO));
		cameraUBO->updateData(&camData, sizeof(CameraUBO), 0);
	}

	void Scene::update(float dt)
	{
		//caricare le luci
		loadLights();
		//instantiating prefabs
		instantiatePrefabs();
		// carica la camera attiva
		//scripts
		auto scriptsView = reg->view<ScriptingSystem>();
		for (auto [entity, script] : scriptsView.each()) {
			script.update(dt);
		}
		//rendering skybox

		//rendering background
		auto backgroundView = reg->view<Background2DRender>();
		Render::getInstance()->setCameraMatrix(editorCamera->getVPMatrix());
		Render::getInstance()->BeginScene(editorCamera);
		for (auto [entity, background] : backgroundView.each()) {
			RenderMeshComponent backgroundMeshComp;
			backgroundMeshComp.mesh = AssetManager::instance().getMeshPtr("squareMesh");
			backgroundMeshComp.material = background.material;
			Render::getInstance()->addMeshRender(&backgroundMeshComp, glm::mat4(1.0f));
			break;
		}
		Render::getInstance()->EndScene();
		//rendering scene cameras
		bool cameraFound = false;
		auto cameraView = reg->view<Orthographic>();
		for (auto [entity, camera] : cameraView.each()) {
			if (camera.getActive()) {
				Render::getInstance()->setCameraMatrix(camera.getVPMatrix());
				Render::getInstance()->setCamera(&camera);
				activeCamera = &camera;
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
					Render::getInstance()->setCamera(&camera);
					activeCamera = &camera;
					cameraFound = true;
				}
				break; // Use the first found camera
			}
		}
		if (!cameraFound) {
			Render::getInstance()->setCameraMatrix(editorCamera->getVPMatrix());
			Render::getInstance()->setCamera(editorCamera);
			activeCamera = editorCamera;
		}

		loadActiveCamera();
		Render::getInstance()->BeginScene(activeCamera);
		auto skyboxView = reg->view<SkyBoxComponent>();
		for (auto [entity, skybox] : skyboxView.each()) {
			Render::getInstance()->setCameraMatrix(editorCamera->getVPMatrix());
			Render::getInstance()->setSkyBox(&skybox);
			break;
		}


		calculateCollisions(dt);
		sendToRender();
		Render::getInstance()->EndScene();
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
	void Scene::sendToRender() {
		auto meshView = reg->view<RenderMeshComponent, Trasform, TreeComponent>();
		for (auto [entity, meshComp, transform, treeComp] : meshView.each()) {
			Trasform absoluteTransform = GameObject(entity, this).getAbsoluteTransform();
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
		lightsUBO = UniformBuffer::create(1);
		cameraUBO = UniformBuffer::create(2);
		initializeMaterials();
		initializeScene();

	}

	std::vector<GameObject> Scene::getGameObjects()
	{
		std::vector< GameObject > results;
		for (auto entity : reg->view<entt::entity>())
		{
			results.push_back(GameObject(entity, this));
		}

		return results;
	}

	void Scene::calculateCollisions(float dt)
	{
		physicsEngine.update(dt);
	}



	Scene::Scene()
	{
		editorCamera = new Orthographic(-1, 1, -1, 1, -1.0, 1);

	}

	Scene::~Scene()
	{
		free(reg);
		free(editorCamera);
	}

} // namespace OnYuu
