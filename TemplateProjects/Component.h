#pragma once
#include <memory>
class GameObject; // forward declaration
class Collider;
class Component {
public:
    virtual ~Component() = default;
    virtual void update(float dt) = 0;
    virtual void start() = 0;
    virtual void onCollisionStay(Collider* other) {};
	virtual void onCollisionEnd(Collider* other) {};
	virtual void onCollisionEnter(Collider* other) {};
    std::shared_ptr<GameObject> obj; // puntatore al GameObject proprietario
};
