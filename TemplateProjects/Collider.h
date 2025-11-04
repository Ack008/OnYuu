#pragma once
#include <glm/glm.hpp>
#include "Component.h"

// Collider.h
// Definizione della gerarchia di collider usata dal motore di fisica.
// - `Collider` è una classe astratta che estende `Component` e definisce
//   l'interfaccia comune per i test di collisione (hasCollided, collideWith...)
// - `BoxCollider` e `CircleCollider` sono implementazioni concrete che
//   forniscono logica specifica per AABB/Box e cerchio rispettivamente.
//
// Nota: i collider sono pensati per essere usati come "script" (derivano da
// `Component`) e quindi vengono gestiti tramite `ScriptingSystem` nel `GameObject`.


class BoxCollider;
class CircleCollider;

class Collider : public Component {
public:
	Collider() = default;
	virtual ~Collider() = default;

	// Ciclo di vita del componente (ereditato):
	virtual void update(float dt) = 0;
	virtual void start() = 0;

	// Interfaccia di collisione:
	// - hasCollided: test generale di collisione rispetto a un altro Collider
	// - collideWith: overload specifiche per coppie di tipo
	virtual bool hasCollided(Collider* other) = 0;
	virtual bool collideWith(BoxCollider* other) = 0;
	virtual bool collideWith(CircleCollider* other) = 0;

	// Flag interno che indica se il collider è di tipo trigger (non genera risposta fisica)
private:
	bool isTrigger = false;

};

// BoxCollider: implementazione a rettangolo/axis-aligned (o orientato, a seconda
// dell'implementazione nel .cpp). Espone metodi per ottenere i punti min/max
// e le funzioni di test specifiche.
class BoxCollider : public Collider {
public:
	virtual void update(float dt) override;
	virtual void start() override;
	virtual ~BoxCollider() = default;

	virtual bool hasCollided(Collider* other) override;
	virtual bool collideWith(BoxCollider* other) override;
	virtual bool collideWith(CircleCollider* other) override;

	glm::vec3 getMinPoint() const { return minPoint; }
	glm::vec3 getMaxPoint() const { return maxPoint; }

private:
	float width;
	float height;
	glm::vec3 minPoint;
	glm::vec3 maxPoint;
};

// CircleCollider: implementazione semplice basata su raggio. Utile per test
// di collisione circolare e per combinare con BoxCollider.
class CircleCollider : public Collider {
public:
	CircleCollider(float radius) : radius(radius) {}
	virtual ~CircleCollider() = default;

	virtual bool hasCollided(Collider* other) override;
	virtual bool collideWith(BoxCollider* other) override;
	virtual bool collideWith(CircleCollider* other) override;
private:
	float radius;
};
