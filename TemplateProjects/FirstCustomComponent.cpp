#include <iostream>
#include "FirstCustomComponent.h"
#include "Transform.h"
#include "GameObject.h"
#include "Input.h"
#include <memory>
#include "RigidBody.h"
#include "projectileComponent.h"
#include "AssetManager.h"
void FirstCustomComponent::update(float dt)
{
	auto& transform = obj->getComponent<Trasform>();
	if (Input::isKeyPressed(KeyCode::W)) {
		transform.position += glm::vec3(0.0f, 2000 * dt, 0.0f);
	}
	if (Input::isKeyPressed(KeyCode::S)) {
		transform.position -= glm::vec3(0.0f, 2000 * dt, 0.0f);
	}
	if (Input::isKeyPressed(KeyCode::A)) {
		transform.position -= glm::vec3(2000 * dt, 0.0f, 0.0f);
	}
	if (Input::isKeyPressed(KeyCode::D)) {
		transform.position += glm::vec3(2000 * dt, 0.0f, 0.0f);
	}
	if (Input::isKeyPressedOnce(KeyCode::Space)) {
		istantiateProjectile(transform);
	}
	//std::cout << "Position: " << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << std::endl;
	std::cout << "Frame Time: " << 1/dt << std::endl;
}

void FirstCustomComponent::istantiateProjectile(Trasform& transform)
{
	GameObject projectile = obj->getScene()->createEntity();
	RenderMeshComponent& renderMesh = projectile.addComponent<RenderMeshComponent>();
	renderMesh.renderingType = RenderingTypeEnum::TRIANGLE;
	renderMesh.mesh = AssetManager::instance().getMesh("triangoloMesh");
	renderMesh.material = AssetManager::instance().getMaterial("defaultMaterial");
	Trasform& projTransform = projectile.getComponent<Trasform>();
	projTransform.position = transform.position + glm::vec3(10, 0.0f, 0.0f);
	projTransform.scale = glm::vec3(50.0f, 50.0f, 1.0f);
	projectile.addComponent<BoxCollider>();
	projectile.addComponent<ProjectileComponent>();
}

void FirstCustomComponent::start()
{
	auto& transform = obj->getComponent<Trasform>();
	transform.position = glm::vec3(500, 0.0f, 1.0f);
	transform.scale = glm::vec3(100, 100, 1.0f);

}

void FirstCustomComponent::onCollisionStay(Collider* other) {
	std::cout << "collision\n";
}