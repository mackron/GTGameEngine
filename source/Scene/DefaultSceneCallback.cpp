// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/DefaultSceneCallback.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>
#include <GTGameEngine/Scene/SceneNodeComponent_Graphics.hpp>
#include <GTGameEngine/Scene/SceneNodeComponent_Events.hpp>

namespace GT
{
    DefaultSceneCallback::~DefaultSceneCallback()
    {
    }


    void DefaultSceneCallback::OnSceneNodeInserted(SceneNode* pSceneNode, const vec4 &worldPosition, const quat &worldRotation, const vec4 &worldScale)
    {
         assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_GRAPHICS)
        auto pGraphicsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Graphics>();
        if (pGraphicsComponent != nullptr)
        {
            pGraphicsComponent->AddModelToGraphicsWorld(worldPosition, worldRotation, worldScale);
        }
#else
        (void)pSceneNode;
#endif
    }

    void DefaultSceneCallback::OnSceneNodeRemoved(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_GRAPHICS)
        auto pGraphicsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Graphics>();
        if (pGraphicsComponent != nullptr)
        {
            pGraphicsComponent->RemoveModelFromGraphicsWorld();
        }
#else
        (void)pSceneNode;
#endif
    }

    void DefaultSceneCallback::OnSceneNodeTransformed(SceneNode* pSceneNode, const vec4 &worldPosition, const quat &worldRotation, const vec4 &worldScale)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_GRAPHICS)
        auto pGraphicsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Graphics>();
        if (pGraphicsComponent != nullptr)
        {
            pGraphicsComponent->SetModelTransform(worldPosition, worldRotation, worldScale);
        }
#else
        (void)pSceneNode;
#endif
    }



    void DefaultSceneCallback::UpdateSceneNode(SceneNode* pSceneNode, double deltaTimeInSeconds)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_EVENTS)
        auto pEventsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Events>();
        if (pEventsComponent != nullptr)
        {
            pEventsComponent->PostOnUpdate(deltaTimeInSeconds);
        }
#else
        (void)pSceneNode;
        (void)deltaTimeInSeconds;
#endif
    }

    void DefaultSceneCallback::PostUpdateSceneNode(SceneNode* pSceneNode, double deltaTimeInSeconds)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_EVENTS)
        auto pEventsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Events>();
        if (pEventsComponent != nullptr)
        {
            pEventsComponent->PostOnPostUpdate(deltaTimeInSeconds);
        }
#else
        (void)pSceneNode;
        (void)deltaTimeInSeconds;
#endif
    }

    void DefaultSceneCallback::UpdateSceneNodePhysicsTransform(SceneNode* pSceneNode)
    {
        (void)pSceneNode;
    }

    void DefaultSceneCallback::StepPhysics(double deltaTimeInSeconds)
    {
        (void)deltaTimeInSeconds;
    }



    bool DefaultSceneCallback::DoesSceneNodeRequireUpdate(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_EVENTS)
        auto pEventsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Events>();
        if (pEventsComponent != nullptr)
        {
            if (pEventsComponent->IsOnUpdateEnabled())
            {
                return true;
            }
        }
#else
        (void)pSceneNode;
#endif


        return false;
    }

    bool DefaultSceneCallback::DoesSceneNodeRequirePostUpdate(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_EVENTS)
        auto pEventsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Events>();
        if (pEventsComponent != nullptr)
        {
            if (pEventsComponent->IsOnPostUpdateEnabled())
            {
                return true;
            }
        }
#else
        (void)pSceneNode;
#endif


        return false;
    }

    bool DefaultSceneCallback::IsSceneNodeStatic(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
        return false;
    }

    bool DefaultSceneCallback::IsPhysicsObject(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
        return false;
    }
}
