#pragma once
#include <vector>
#include "Component.h"
#include <entt/entt.hpp>

class Scene {
public:
    Scene() = default;
    ~Scene();

    entt::entity createEntity();

    void update(float dt);
    void start();
	virtual void OnResize(uint32_t width, uint32_t height) = 0;

private:
    entt::registry* reg = new entt::registry();
    std::vector<Component*> componentsList;

    friend class GameObject;
};
