#pragma once
#include "Engine.h"
#include "EnemyScripts.h"
class Enemy1 : public Prefab {
	public:
		virtual GameObject istantiateObject(Scene* scene) override {
			GameObject enemy = scene->createEntity();
			auto& renderSquare = enemy.addComponent<RenderMeshComponent>();
			renderSquare.mesh = AssetManager::instance().getMesh("enemy1Mesh");
			renderSquare.material = AssetManager::instance().getMaterial("colorUniformSupporterMaterial");


			renderSquare.material->set("uColor", glm::vec4(r, g, b,1.0));
			enemy.addComponent<BoxCollider>();
			enemy.addComponent<RigidBody>(RigidBody::BodyType::STATIC).setUseGravity(true);
			enemy.getComponent<Trasform>().scale = glm::vec3(3, 3.f, 1.0f);
			enemy.getComponent<Trasform>().position = glm::vec3(0, 0, 0);
			enemy.addComponent<EnemyScripts>();
			enemy.getComponent<TagComponent>().tag = "Enemy";
			return GameObject(enemy.getID(), scene);
		}
		Enemy1() {
			srand(time(NULL));
			r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		}
private:
	float r;
	float g;
	float b;
};

class Enemy2 : public Prefab {
public:
	virtual GameObject istantiateObject(Scene* scene) override {
		GameObject enemy = scene->createEntity();
		auto& renderSquare = enemy.addComponent<RenderMeshComponent>();
		renderSquare.mesh = AssetManager::instance().getMesh("enemyMesh2");
		renderSquare.material = AssetManager::instance().getMaterial("colorUniformSupporterMaterial");
		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		renderSquare.material->set("uColor", glm::vec4(r, g, b, 1.0));
		enemy.addComponent<BoxCollider>();
		enemy.addComponent<RigidBody>(RigidBody::BodyType::STATIC).setUseGravity(true);
		enemy.getComponent<Trasform>().scale = glm::vec3(3, 3.f, 1.0f);
		enemy.getComponent<Trasform>().position = glm::vec3(0, 0, 0);
		enemy.addComponent<EnemyScripts>();
		enemy.getComponent<TagComponent>().tag = "Enemy";
		return GameObject(enemy.getID(), scene);
	}
	Enemy2() {
		srand(time(NULL));
		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}
private:
	float r;
	float g;
	float b;
};