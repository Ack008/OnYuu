#include <iostream>
#include "TryComponent.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshComponent.h"
void TryComponent::update(float dt)
{
	obj->getComponent<RenderMeshComponent>().mesh->position[0].x += 100 * dt;
}

void TryComponent::start()
{

}
