#pragma once
#include <glm/glm.hpp>
#include "Component.h"
class BoxCollider;
class CircleCollider;
class Collider : public Component {
public:
		Collider() = default;
		virtual ~Collider() = default;
		virtual void update(float dt) = 0;
		virtual void start() = 0;
		virtual bool hasCollided(Collider* other) = 0;
		virtual bool collideWith(BoxCollider* other) = 0;
		virtual bool collideWith(CircleCollider* other) = 0;
private:
	bool isTrigger = false;

};

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
