#include "Engine.h"
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
	
}

void Scene::calculateCollisions(float dt)
{
	physicsEngine.update(dt);
}

Scene::Scene()
{
	editorCamera = new Orthographic(-1,1,-1,1,-1.0,1);
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

	std::shared_ptr<Mesh> pavimentoMesh = std::make_shared<Mesh>(Mesh{
		std::vector<glm::vec3>{
			{-1, -1, 0},
			{1,-1,0 },
			{1,1,0 },
			{-1,1,0 }
		},
		std::vector<glm::vec4>{
			{ 0.5,0.38,0.05,1  },
			{  0.5,0.38,0.05,1 },
			{  0.5,0.38,0.05,1 },
			{  0.5,0.38,0.05,1  }
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
	std::shared_ptr <Shader> colorSupporterShader = Shader::create("vertexShaderC.glsl", "colorUniformSupporterShader.glsl");
	std::shared_ptr<Shader> backgroundShader = Shader::create("vertexShaderC.glsl", "sfondoFragShader.glsl");
	AssetManager::instance().addMesh("squareMesh", squareMesh);
	AssetManager::instance().addMesh("pavimentoMesh", pavimentoMesh);
	AssetManager::instance().addMesh("hermiteMesh1", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
			{-0.849609,  0.803819,0},
			 { -0.722656, - 0.678819, 0},
			{ 0.0332031,  0.0711806 , 0},
			{- 0.646484, - 0.326389,  0 },
			{- 0.68457,  0.380208,  0},	
			{ -0.170898, - 0.0451389,0}

		})));

	AssetManager::instance().addMesh("enemy1Mesh", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
			{-0.414062,0.550347		,0},
			{0.615234 ,0.583333		,0},
			{0.638672 ,-0.763889	,0},
			{0.353516 ,-0.776042	,0},
			{0.431641 ,0.131944		,0},
			{- 0.414062,0.550347	,0}

		})));

	AssetManager::instance().addMesh("ballMesh", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
			{-0.00488281,-0.715278,0,},
			{ -0.0605469,-0.696181,0, }	,
			{ -0.185547,-0.560764,0, }	,
			{ -0.270508,-0.480903,0, }	,
			{ -0.335938,-0.418403,0, }	,
			{ -0.349609,-0.286458,0, }	,
			{ -0.368164,-0.121528,0, }	,
			{ -0.359375,0.0486111,0, }	,
			{ -0.28125,0.173611,0, }	,
			{ -0.136719,0.348958,0, }	,
			{ -0.0332031,0.432292,0, }	,
			{ 0.0371094,0.454861,0, }	,
			{ 0.160156,0.401042,0, }	,
			{ 0.276367,0.314236,0, }	,
			{ 0.351562,0.225694,0, }	,
			{ 0.438477,-0.100694,0, }	,
			{ 0.263672,-0.147569,0, }	,
			{ 0.280273,-0.421875,0, }	,
			{ 0.270508,-0.579861,0, }	,
			{ 0.15625,-0.692708,0, }	,
			{ 0.136719,-0.696181,0, }	,
			{ 0.0107422,-0.715278,0, }	,
			{ -0.00488281,-0.715278,0, }


		})));

	AssetManager::instance().addMesh("enemyMesh2", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
			{-0.0322266,-0.118056,0,}    ,
			{ -0.195312,-0.449653,0, }	 ,
			{ -0.186523,-0.102431,0, }	 ,
			{ -0.347656,-0.0902778,0, }	 ,
			{ -0.186523,0.130208,0, }	 ,
			{ -0.220703,0.420139,0, }	 ,
			{ -0.00585938,0.185764,0, }	 ,
			{ 0.161133,0.199653,0, }	 ,
			{ 0.0585938,0.0329861,0, }	 ,
			{ 0.261719,-0.15625,0, }	 ,
			{ -0.0322266,-0.118056,0, }



		})));
	AssetManager::instance().addMesh("triangoloMesh", triangoloMesh);
	AssetManager::instance().addMaterial("defaultMaterial", std::make_shared<Material>(shader));
	AssetManager::instance().addMaterial("backgroundMaterial", std::make_shared<Material>(backgroundShader));
	AssetManager::instance().addMaterial("colorUniformSupporterMaterial", std::make_shared<Material>(colorSupporterShader));

}

Scene::~Scene()
{
	free(reg);
	free(editorCamera);
}


