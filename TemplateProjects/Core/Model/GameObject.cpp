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

void GameObject::setFather(GameObject* father)
{
    if (!hasComponent<TreeComponent>()) {
        addComponent<TreeComponent>();
    }
    auto& treeComp = getComponent<TreeComponent>();
    treeComp.father = father;
}

Trasform GameObject::getAbsoluteTransform()
{
    Trasform local = getComponent<Trasform>();
    TreeComponent& tree = getComponent<TreeComponent>();

    if (tree.father == nullptr)
        return local;

    GameObject parent(*tree.father);
    Trasform parentAbs = parent.getAbsoluteTransform();

    return parentAbs + local;
}
