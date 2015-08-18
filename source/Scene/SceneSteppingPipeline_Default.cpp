// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneSteppingPipeline_Default.hpp>
#include <GTGameEngine/Scene/SceneCallback.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>
#include <cassert>

namespace GT
{
    // Must be 8 bytes.
    struct SceneNodeAUXData
    {
        // Various flags describing the scene node, specific to the default pipeline.
        uint32_t flags;

        // Unused at the moment.
        uint32_t padding;
    };

    #define SCENE_NODE_AUX_DATA(pSceneNode) (reinterpret_cast<SceneNodeAUXData*>(pSceneNode) - 1)

    #define SCENE_NODE_FLAG_REQUIRES_UPDATE     (1 << 0)
    #define SCENE_NODE_FLAG_REQUIRES_POSTUPDATE (1 << 1)
    #define SCENE_NODE_FLAG_DYNAMIC             (1 << 2)
    #define SCENE_NODE_FLAG_PHYSICS             (1 << 3)


    SceneSteppingPipeline_Default::SceneSteppingPipeline_Default(SceneCallback &callback)
        : SceneSteppingPipeline(),
          m_callback(callback),
          m_stateFlags(),
          m_sceneNodesRequiringUpdate(),
          m_sceneNodesRequiringPostUpdate(),
          m_dynamicSceneNodes(),
          m_dynamicPhysicsSceneNodes()
    {
    }

    
    void SceneSteppingPipeline_Default::InsertSceneNode(SceneNode* pSceneNode)
    {
        assert(pSceneNode != NULL);

        SceneNodeAUXData* pAUXData = SCENE_NODE_AUX_DATA(pSceneNode);
        pAUXData->flags   = 0;
        pAUXData->padding = 0;

        // Place the scene node into it's appropriate pools.
        {
            if (m_callback.DoesSceneNodeRequireUpdate(pSceneNode))
            {
                pAUXData->flags |= SCENE_NODE_FLAG_REQUIRES_UPDATE;
                m_sceneNodesRequiringUpdate.PushBack(pSceneNode);
            }

            if (m_callback.DoesSceneNodeRequirePostUpdate(pSceneNode))
            {
                pAUXData->flags |= SCENE_NODE_FLAG_REQUIRES_POSTUPDATE;
                m_sceneNodesRequiringPostUpdate.PushBack(pSceneNode);
            }

            if (!m_callback.IsSceneNodeStatic(pSceneNode))
            {
                pAUXData->flags |= SCENE_NODE_FLAG_DYNAMIC;
                m_dynamicSceneNodes.PushBack(pSceneNode);

                if (m_callback.IsPhysicsObject(pSceneNode))
                {
                    pAUXData->flags |= SCENE_NODE_FLAG_PHYSICS;
                    m_dynamicPhysicsSceneNodes.PushBack(pSceneNode);
                }
            }
        }


        // Let the callback know that the scene node has been added.
        //
        // TODO: Pass in absolute transformation instead of local.
        m_callback.OnSceneNodeInserted(pSceneNode, pSceneNode->GetPosition(), pSceneNode->GetRotation(), pSceneNode->GetScale());
    }

    void SceneSteppingPipeline_Default::RemoveSceneNode(SceneNode* pSceneNode)
    {
        assert(pSceneNode != NULL);

        SceneNodeAUXData* pAUXData = SCENE_NODE_AUX_DATA(pSceneNode);
        assert(pAUXData != nullptr);

        if ((pAUXData->flags & SCENE_NODE_FLAG_REQUIRES_UPDATE) != 0)
        {
            m_sceneNodesRequiringUpdate.RemoveFirstOccuranceOf(pSceneNode);
        }
        
        if ((pAUXData->flags & SCENE_NODE_FLAG_REQUIRES_POSTUPDATE) != 0)
        {
            m_sceneNodesRequiringPostUpdate.RemoveFirstOccuranceOf(pSceneNode);
        }

        if ((pAUXData->flags & SCENE_NODE_FLAG_DYNAMIC) != 0)
        {
            m_dynamicSceneNodes.RemoveFirstOccuranceOf(pSceneNode);
        }

        if ((pAUXData->flags & SCENE_NODE_FLAG_PHYSICS) != 0)
        {
            m_dynamicPhysicsSceneNodes.RemoveFirstOccuranceOf(pSceneNode);
        }


        // Let the callback know that the scene node has been removed.
        m_callback.OnSceneNodeRemoved(pSceneNode);
    }


