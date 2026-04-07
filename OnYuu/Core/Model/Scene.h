#pragma once
#include <vector>
#include <entt/entt.hpp>
#include "Component.h"
#include "Core/Model/Components/Collider.h"
#include "Physics/PhysicsEngine.h"
#include "Core/Model/Components/Camera.h"
#include "Core/Model/Components/Lights.h"
#include "Render/Buffer.h"
#include "Render/RenderTarget.h"
namespace OnYuu {
// Classe che rappresenta la scena del motore di gioco.
// Responsabilità principali:
// - gestire il registro degli enti (entt::registry)
// - creare/distruggere GameObject
// - eseguire l'aggiornamento della fisica e le collisioni
// - inoltrare gli oggetti da renderizzare
// Nota: molti metodi delegano il lavoro a sistemi/engine esterni come PhysicsEngine.
class Prefab;
class Scene {
public:
    Scene();
    ~Scene();

    // Crea e ritorna un nuovo GameObject associato a questa scena.
    GameObject createEntity();

    // Aggiorna la scena (chiamato ogni frame). 'dt' è il delta time in secondi.
    void update(float dt);

	// Renderizza la scena (chiamato ogni frame dopo update).
	void render(Camera *renderCamera = nullptr, std::shared_ptr<RenderTarget> renderTarget = nullptr);
    // Chiamato all'avvio della scena per inizializzare componenti e sistemi.
    void start();

    // Metodo puro virtuale: le scene concrete devono implementare la gestione
    // del ridimensionamento della viewport / camera.
    virtual void OnResize(uint32_t width, uint32_t height) {};

    // Aggiunge un GameObject alla lista di quelli da distruggere a fine frame.
    void addToDestroy(GameObject* obj) { toDestroy.push_back(obj); };
    // Istanzia i prefab segnati per l'instanziazione.
    void instantiatePrefabs();
    // Imposta ed inizializza i materiali specifici della scena.
    virtual void initializeMaterials() {};

    //Imposta la scena ed i suoi oggetti
    virtual void initializeScene() {};
	// Ottieni tutte le entità 
    std::vector< GameObject > getGameObjects();
private:
    // Calcola le collisioni tra i collider presenti e notifica gli oggetti coinvolti.
    void calculateCollisions(float dt);

    // Prepara e invia i dati alla pipeline di rendering.
    void sendToRender();

    // Distrugge gli enti segnati con addToDestroy.
    void destroyEntities();


private:
    // Registro di entità utilizzato per componenti e sistemi (entt).
    // È allocato dinamicamente qui: la classe è responsabile della sua vita.
    entt::registry* reg = new entt::registry();

    // Vettore di puntatori a GameObject che devono essere distrutti.
    std::vector<GameObject*> toDestroy;
    //vector di prefab da istanziare nel prossimo frame
    std::vector<Prefab*> toInstantiate;
    // Sistema di fisica usato dalla scena per simulazioni e risoluzioni di collisione.
    PhysicsEngine physicsEngine;

    // Camera utilizzata dall'editor (può essere nullptr se non impostata).
    Camera* editorCamera = nullptr;
	// Active camera della scena (può essere nullptr se non impostata).
	Camera* activeCamera = nullptr;
    // Le seguenti classi hanno accesso ai membri privati della Scene.
    friend class GameObject;
    friend class Layer;
    friend class DebugLayer;
	friend class SceneHierarchyPanel;
	friend class ViewportPanel;
   
};
} // namespace OnYuu