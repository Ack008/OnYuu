#include "GameScene.h"
#include "Component/PlayerScripts.h"
#include "Component/ControllerScript.h"
#include "Component/BallScript.h"
#include <iostream>
GameScene::GameScene() 
	:Scene()
{
	camera.addComponent<Orthographic>(-50,50,-50,50, 0, -20);
	//defining player
	auto& renderSquare = player.addComponent<RenderMeshComponent>();
	renderSquare.mesh = AssetManager::instance().getMesh("squareMesh");
	renderSquare.material = AssetManager::instance().getMaterial("defaultMaterial");
	player.addComponent<BoxCollider>();
	player.addComponent<PlayerScripts>();
	player.getComponent<Trasform>().scale = glm::vec3(5,2, 1.0f);
	player.getComponent<TagComponent>().tag = "Player";

	//defining ball
	auto &rd = ball.addComponent<RenderMeshComponent>();
	rd.mesh = AssetManager::instance().getMesh("ballMesh");
	rd.material = AssetManager::instance().getMaterial("defaultMaterial");
	ball.addComponent<BoxCollider>();
	ball.addComponent<RigidBody>(RigidBody::BodyType::DYNAMIC, 1.0f, 1.1f).setUseGravity(false);
	ball.getComponent<Trasform>().scale = glm::vec3(2, 2, 1.0f);
	ball.getComponent<TagComponent>().tag = "Ball";
	ball.addComponent<BallScript>();
	//defining the floor
	pavimento.addComponent<RenderMeshComponent>().mesh = AssetManager::instance().getMesh("pavimentoMesh");
	pavimento.getComponent<RenderMeshComponent>().material = AssetManager::instance().getMaterial("defaultMaterial");
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
	background.addComponent<Background2DRender>().material = AssetManager::instance().getMaterial("backgroundMaterial");


}
void GameScene::OnResize(uint32_t width, uint32_t height)
{
	std::cout << "Resize Prova: " << width << "x" << height << std::endl;
	//camera.getComponent<Orthographic>().OnResize(width, height);
}
