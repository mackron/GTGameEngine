// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponent.hpp>

namespace GT
{
    SceneNodeComponent::SceneNodeComponent(const SceneNodeComponentDescriptor &descriptor)
        : m_descriptor(descriptor), m_pSceneNode(nullptr), m_changeFlags(0)
    {
    }

    SceneNodeComponent::~SceneNodeComponent()
    {
    }


    const SceneNodeComponentDescriptor & SceneNodeComponent::GetDescriptor() const
    {
        return m_descriptor;
    }


    SceneNode* SceneNodeComponent::GetSceneNode() const
    {
        return m_pSceneNode;
    }

    void SceneNodeComponent::_IUO_SetSceneNode(SceneNode* pSceneNode)
    {
        m_pSceneNode = pSceneNode;
    }


    SceneNodeComponentTypeID SceneNodeComponent::GetTypeID() const
    {
        return m_descriptor.GetTypeID();
    }

    const char* SceneNodeComponent::GetName() const
    {
        return m_descriptor.GetName();
    }



    ////////////////////////////////////
    // Internal Use Only

    uint32_t SceneNodeComponent::_GetChangeFlags() const
    {
        return m_changeFlags;
    }

    void SceneNodeComponent::_SetChangeFlags(uint32_t changeFlags)
    {
        m_changeFlags = changeFlags;
    }







    /////////////////////////////////////////////////
    // SceneNodeComponentDescriptor

    SceneNodeComponentTypeID SceneNodeComponentDescriptor::TypeID()
    {
        return SceneNodeComponentType_Unknown;
    }

    const char* SceneNodeComponentDescriptor::Name()
    {
        return "Unknown";
    }



    SceneNodeComponentDescriptor::SceneNodeComponentDescriptor()
    {
    }

    SceneNodeComponentDescriptor::~SceneNodeComponentDescriptor()
    {
    }


    SceneNodeComponentTypeID SceneNodeComponentDescriptor::GetTypeID() const
    {
        return SceneNodeComponentDescriptor::TypeID();
    }

    const char* SceneNodeComponentDescriptor::GetName() const
    {
        return SceneNodeComponentDescriptor::Name();
    }


    SceneNodeComponent* SceneNodeComponentDescriptor::CreateComponent() const
    {
        return nullptr;
    }

    void SceneNodeComponentDescriptor::DeleteComponent(SceneNodeComponent* pComponent) const
    {
        delete pComponent;
    }
}
