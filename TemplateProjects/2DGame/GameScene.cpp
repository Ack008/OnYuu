#include "GameScene.h"
#include "Component/PlayerScripts.h"
#include "Component/ControllerScript.h"
#include "Component/BallScript.h"
#include <iostream>
GameScene::GameScene() 
	:Scene()
{



}
void GameScene::initializeScene()
{
	camera.addComponent<Orthographic>(-50, 50, -50, 50, 0, -20);
	//defining player
	auto& renderSquare = player.addComponent<RenderMeshComponent>();
    renderSquare.mesh = AssetManager::instance().getMesh("squareMesh");
    renderSquare.material = AssetManager::instance().getMaterialPtr("defaultMaterial");
	player.addComponent<BoxCollider>();
	player.addComponent<PlayerScripts>();
	player.getComponent<Trasform>().scale = glm::vec3(5, 2, 1.0f);
	player.getComponent<TagComponent>().tag = "Player";

	//defining ball
	auto& rd = ball.addComponent<RenderMeshComponent>();
    rd.mesh = AssetManager::instance().getMesh("ballMesh");
    rd.material = AssetManager::instance().getMaterialPtr("defaultMaterial");
	ball.addComponent<BoxCollider>();
	ball.addComponent<RigidBody>(RigidBody::BodyType::DYNAMIC, 1.0f, 1.1f).setUseGravity(false);
	ball.getComponent<Trasform>().scale = glm::vec3(2, 2, 1.0f);
	ball.getComponent<TagComponent>().tag = "Ball";
	ball.addComponent<BallScript>();
	//defining the floor
    pavimento.addComponent<RenderMeshComponent>().mesh = AssetManager::instance().getMesh("pavimentoMesh");
    pavimento.getComponent<RenderMeshComponent>().material = AssetManager::instance().getMaterialPtr("defaultMaterial");
	Trasform& pavimentoTrasform = pavimento.getComponent<Trasform>();
	pavimentoTrasform.scale = glm::vec3(100, 20, 0.0f);
	pavimentoTrasform.position = glm::vec3(0, -60, 0);
	pavimento.addComponent<BoxCollider>();
	pavimento.getComponent<TagComponent>().tag = "Pavimento";
	auto& rb = pavimento.addComponent<RigidBody>(RigidBody::BodyType::STATIC);
	rb.setUseGravity(false);
	//controller script


	controller.addComponent<ControllerScript>(player.getComponent<PlayerScripts>());
	//defining game enviroment objects
    background.addComponent<Background2DRender>().material = AssetManager::instance().getMaterialPtr("backgroundMaterial");
}
void GameScene::OnResize(uint32_t width, uint32_t height)
{
	std::cout << "Resize Prova: " << width << "x" << height << std::endl;
	//camera.getComponent<Orthographic>().OnResize(width, height);
}

