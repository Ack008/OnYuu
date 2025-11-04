#pragma once
#include <type_traits>
#include "Component.h"
#include "Scene.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include <entt/entt.hpp>
#include <vector>
#include "ScriptingSystem.h"
class GameObject {
public:
    GameObject(entt::entity id, Scene* scene);
	GameObject() = default;
	~GameObject() = default;
	GameObject(const GameObject& other) = default;
    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        if constexpr (std::is_base_of_v<Component, T>) {
            // Recupera o crea il ScriptListComponent per questo entity
            auto& scriptList = _sceneptr->reg->get_or_emplace<ScriptingSystem>(id,id,_sceneptr);

            // Aggiunge lo script e restituisce il riferimento tipizzato
            T& script = scriptList.addScript<T>(std::forward<Args>(args)...);
            if constexpr (std::is_base_of_v<Collider, T>) {
                _sceneptr->physicsEngine.addCollider(&script);
            }
            return script;
        }
        else {
            // Comportamento standard per componenti non script
            T& comp = _sceneptr->reg->emplace<T>(id, std::forward<Args>(args)...);
            return comp;
        }
    }

    template<typename T>
    T& getComponent() {
        if constexpr (std::is_base_of_v<Component, T>) {
			return _sceneptr->reg->get<ScriptingSystem>(id).getScript<T>();
        }
        return _sceneptr->reg->get<T>(id);
    }

    template<typename T>
    bool hasComponent() {
		if constexpr (std::is_base_of_v<Component, T>) {
			return _sceneptr->reg->any_of<ScriptingSystem>(id) && _sceneptr->reg->get<ScriptingSystem>(id).hasScript<T>();
		}
        return _sceneptr->reg->any_of<T>(id);
    }

    template<typename T>
    void removeComponent() {
        if constexpr (std::is_base_of_v<Component, T>) {
            if (_sceneptr->reg->any_of<ScriptingSystem>(id)) {
                auto& scriptList = _sceneptr->reg->get<ScriptingSystem>(id);
                if constexpr (std::is_base_of_v<Collider, T>) {
                    _sceneptr->physicsEngine.removeCollider(&scriptList.getScript<T>());
                }
                scriptList.removeScript<T>();
            }
            return;
        }
        _sceneptr->reg->remove<T>(id);
    }
    void onCollisionStay(Collider* other) {
		if (hasComponent<ScriptingSystem>()) {
            auto& scriptList = getComponent<ScriptingSystem>();
            scriptList.onCollisionStay(other);
        }
    }
    void onCollisionEnter(Collider* other) {
        if (hasComponent<ScriptingSystem>()) {
            auto& scriptList = getComponent<ScriptingSystem>();
            scriptList.onCollisionEnter(other);
        }
    }
    void onCollisionEnd(Collider* other) {
        if (hasComponent<ScriptingSystem>()) {
            auto& scriptList = getComponent<ScriptingSystem>();
            scriptList.onCollisionEnd(other);
        }
    }
    void Destroy() {
        _sceneptr->addToDestroy(this);
    }
    Scene* getScene() { return _sceneptr; }
private:
    Scene* _sceneptr;
    entt::entity id{ entt::null };
    friend class Scene;
};
