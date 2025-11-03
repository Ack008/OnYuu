#pragma once
#include "Component.h"
#include "Collider.h"
#include <memory>
#include <vector>
class ScriptingSystem {
public:
    std::vector<std::shared_ptr<Component>> scripts;
	ScriptingSystem(entt::entity id, Scene* scene) : id(id), scene(scene){}
    template<typename T, typename... Args>
    T& addScript(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto instance = std::make_shared<T>(std::forward<Args>(args)...);
        instance->obj = std::make_shared<GameObject>(id,scene);
        scripts.push_back(instance);
		startedScripts[instance] = false;
        return *instance;
    }
	template<typename T>
    void removeScript() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        scripts.erase(std::remove_if(scripts.begin(), scripts.end(),
            [](const std::shared_ptr<Component>& script) {
                return dynamic_cast<T*>(script.get()) != nullptr;
            }), scripts.end());
	}
	template<typename T>
    bool hasScript() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        for (const auto& script : scripts) {
            if (dynamic_cast<T*>(script.get()) != nullptr) {
                return true;
            }
        }
		return false;
	}
	template<typename T>
    T& getScript() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        for (const auto& script : scripts) {
            if (auto casted = dynamic_cast<T*>(script.get())) {
                return *casted;
            }
        }
        throw std::runtime_error("Script of specified type not found");
	}
    void update(float dt) {
        for (auto& script : scripts) {
            if (!startedScripts[script]) {
				script->start();
				startedScripts[script] = true;
            }
            script->update(dt);
        }
    }
    void onCollisionEnter(Collider* other) {
        for (auto& script : scripts) {
            script->onCollisionEnter(other);
        }
	}
    void onCollisionStay(Collider* other) {
        for (auto& script : scripts) {
            script->onCollisionStay(other);
        }
    }
    void onCollisionEnd(Collider* other) {
        for (auto& script : scripts) {
            script->onCollisionEnd(other);
        }
	}
	entt::entity id;
	Scene* scene;
	std::shared_ptr<GameObject >obj;
	std::unordered_map<std::shared_ptr<Component>, bool> startedScripts;
};