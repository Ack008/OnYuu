#pragma once
#include <type_traits>
#include "Component.h"
#include "Scene.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include <entt/entt.hpp>
#include <vector>

class GameObject {
public:
    GameObject(Scene* scene);

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        T& comp = _sceneptr->reg->emplace<T>(id, std::forward<Args>(args)...);
        if constexpr (std::is_base_of_v<Component, T>) {
            _sceneptr->componentsList.push_back(&comp);
            comp.obj = this;
		}
        return comp;
    }

    template<typename T>
    T& getComponent() {
        return _sceneptr->reg->get<T>(id);
    }

    template<typename T>
    bool hasComponent() {
        return _sceneptr->reg->any_of<T>(id);
    }

    template<typename T>
    void removeComponent() {
        _sceneptr->reg->remove<T>(id);
    }

private:
    Scene* _sceneptr;
    entt::entity id;
};
