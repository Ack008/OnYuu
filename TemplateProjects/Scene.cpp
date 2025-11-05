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

	AssetManager::instance().addMesh("enemyMesh2", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
			{-0.574219,-0.638889,0,},
			{ -0.354492,-0.0885417,0, },
			{ -0.458008,0.463542,0, },
			{ -0.578125,0.03125,0, },
			{ -0.560547,-0.00694444,0, },
			{ -0.598633,0.0260417,0, },
			{ -0.603516,-0.015625,0, },
			{ -0.624023,0.0381944,0, },
			{ -0.664062,0.00520833,0, },
			{ -0.638672,0.0868056,0, },
			{ -0.620117,0.118056,0, },
			{ -0.537109,0.508681,0, },
			{ -0.474609,0.567708,0, },
			{ -0.412109,0.581597,0, },
			{ -0.433594,0.736111,0, },
			{ -0.423828,0.829861,0, },
			{ -0.397461,0.866319,0, },
			{ -0.354492,0.881944,0, },
			{ -0.313477,0.883681,0, },
			{ -0.291016,0.871528,0, },
			{ -0.288086,0.868056,0, },
			{ -0.238281,0.828125,0, },
			{ -0.207031,0.782986,0, },
			{ -0.19043,0.701389,0, } ,
			{ -0.208008,0.644097,0, },
			{ -0.253906,0.609375,0, },
			{ -0.291016,0.623264,0, },
			{ -0.263672,0.550347,0, },
			{ -0.224609,0.538194,0, },
			{ -0.186523,0.552083,0, },
			{ -0.151367,0.574653,0, },
			{ -0.00292969,0.376736,0, },
			{ 0.0869141,0.229167,0, }  ,
			{ 0.115234,0.258681,0, }   ,
			{ 0.141602,0.199653,0, }   ,
			{ 0.108398,0.225694,0, }   ,
			{ 0.131836,0.185764,0, }   ,
			{ 0.117188,0.173611,0, }   ,
			{ 0.0957031,0.208333,0, }  ,
			{ 0.0996094,0.163194,0, }  ,
			{ 0.100586,0.119792,0, }   ,
			{ 0.0742188,0.196181,0, }  ,
			{ 0.0351562,0.204861,0, }  ,
			{ -0.160156,0.491319,0, }  ,
			{ -0.260742,0.486111,0, }  ,
			{ -0.166992,0.015625,0, }  ,
			{ 0.0439453,-0.390625,0, } ,
			{ -0.0605469,-0.578125,0, },
			{ -0.204102,-0.168403,0, } ,
			{ -0.275391,-0.164931,0, } ,
			{ -0.423828,-0.746528,0, } ,
			{ -0.579102,-0.647569,0, }


		})));
	AssetManager::instance().addMesh("triangoloMesh", triangoloMesh);
	AssetManager::instance().addMaterial("defaultMaterial", std::make_shared<Material>(shader));
	AssetManager::instance().addMaterial("backgroundMaterial", std::make_shared<Material>(backgroundShader));

}

Scene::~Scene()
{
	free(reg);
	free(editorCamera);
}


