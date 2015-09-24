// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/EventsComponent.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    SceneNodeComponentTypeID EventsComponentDescriptor::TypeID()
    {
        return SceneNodeComponentType_Events;
    }

    const char* EventsComponentDescriptor::Name()
    {
        return "Events";
    }


    EventsComponentDescriptor::EventsComponentDescriptor()
    {
    }

    EventsComponentDescriptor::~EventsComponentDescriptor()
    {
    }

    SceneNodeComponentTypeID EventsComponentDescriptor::GetTypeID() const
    {
        return EventsComponentDescriptor::TypeID();
    }

    const char* EventsComponentDescriptor::GetName() const
    {
        return EventsComponentDescriptor::Name();
    }

    SceneNodeComponent* EventsComponentDescriptor::CreateComponent() const
    {
        return new EventsComponent(*this);
    }

    void EventsComponentDescriptor::DeleteComponent(SceneNodeComponent* pComponent) const
    {
        delete pComponent;
    }




    EventsComponent::EventsComponent(const EventsComponentDescriptor &descriptor)
        : SceneNodeComponent(descriptor),
          m_onUpdate(nullptr), m_onPostUpdate(nullptr)
    {
    }

    EventsComponent::~EventsComponent()
    {
    }


    bool EventsComponent::IsOnUpdateEnabled() const
    {
        return m_onUpdate != nullptr;
    }

    bool EventsComponent::IsOnPostUpdateEnabled() const
    {
        return m_onPostUpdate != nullptr;
    }


    void EventsComponent::OnUpdate(EventsComponent::OnUpdateProc proc)
    {
        m_onUpdate = proc;
    }

    void EventsComponent::OnPostUpdate(EventsComponent::OnPostUpdateProc proc)
    {
        m_onPostUpdate = proc;
    }


    void EventsComponent::PostOnUpdate(double deltaTimeInSeconds)
    {
        if (m_onUpdate != nullptr)
        {
            m_onUpdate(deltaTimeInSeconds);
        }
    }

    void EventsComponent::PostOnPostUpdate(double deltaTimeInSeconds)
    {
        if (m_onPostUpdate != nullptr)
        {
            m_onPostUpdate(deltaTimeInSeconds);
        }
    }
}
