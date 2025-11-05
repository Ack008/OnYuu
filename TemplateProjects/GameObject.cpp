#include "GameObject.h"
#include "Transform.h"
#include "TreeComponent.h"
#include "Prefab.h"
GameObject::GameObject(entt::entity id, Scene* scene)
	:id(id), _sceneptr(scene)
{

}

GameObject GameObject::instantiatePrefabNow(Prefab* prefab)
{
	return prefab->istantiateObject(_sceneptr);
}

