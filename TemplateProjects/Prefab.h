#pragma once
#include "Engine.h"
class Prefab {
	public:
		virtual void istantiateObject(Scene* scene) = 0;
	//possibile implementazione di prefab statici
	/*
	static GameObject createPlayer(Scene* scene) {
		GameObject player = scene->createEntity();
		auto& renderSquare = player.addComponent<RenderMeshComponent>();
		renderSquare.mesh = AssetManager::instance().getMesh("squareMesh");
		renderSquare.material = AssetManager::instance().getMaterial("defaultMaterial");
		player.addComponent<BoxCollider>();
		player.addComponent<RigidBody>(RigidBody::BodyType::DYNAMIC, 1.0, 0.2).setUseGravity(true);
		player.addComponent<PlayerScripts>();
		player.getComponent<Trasform>().scale = glm::vec3(2, 2.f, 1.0f);
		return player;
	}
	static GameObject createFloor(Scene* scene) {
		GameObject floor = scene->createEntity();
		floor.addComponent<RenderMeshComponent>().mesh = AssetManager::instance().getMesh("pavimentoMesh");
		floor.getComponent<RenderMeshComponent>().material = AssetManager::instance().getMaterial("defaultMaterial");
		Trasform& pavimentoTrasform = floor.getComponent<Trasform>();
		pavimentoTrasform.scale = glm::vec3(100, 5, 0.0f);
		pavimentoTrasform.position = glm::vec3(0, -50, 0);
		floor.addComponent<BoxCollider>();
		floor.addComponent<RigidBody>(RigidBody::BodyType::STATIC);
		return floor;
	}*/
};