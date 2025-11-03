#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshComponent.h"


void BoxCollider::update(float dt) {
	
}
void BoxCollider::start() {
	if (obj->hasComponent<RenderMeshComponent>()) {
		auto& renderMeshComp = obj->getComponent<RenderMeshComponent>();
		if (renderMeshComp.mesh) {
			// Assuming the mesh has position data to determine width and height
			if (!renderMeshComp.mesh->position.empty()) {
				// Calculate width and height based on mesh's bounding box
				float minX = renderMeshComp.mesh->position[0].x;
				float maxX = renderMeshComp.mesh->position[0].x;
				float minY = renderMeshComp.mesh->position[0].y;
				float maxY = renderMeshComp.mesh->position[0].y;
				for (const auto& pos : renderMeshComp.mesh->position) {
					if (pos.x < minX) minX = pos.x;
					if (pos.x > maxX) maxX = pos.x;
					if (pos.y < minY) minY = pos.y;
					if (pos.y > maxY) maxY = pos.y;
				}
				width = maxX - minX;
				height = maxY - minY;
				maxPoint = glm::vec3(maxX, maxY, 0.0f);
				minPoint = glm::vec3(minX, minY, 0.0f);
			}
		}
	}
}
bool BoxCollider::hasCollided(Collider* other) {
	return other->collideWith(this);
}
bool BoxCollider::collideWith(BoxCollider* other) {
	Trasform thisTransform = obj->getComponent<Trasform>();
	glm::vec3 thisMin = thisTransform.getModelMatrix() * glm::vec4(this->getMinPoint(),1);
	glm::vec3 thisMax = thisTransform.getModelMatrix() * glm::vec4(this->getMaxPoint(),1);
	Trasform otherTransform = other->obj->getComponent<Trasform>();
	glm::vec3 otherMin = otherTransform.getModelMatrix() * glm::vec4(other->getMinPoint(),1);
	glm::vec3 otherMax = otherTransform.getModelMatrix() * glm::vec4(other->getMaxPoint(),1);
	// Implement box-box collision detection logic heree
	bool overlapX = thisMin.x <= otherMax.x && thisMax.x >= otherMin.x;
	bool overlapY = thisMin.y <= otherMax.y && thisMax.y >= otherMin.y;
	return overlapX && overlapY;
}
bool BoxCollider::collideWith(CircleCollider* other) {
	// Implement box-circle collision detection logic here
	return false; // Placeholder return value
}
bool CircleCollider::hasCollided(Collider* other) {
	return other->collideWith(this);
}
bool CircleCollider::collideWith(BoxCollider* other) {
	// Implement circle-box collision detection logic here
	return false; // Placeholder return value
}
bool CircleCollider::collideWith(CircleCollider* other) {
	// Implement circle-circle collision detection logic here
	return false; // Placeholder return value
}
