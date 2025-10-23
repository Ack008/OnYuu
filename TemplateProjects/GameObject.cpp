#include "GameObject.h"
#include "Transform.h"
GameObject::GameObject(Scene* scene)
	:_sceneptr(scene)
{
	this-> id = _sceneptr->createEntity();
	addComponent<Trasform>();
}
