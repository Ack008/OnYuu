#include "GameObject.h"
#include "Transform.h"
#include "TreeComponent.h"
GameObject::GameObject(entt::entity id, Scene* scene)
	:id(id), _sceneptr(scene)
{

}
