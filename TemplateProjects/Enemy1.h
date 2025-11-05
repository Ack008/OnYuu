#pragma once
#include "Engine.h"
#include "EnemyScripts.h"
class Enemy1 : public Prefab {
	public:
		virtual void istantiateObject(Scene* scene) override {
			GameObject enemy = scene->createEntity();
			auto& renderSquare = enemy.addComponent<RenderMeshComponent>();
			renderSquare.mesh = AssetManager::instance().getMesh("enemy1Mesh");
			renderSquare.material = AssetManager::instance().getMaterial("defaultMaterial");
			enemy.addComponent<BoxCollider>();
			enemy.addComponent<RigidBody>(RigidBody::BodyType::DYNAMIC, 1.0, 0.2).setUseGravity(true);
			enemy.getComponent<Trasform>().scale = glm::vec3(2, 2.f, 1.0f);
			enemy.getComponent<Trasform>().position = glm::vec3(0, 0, 0);
			enemy.addComponent<EnemyScripts>();
			enemy.getComponent<TagComponent>().tag = "Enemy";
		}
};

class Enemy2 : public Prefab {
public:
	virtual void istantiateObject(Scene* scene) override {
		GameObject enemy = scene->createEntity();
		auto& renderSquare = enemy.addComponent<RenderMeshComponent>();
		renderSquare.mesh = AssetManager::instance().getMesh("enemyMesh2");
		renderSquare.material = AssetManager::instance().getMaterial("defaultMaterial");
		enemy.addComponent<BoxCollider>();
		enemy.getComponent<Trasform>().scale = glm::vec3(-5, 5.f, 1.0f);
		enemy.getComponent<Trasform>().position = glm::vec3(0, 0, 0);
		enemy.addComponent<EnemyScripts>().setAmplitude(10);
		enemy.getComponent<TagComponent>().tag = "Enemy";
	}
};