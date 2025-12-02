#pragma once
#include <type_traits>
#include "Component.h"
#include "Scene.h"
#include "Render/Renderer.h"
#include "Core/Model/Components/MeshComponent.h"
#include <entt/entt.hpp>
#include <vector>
#include "Core/Model/Components/ScriptingSystem.h"
#include "Core/Model/Components/TagComponent.h"
class Trasform;
// GameObject.h
// Wrapper leggero attorno a `entt::entity` che espone API comode per gestire
// componenti, script e interazioni con la `Scene`.
//
// Responsabilità principali:
// - mantenere l'identificativo `entt::entity` e il puntatore alla `Scene` a cui
//   l'entity appartiene
// - fornire metodi template per aggiungere/ottenere/verificare/rimuovere componenti
// - gestire la registrazione automatica dei componenti di tipo `Collider` nel
//   `PhysicsEngine` della scena
// - delegare gli eventi di componenti custom a script associati (tramite `ScriptingSystem`)
// - permettere di segnare l'oggetto per la distruzione (viene rimossa dalla scena
//   in un momento sicuro tramite `Scene::destroyEntities`)

class GameObject {
public:
    GameObject(entt::entity id, Scene* scene);
	GameObject() = default;
	~GameObject() = default;
	GameObject(const GameObject& other) = default;

	// Aggiunge un componente di tipo `T` all'entity.
	// Comportamento speciale:
	// - se `T` deriva da `Component` (quindi è inteso come "script"), viene
	//   utilizzato/creato un `ScriptingSystem` come contenitore per gli script;
	//   lo script viene aggiunto lì tramite `ScriptingSystem::addScript<T>` e
	//   viene ritornato il riferimento allo script creato.
	// - se lo script è anche un `Collider`, allora viene registrato nel
	//   `physicsEngine` della scena (chiamando `addCollider`).
	// - per componenti non-script il comportamento è quello standard di `entt`
	//   usando `registry::emplace`.
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

	// Ottiene un riferimento al componente `T` attaccato all'entity.
	// Se `T` è uno `script` (deriva da `Component`) viene recuperato tramite
	// il `ScriptingSystem` associato; altrimenti si usa `registry::get<T>`.
    template<typename T>
    T& getComponent() {
        if constexpr (std::is_base_of_v<Component, T>) {
			return _sceneptr->reg->get<ScriptingSystem>(id).getScript<T>();
        }
        return _sceneptr->reg->get<T>(id);
    }
    Trasform getAbsoluteTransform();

	//Istanzia un prefab in modo ritardato
	void delayedIstantiatePrefab(Prefab* prefab) {
        _sceneptr->toInstantiate.push_back(prefab);
	}
	//Istanzia un prefab immediatamente 
    GameObject instantiatePrefabNow(Prefab* prefab);
	// Verifica se l'entity possiede un componente `T`.
	// Ottieni tutti i game objects associati a un componente
	template<typename T>
    std::vector< GameObject > getGameObjectsByComponent() {
        std::vector< GameObject > results;
        auto view = _sceneptr->reg->view<T>();
        for (auto entity : view) {
            results.push_back(GameObject(entity, _sceneptr));
        }
        return results;
	}
	// Per script controlla la presenza di `ScriptingSystem` e poi se contiene lo script.
    template<typename T>
    bool hasComponent() {
		if constexpr (std::is_base_of_v<Component, T>) {
			return _sceneptr->reg->any_of<ScriptingSystem>(id) && _sceneptr->reg->get<ScriptingSystem>(id).hasScript<T>();
		}
        return _sceneptr->reg->any_of<T>(id);
    }

	// Rimuove un componente `T` dall'entity.
	// Per gli script rimuove lo script dal `ScriptingSystem` e, se si tratta di
	// un `Collider`, lo deregistra dal `PhysicsEngine`.
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
	//trova un gameobject dato un tag
    GameObject findGameObjectByTag(const std::string& tag) {
        auto view = _sceneptr->reg->view<TagComponent>();
        for (auto entity : view) {
            auto& tagComp = view.get<TagComponent>(entity);
            if (tagComp.tag == tag) {
                return GameObject(entity, _sceneptr);
            }
        }
        throw std::runtime_error("GameObject with tag " + tag + " not found");
	}
	//Trova tutti i gameobject con un dato tag
    std::vector<GameObject> findGameObjectsByTag(const std::string& tag) {
        std::vector<GameObject> results;
        auto view = _sceneptr->reg->view<TagComponent>();
        for (auto entity : view) {
            auto& tagComp = view.get<TagComponent>(entity);
            if (tagComp.tag == tag) {
				results.push_back(GameObject(entity, _sceneptr));
            }
        }
        return results;
	}
	// Eventi di collisione: vengono passati ai metodi del `ScriptingSystem` se presente
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

	// Segnala la distruzione dell'oggetto: la rimozione effettiva avverrà in un
	// punto sicuro (tipicamente a fine frame) dalla `Scene`.
    void Destroy() {
        _sceneptr->addToDestroy(new GameObject(id,_sceneptr));
    }
    bool operator ==(const GameObject& other) const {
        return id == other.id && _sceneptr == other._sceneptr;
	}
    void setFather(GameObject* father);
    entt::entity getID() const { return id; }
    Scene* getScene() { return _sceneptr; }
    operator bool() const {
        return id != entt::null && _sceneptr != nullptr;
	}
private:
    Scene* _sceneptr; // scena proprietaria (non possiede)
    entt::entity id{ entt::null };
    friend class Scene; // Scene può costruire/manipolare GameObject internamente
private:

};
