// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeComponentDescriptor_Events.hpp>
#include <GTGameEngine/Scene/EventsComponent.hpp>

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

    SceneNodeComponent* SceneNodeComponentDescriptor_Events::CreateComponent() const
    {
        return new SceneNodeComponent_Events(*this);
    }

    void SceneNodeComponentDescriptor_Events::DeleteComponent(SceneNodeComponent* pComponent) const
    {
        delete pComponent;
    }
}
