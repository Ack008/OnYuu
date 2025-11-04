#pragma once
#include <memory>
class GameObject; // forward declaration
class Collider;

// Classe base astratta per tutti i componenti (anche "script") che possono
// essere attaccati a un `GameObject`.
// Responsabilità e contratti:
// - ogni componente implementa il ciclo di vita minimo: `start()` (chiamato
//   quando il componente viene inizializzato) e `update(float dt)` (chiamato
//   ogni frame con il delta time)
// - callback di collisione opzionali: `onCollisionEnter`, `onCollisionStay`,
//   `onCollisionEnd` possono essere override per reagire ad eventi di fisica
// - il membro `obj` è un `shared_ptr` al `GameObject` proprietario: permette
//   al componente di accedere e manipolare l'entità a cui è attaccato. La
//   gestione della vita dell'oggetto dipende dalla logica della scena; qui non
//   vengono imposte policy aggiuntive.
class Component {
public:
    virtual ~Component() = default;

    // Chiamato una volta all'inizializzazione del componente
    virtual void start() = 0;

    // Chiamato ogni frame. 'dt' è il delta time in secondi
    virtual void update(float dt) = 0;

    // Callback di collisione: possono essere override nelle sottoclassi
    virtual void onCollisionStay(Collider* other) {};
	virtual void onCollisionEnd(Collider* other) {};
	virtual void onCollisionEnter(Collider* other) {};

    // Puntatore al GameObject proprietario. Usare con attenzione: la policy di
    // ownership è decisa dalla Scene/GameObject, non dalla classe Component.
    std::shared_ptr<GameObject> obj;
};
