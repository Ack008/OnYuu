#pragma once

class GameObject; // forward declaration

class Component {
public:
    virtual ~Component() = default;
    virtual void update(float dt) = 0;
    virtual void start() = 0;

    GameObject* obj = nullptr; // puntatore al GameObject proprietario
};
