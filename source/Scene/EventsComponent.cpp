// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/EventsComponent.hpp>
#include <GTGameEngine/EngineContext.hpp>

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




    SceneNodeComponent_Events::SceneNodeComponent_Events(const SceneNodeComponentDescriptor_Events &descriptor)
        : SceneNodeComponent(descriptor),
          m_onUpdate(nullptr), m_onPostUpdate(nullptr)
    {
    }

    SceneNodeComponent_Events::~SceneNodeComponent_Events()
    {
    }


    bool SceneNodeComponent_Events::IsOnUpdateEnabled() const
    {
        return m_onUpdate != nullptr;
    }

    bool SceneNodeComponent_Events::IsOnPostUpdateEnabled() const
    {
        return m_onPostUpdate != nullptr;
    }


    void SceneNodeComponent_Events::OnUpdate(SceneNodeComponent_Events::OnUpdateProc proc)
    {
        m_onUpdate = proc;
    }

    void SceneNodeComponent_Events::OnPostUpdate(SceneNodeComponent_Events::OnPostUpdateProc proc)
    {
        m_onPostUpdate = proc;
    }


    void SceneNodeComponent_Events::PostOnUpdate(double deltaTimeInSeconds)
    {
        if (m_onUpdate != nullptr)
        {
            m_onUpdate(deltaTimeInSeconds);
        }
    }

    void SceneNodeComponent_Events::PostOnPostUpdate(double deltaTimeInSeconds)
    {
        if (m_onPostUpdate != nullptr)
        {
            m_onPostUpdate(deltaTimeInSeconds);
        }
    }
}
