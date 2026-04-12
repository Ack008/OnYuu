#include "GameObject.h"
#include "Core/Model/Components/Transform.h"
#include "Core/Model/Components/TreeComponent.h"
#include "Core/Prefab.h"
namespace OnYuu {
    GameObject::GameObject(entt::entity id, Scene* scene)
        :id(id), _sceneptr(scene)
    {

    }

    void GameObject::Destroy()
    {
        auto& children = getComponent<TreeComponent>().obj;
        for (std::size_t i = 0; i < children.size(); ++i)
        {
            children[i].Destroy();
        }
        _sceneptr->addToDestroy(new GameObject(id, _sceneptr));
    }

    GameObject GameObject::instantiatePrefabNow(Prefab* prefab)
    {
        return prefab->istantiateObject(_sceneptr);
    }

    void GameObject::setFather(GameObject father)
    {
        if (!hasComponent<TreeComponent>()) {
            addComponent<TreeComponent>();
        }
        auto& treeComp = getComponent<TreeComponent>();
        treeComp.father = father;
        father.getComponent<TreeComponent>().obj.push_back(*this);
    }

    Transform GameObject::getAbsoluteTransform()
    {
        Transform local = getComponent<Transform>();
        TreeComponent& tree = getComponent<TreeComponent>();

        if (!tree.father)
            return local;

        GameObject parent(tree.father);
        Transform parentAbs = parent.getAbsoluteTransform();

        return parentAbs + local;
    }
} // namespace OnYuu