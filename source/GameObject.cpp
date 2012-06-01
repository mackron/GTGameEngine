
#include <GTEngine/GameObject.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' : used in base member initializer list
#endif

namespace GTEngine
{
    GameObject::GameObject()
        : sceneNode(), sceneNodeEventHandler(*this)
    {
        this->sceneNode.AttachEventHandler(this->sceneNodeEventHandler);
    }

    GameObject::~GameObject()
    {
    }
}

// Default event handlers.
namespace GTEngine
{
    void GameObject::OnTransform()
    {
    }

    void GameObject::OnScale()
    {
    }

    void GameObject::OnSceneChanged()
    {
    }

    void GameObject::OnStaticChanged()
    {
    }

    void GameObject::OnVisibleChanged()
    {
    }

    void GameObject::OnComponentAttached(Component &)
    {
    }

    void GameObject::OnComponentDetached(Component &)
    {
    }

    void GameObject::OnUpdate(double)
    {
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
