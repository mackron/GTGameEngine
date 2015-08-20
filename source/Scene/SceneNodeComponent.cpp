// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponent.hpp>

namespace GT
{
    SceneNodeComponent::SceneNodeComponent(SceneNodeComponentDescriptor &descriptor)
        : m_descriptor(descriptor), m_changeFlags(0)
    {
    }


    SceneNodeComponentDescriptor & SceneNodeComponent::GetDescriptor() const
    {
        return m_descriptor;
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
}
