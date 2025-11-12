#pragma once
#include "Core/Engine.h"
#include "../Component/EnemyScripts.h"
class Enemy1 : public Prefab {
	public:
		virtual GameObject istantiateObject(Scene* scene) override {
			GameObject enemy = scene->createEntity();
			auto& renderSquare = enemy.addComponent<RenderMeshComponent>();
			renderSquare.mesh = AssetManager::instance().getMesh("enemyJapanese");
			renderSquare.material = AssetManager::instance().getMaterial("defaultMaterial");


			enemy.addComponent<BoxCollider>();
			enemy.addComponent<RigidBody>(RigidBody::BodyType::STATIC).setUseGravity(true);
			enemy.getComponent<Trasform>().scale = glm::vec3(6, 6.f, 1.0f);
			enemy.getComponent<Trasform>().position = glm::vec3(0, 0, 0);
			enemy.addComponent<EnemyScripts>();
			enemy.getComponent<TagComponent>().tag = "Enemy";
			return GameObject(enemy.getID(), scene);
		}
		Enemy1() {
		}
private:

};

class Enemy2 : public Prefab {
public:
	virtual GameObject istantiateObject(Scene* scene) override {
		GameObject enemy = scene->createEntity();
		auto& renderSquare = enemy.addComponent<RenderMeshComponent>();
		renderSquare.mesh = AssetManager::instance().getMesh("LoveJapanese");
		renderSquare.material = AssetManager::instance().getMaterial("defaultMaterial");
		
		enemy.addComponent<BoxCollider>();
		enemy.addComponent<RigidBody>(RigidBody::BodyType::STATIC).setUseGravity(true);
		enemy.getComponent<Trasform>().scale = glm::vec3(5, 5.f, 1.0f);
		enemy.getComponent<Trasform>().position = glm::vec3(0, 0, 0);
		EnemyScripts& scripts = enemy.addComponent<EnemyScripts>();
		enemy.getComponent<TagComponent>().tag = "Enemy";
		return GameObject(enemy.getID(), scene);
	}
	Enemy2() {
		
	}
private:

};