void GameScene::initializeMaterials()
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
		{},
		{},
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
		{},
		{},
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
			 { -0.722656, -0.678819, 0},
			{ 0.0332031,  0.0711806 , 0},
			{-0.646484, -0.326389,  0 },
			{-0.68457,  0.380208,  0},
			{ -0.170898, -0.0451389,0}

		})));

	AssetManager::instance().addMesh("enemy1Mesh", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
			{-0.414062,0.550347		,0},
			{0.615234 ,0.583333		,0},
			{0.638672 ,-0.763889	,0},
			{0.353516 ,-0.776042	,0},
			{0.431641 ,0.131944		,0},
			{-0.414062,0.550347	,0}

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


	AssetManager::instance().addMesh("LoveJapanese", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
		std::vector<glm::vec3>{
			{ -0.254883  ,0.456597 , 0 },
			{ 0.0439453  ,0.519097 , 0 },
			{ 0.0595703  ,0.505208 , 0 },
			{ 0.0498047  ,0.491319 , 0 },
			{ -0.250977  ,0.420139 , 0 },
			{ -0.254883  ,0.456597 , 0 },
		},
		std::vector<glm::vec3>{
			{ -0.234375  ,0.394097 , 0 },
			{ -0.189453  ,0.347222 , 0 },
			{ -0.177734  ,0.34375 , 0 },
			{ -0.173828  ,0.375 , 0 },
			{ -0.217773  ,0.413194 , 0 },
		},
		std::vector<glm::vec3>{
			{ -0.140625  ,0.418403 , 0 },
			{ -0.0966797  ,0.357639 , 0 },
			{ -0.0673828  ,0.361111 , 0 },
			{ -0.0683594  ,0.392361 , 0 },
			{ -0.110352  ,0.442708 , 0 },
			{ -0.140625  ,0.418403 , 0 },
		 },
		std::vector<glm::vec3>{
			{ -0.0556641  ,0.444444 , 0 },
			{ -0.0078125  ,0.361111 , 0 },
			{ 0.0244141  ,0.364583 , 0 },
			{ 0.0253906  ,0.413194 , 0 },
			{ -0.0556641  ,0.444444 , 0 },
		 },
		std::vector<glm::vec3>{
			{ -0.257812  ,0.310764 , 0 },
			{ -0.290039  ,0.255208 , 0 },
			{ -0.287109  ,0.236111 , 0 },
			{ -0.276367  ,0.215278 , 0 },
			{ -0.236328  ,0.295139 , 0 },
			{ -0.257812  ,0.310764 , 0 },
		 },
		std::vector<glm::vec3>{
			{ -0.192383  ,0.303819 , 0 },
			{ -0.186523  ,0.248264 , 0 },
			{ -0.142578  ,0.196181 , 0 },
			{ -0.0927734  ,0.177083 , 0 },
			{ -0.046875  ,0.145833 , 0 },
			{ -0.0195312  ,0.140625 , 0 },
			{ 0.0478516  ,0.154514 , 0 },
			{ 0.0927734  ,0.225694 , 0 },
			{ 0.0605469  ,0.227431 , 0 },
			{ 0.0458984  ,0.217014 , 0 },
			{ 0.0283203  ,0.204861 , 0 },
			{ -0.000976562  ,0.190972 , 0 },
			{ -0.0449219  ,0.180556 , 0 },
			{ -0.0957031  ,0.197917 , 0 },
			{ -0.137695  ,0.222222 , 0 },
			{ -0.192383  ,0.303819 , 0 },
		 },
		std::vector<glm::vec3>{
			{ -0.112305  ,0.28125 , 0 },
			{ -0.103516  ,0.322917 , 0 },
			{ -0.0849609  ,0.328125 , 0 },
			{ -0.0810547  ,0.310764 , 0 },
			{ -0.0810547  ,0.284722 , 0 },
			{ -0.112305  ,0.28125 , 0 },
		 },
		std::vector<glm::vec3>{
			{ 0.0869141  ,0.328125 , 0 },
			{ 0.0888672  ,0.293403 , 0 },
			{ 0.115234  ,0.258681 , 0 },
			{ 0.152344  ,0.237847 , 0 },
			{ 0.135742  ,0.326389 , 0 },
			{ 0.0869141  ,0.328125 , 0 },
		 },
		std::vector<glm::vec3>{
			{ -0.212891  ,-0.157986 , 0 },
			{ 0.0585938  ,0.0972222 , 0 },
			{ -0.15332  ,0.0885417 , 0 },
			{ -0.154297  ,0.0694444 , 0 },
			{ 0.0136719  ,0.0868056 , 0 },
			{ -0.219727  ,-0.125 , 0 },
			{ -0.212891  ,-0.157986 , 0 },
		 },
		std::vector<glm::vec3>{
			{ -0.155273  ,0.0677083 , 0 },
			{ 0.046875  ,-0.152778 , 0 },
			{ 0.0771484  ,-0.152778 , 0 },
			{ 0.0693359  ,-0.109375 , 0 },
			{ -0.155273  ,0.0677083 , 0 },
		 },
		}, { 0,0,1,1 }, { 1,.0,1,1 })));


	AssetManager::instance().addMesh("enemyJapanese", std::make_shared<Mesh>(HermitInterpolationMesh::generateMesh({
			{
				{ -0.126953  ,0.418403 , 0 },
				{ -0.105469  ,0.446181 , 0 },
				{ -0.0957031  ,0.380208 , 0 },
				{ -0.0927734  ,0.184028 , 0 },
				{ -0.136719  ,0.203125 , 0 },
				{ -0.139648  ,0.446181 , 0 },
				{ -0.126953  ,0.418403 , 0 },
			 },
			{
				{ -0.298828  ,0.194444 , 0 },
				{ 0.0595703  ,0.194444 , 0 },
				{ 0.0761719  ,0.152778 , 0 },
				{ -0.323242  ,0.152778 , 0 },
				{ -0.298828  ,0.194444 , 0 },
			 },
			{
				{ -0.28418  ,0.118056 , 0 },
				{ -0.213867  ,-0.0364583 , 0 },
				{ -0.182617  ,-0.0173611 , 0 },
				{ -0.255859  ,0.152778 , 0 },
				{ -0.28418  ,0.118056 , 0 },
			 },
			{
				{ -0.0917969  ,-0.015625 , 0 },
				{ -0.0224609  ,0.142361 , 0 },
				{ 0.00878906  ,0.157986 , 0 },
				{ 0.0078125  ,0.111111 , 0 },
				{ -0.0615234  ,-0.0329861 , 0 },
				{ -0.0917969  ,-0.0486111 , 0 },
				{ -0.0917969  ,-0.015625 , 0 },
			 },
			{
				{ -0.313477  ,-0.470486 , 0 },
				{ -0.30957  ,-0.0347222 , 0 },
				{ -0.277344  ,-0.0434028 , 0 },
				{ -0.28418  ,-0.503472 , 0 },
				{ -0.313477  ,-0.470486 , 0 },
			 },
			{
				{ -0.0351562  ,-0.501736 , 0 },
				{ -0.00292969  ,-0.498264 , 0 },
				{ 0.00585938  ,-0.454861 , 0 },
				{ -0.00292969  ,-0.282986 , 0 },
				{ -0.00195312  ,-0.0486111 , 0 },
				{ -0.125  ,-0.0625 , 0 },
				{ -0.21875  ,-0.0607639 , 0 },
				{ -0.282227  ,-0.0538194 , 0 },
				{ -0.300781  ,-0.0451389 , 0 },
				{ -0.263672  ,-0.00173611 , 0 },
				{ 0.000976562  ,-0.0138889 , 0 },
				{ 0.015625  ,-0.0920139 , 0 },
				{ 0.015625  ,-0.486111 , 0 },
				{ -0.0351562  ,-0.501736 , 0 },
			 },
			{
				{ -0.277344  ,-0.185764 , 0 },
				{ -0.00390625  ,-0.184028 , 0 },
				{ 0.00390625  ,-0.192708 , 0 },
				{ -0.0302734  ,-0.196181 , 0 },
				{ -0.286133  ,-0.208333 , 0 },
				{ -0.277344  ,-0.185764 , 0 },
			 },
			{
				{ -0.158203  ,-0.0625 , 0 },
				{ -0.161133  ,-0.295139 , 0 },
				{ -0.143555  ,-0.302083 , 0 },
				{ -0.140625  ,-0.258681 , 0 },
				{ -0.140625  ,-0.0520833 , 0 },
				{ -0.158203  ,-0.0625 , 0 },
			 },
			{
				{ -0.265625  ,-0.46875 , 0 },
				{ -0.261719  ,-0.326389 , 0 },
				{ -0.25  ,-0.314236 , 0 },
				{ -0.25293  ,-0.357639 , 0 },
				{ -0.265625  ,-0.46875 , 0 },
			 },
			{
				{ -0.119141  ,-0.453125 , 0 },
				{ -0.0917969  ,-0.454861 , 0 },
				{ -0.0947266  ,-0.364583 , 0 },
				{ -0.0986328  ,-0.298611 , 0 },
				{ -0.249023  ,-0.3125 , 0 },
				{ -0.246094  ,-0.282986 , 0 },
				{ -0.0810547  ,-0.302083 , 0 },
				{ -0.119141  ,-0.453125 , 0 },
			 },
			{
				{ -0.261719  ,-0.442708 , 0 },
				{ -0.09375  ,-0.447917 , 0 },
				{ -0.115234  ,-0.465278 , 0 },
				{ -0.261719  ,-0.442708 , 0 },
			 },
			{
				{ 0.0898438  ,-0.0555556 , 0 },
				{ 0.226562  ,0.380208 , 0 },
				{ 0.233398  ,0.371528 , 0 },
				{ 0.204102  ,0.177083 , 0 },
				{ 0.0908203  ,-0.0954861 , 0 },
				{ 0.0898438  ,-0.0555556 , 0 },
			 },
			{
				{ 0.152344  ,0.140625 , 0 },
				{ 0.445312  ,0.135417 , 0 },
				{ 0.431641  ,0.114583 , 0 },
				{ 0.133789  ,0.126736 , 0 },
				{ 0.152344  ,0.140625 , 0 },
			 },
			{
				{ 0.398438  ,0.112847 , 0 },
				{ 0.0810547  ,-0.378472 , 0 },
				{ 0.0800781  ,-0.399306 , 0 },
				{ 0.0917969  ,-0.378472 , 0 },
				{ 0.430664  ,0.135417 , 0 },
				{ 0.398438  ,0.112847 , 0 },
			 },
			{
				{ 0.158203  ,0.130208 , 0 },
				{ 0.538086  ,-0.46875 , 0 },
				{ 0.568359  ,-0.470486 , 0 },
				{ 0.568359  ,-0.425347 , 0 },
				{ 0.544922  ,-0.401042 , 0 },
				{ 0.171875  ,0.178819 , 0 },
				{ 0.158203  ,0.130208 , 0 },
			 },
		}, { 1,0,0,1 }, { 1,0,0,1 })));
	AssetManager::instance().addMesh("triangoloMesh", triangoloMesh);
	AssetManager::instance().addMaterial("defaultMaterial", std::make_shared<Material>(shader));
	AssetManager::instance().addMaterial("backgroundMaterial", std::make_shared<Material>(backgroundShader));
	AssetManager::instance().addMaterial("colorUniformSupporterMaterial", std::make_shared<Material>(colorSupporterShader));
}
