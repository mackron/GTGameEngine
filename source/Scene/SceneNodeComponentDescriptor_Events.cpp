// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponentDescriptor_Events.hpp>

namespace GT
{
    SceneNodeComponentTypeID SceneNodeComponentDescriptor_Events::TypeID()
    {
        return SceneNodeComponentType_Events;
    }

    const char* SceneNodeComponentDescriptor_Events::Name()
    {
        return "Events";
    }



    SceneNodeComponentDescriptor_Events::SceneNodeComponentDescriptor_Events()
    {
    }

    SceneNodeComponentDescriptor_Events::~SceneNodeComponentDescriptor_Events()
    {
    }

    SceneNodeComponentTypeID SceneNodeComponentDescriptor_Events::GetTypeID() const
    {
        return SceneNodeComponentDescriptor_Events::TypeID();
    }

    const char* SceneNodeComponentDescriptor_Events::GetName() const
    {
        return SceneNodeComponentDescriptor_Events::Name();
    }
}
