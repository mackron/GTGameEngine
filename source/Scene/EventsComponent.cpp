// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/EventsComponent.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
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
