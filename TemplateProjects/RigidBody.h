#pragma once
#include <glm/glm.hpp>
#include "Component.h"
#include "Transform.h"
#include "GameObject.h"

// Definizione costante di accelerazione gravitazionale (m/s^2)
#define GRAVITY_ACCELERATION 9.81f

// RigidBody
// Componente fisico che fornisce comportamento cinematico di base per un
// GameObject. Supporta tre tipi di corpo tramite `BodyType`:
// - STATIC: corpo immobile (non integrato)
// - DYNAMIC: corpo fisico soggetto a forze/gravità e integrazione della velocità
// - KINEMATIC: corpo animato dall'utente ma non influenzato direttamente dalla fisica
//
// Responsabilità principali:
// - integra posizione e velocità (nell'method `update`) quando il body è DYNAMIC
// - applica forza tramite `applyForce`
// - risposta semplificata alle collisioni in `onCollisionEnter` (invertendo la velocità)
//
class RigidBody : public Component {
public:
    enum class BodyType {
        STATIC,
        DYNAMIC,
        KINEMATIC
    };

    // Costruttore:
    // - `type`: tipo del corpo (default DYNAMIC)
    // - `mass`: massa del corpo (default 1.0f)
    // Il costruttore inizializza velocità/accelerazione. Per i corpi DYNAMIC la
    // accelerazione viene impostata a zero di default; per gli altri tipi può
    // essere utilizzata una accelerazione iniziale (qui esempio con valore non-zero).
    RigidBody(BodyType type = BodyType::DYNAMIC, float mass = 1.0f)
        : bodyType(type), mass(mass), velocity(0.0f), acceleration(type == BodyType::DYNAMIC ? glm::vec3(0.0f) : glm::vec3(0,-9.81,0)) {
    }

    // update: chiamato ogni frame (dt in secondi).
    // Per corpi DYNAMIC integra accelerazione -> velocità -> posizione.
    // Nota: la gravità viene sommata all'accelerazione solo se `_useGravity` è true.
    virtual void update(float dt) override {
        if (bodyType == BodyType::DYNAMIC) {
            if(_useGravity)
                acceleration += glm::vec3(0.0f, -GRAVITY_ACCELERATION, 0.0f); // Apply gravity
            velocity += acceleration * dt;
            obj->getComponent<Trasform>().position += velocity * dt;
            acceleration = glm::vec3(0.0f); // Reset acceleration after each update
        }
    }

    // Risposta alle collisioni: semplice comportamento che inverte la velocità
    // con damping quando si verifica un enter di collisione. Può essere sovrascritto
    // per comportamenti più realistici.
    virtual void onCollisionEnter(Collider* other) override {
        // Simple collision response: invert velocity
        if (bodyType == BodyType::DYNAMIC) {
            velocity = -velocity * 0.5f; // simple bounce with damping
        }
    }

    // Aggiunge una forza al corpo (F = m * a) -> modifica l'accelerazione
    void applyForce(const glm::vec3& force) {
        if (bodyType == BodyType::DYNAMIC) {
            acceleration += force / mass;
        }
    }

    // Getters / setters utili
    BodyType getBodyType() const { return bodyType; }
    float getMass() const { return mass; }
    glm::vec3 getVelocity() const { return velocity; }
    void setVelocity(const glm::vec3& vel) { velocity = vel; }

    // Abilita/disabilita l'applicazione della gravità a questo corpo.
    // ATTENZIONE: implementazione originale contiene un bug: assegna il parametro
    // `useGravity` a sé stesso invece di impostare `_useGravity`.
    // Corretto sarebbe: `_useGravity = useGravity;`
    void setUseGravity(bool useGravity) 
    
    {
        _useGravity = useGravity;
    }

private:
    BodyType bodyType;
    float mass;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    bool _useGravity = false; // flag che abilita la gravità


    // Implementazione di `start` ereditata da Component: qui è vuota.
    void start() {};

};