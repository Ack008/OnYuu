#include "GameObject.h"
#include "Core/Model/Components/Transform.h"
#include "Core/Model/Components/TreeComponent.h"
#include "Core/Prefab.h"
GameObject::GameObject(entt::entity id, Scene* scene)
	:id(id), _sceneptr(scene)
{

}

GameObject GameObject::instantiatePrefabNow(Prefab* prefab)
{
	return prefab->istantiateObject(_sceneptr);
}

