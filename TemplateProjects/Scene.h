#pragma once
#include <vector>
#include <entt/entt.hpp>
#include "Component.h"
#include "Collider.h"
#include "PhysicsEngine.h"
class Scene {
public:
    Scene();
    ~Scene();

    GameObject createEntity();

    void update(float dt);
    void start();
	virtual void OnResize(uint32_t width, uint32_t height) = 0;
    void addToDestroy(GameObject* obj) { toDestroy.push_back(obj); };
private:
	void calculateCollisions(float dt);
	void sendToRender();
    void destroyEntities();
private:
    entt::registry* reg = new entt::registry();
    std::vector<Component*> componentsList;
	std::vector<BoxCollider*> boxColliderList;
    std::vector<GameObject*> toDestroy;
	PhysicsEngine physicsEngine;

    friend class GameObject;
    friend class Layer;
	friend class DebugLayer;
};
