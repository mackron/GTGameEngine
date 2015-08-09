// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponentDescriptor.hpp>

namespace GT
{
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
}