    void SceneSteppingPipeline_Default::Step(double deltaTimeInSeconds)
    {
        if ((m_stateFlags & StateFlag_IsStepping) == 0)
        {
            m_stateFlags |= StateFlag_IsStepping;
            {
                // Do the main update.
                this->Update(deltaTimeInSeconds);

                // Do the physics simulation.
                this->StepPhysics(deltaTimeInSeconds);


                // Let the callback know about scene nodes whose world position has changed. This only checks non-static objects.
                //this->ResolveTransformations();


                // Do the post-update.
                this->PostUpdate(deltaTimeInSeconds);

                // Post events in one go after everything else have been done. This is done on a single thread, on the same one that called Step().
                this->PostEvents();

                // The final thing to do is cleanup everything that has been marked as deleted.
                this->Cleanup();
            }
            m_stateFlags &= ~StateFlag_IsStepping;
        }
    }

    bool SceneSteppingPipeline_Default::IsStepping() const
    {
        return (m_stateFlags & StateFlag_IsStepping) != 0;
    }



    ///////////////////////////////////
    // Private

    void SceneSteppingPipeline_Default::Update(double deltaTimeInSeconds)
    {
        for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesRequiringUpdate.GetCount(); ++iSceneNode)
        {
            auto pSceneNode = m_sceneNodesRequiringUpdate[iSceneNode];
            assert(pSceneNode != nullptr);

            m_callback.UpdateSceneNode(pSceneNode, deltaTimeInSeconds);
        }
    }

    void SceneSteppingPipeline_Default::StepPhysics(double deltaTimeInSeconds)
    {
        // The physics system needs to know which scene nodes have had their transformations explicitly set.
        for (size_t iSceneNode = 0; iSceneNode < m_dynamicPhysicsSceneNodes.GetCount(); ++iSceneNode)
        {
            SceneNode* pSceneNode = m_dynamicPhysicsSceneNodes[iSceneNode];
            assert(pSceneNode != nullptr);

            if ((pSceneNode->_GetFlags() & (SceneNode::_PositionChanged | SceneNode::_RotationChanged | SceneNode::_ScaleChanged)) != 0)
            {
                m_callback.UpdateSceneNodePhysicsTransform(pSceneNode);
            }
        }

        
        // Now do the actual physics simulation.
        m_callback.StepPhysics(deltaTimeInSeconds);
    }

    void SceneSteppingPipeline_Default::PostUpdate(double deltaTimeInSeconds)
    {
        for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesRequiringPostUpdate.GetCount(); ++iSceneNode)
        {
            auto pSceneNode = m_sceneNodesRequiringPostUpdate[iSceneNode];
            assert(pSceneNode != nullptr);

            m_callback.PostUpdateSceneNode(pSceneNode, deltaTimeInSeconds);
        }
    }

    void SceneSteppingPipeline_Default::PostEvents()
    {
        // OnSceneNodeTransformed
        for (size_t iSceneNode = 0; iSceneNode < m_dynamicSceneNodes.GetCount(); ++iSceneNode)
        {
            auto pSceneNode = m_dynamicSceneNodes[iSceneNode];
            assert(pSceneNode);

            uint32_t flags = pSceneNode->_GetFlags();
            if ((flags & (SceneNode::_PositionChanged | SceneNode::_RotationChanged | SceneNode::_ScaleChanged)) != 0)
            {
                m_callback.OnSceneNodeTransformed(pSceneNode, pSceneNode->GetPosition(), pSceneNode->GetRotation(), pSceneNode->GetScale());

                pSceneNode->_SetFlags(flags & ~(SceneNode::_PositionChanged | SceneNode::_RotationChanged | SceneNode::_ScaleChanged));
            }
        }
    }
        
    void SceneSteppingPipeline_Default::Cleanup()
    {
    }
}
