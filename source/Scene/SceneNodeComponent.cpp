// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponent.hpp>

namespace GT
{
    SceneNodeComponent::SceneNodeComponent(SceneNodeComponentTypeID typeID)
        : m_typeID(typeID)
    {
    }


    SceneNodeComponentTypeID SceneNodeComponent::GetTypeID() const
    {
        return m_typeID;
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
