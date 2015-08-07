// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/DefaultSceneState.hpp>
#include <GTGameEngine/Scene/SceneEventHandler.hpp>

namespace GT
{
    DefaultSceneState::DefaultSceneState()
        : SceneState(),
          m_sceneNodes(),
          m_pEventHandler(nullptr),
          m_stateFlags(0),
          m_containerLock(),
          m_updatingSceneNodes(),
          m_dynamicSceneNodes(),
          m_sceneNodesTransformedWhileStepping(),
          m_sceneNodesInsertedWhileStepping(),
          m_sceneNodesRemovedWhileStepping(),
          m_sceneNodesDeletedWhileStepping()
    {
    }

    DefaultSceneState::~DefaultSceneState()
    {
    }


    SceneNode* DefaultSceneState::CreateSceneNode(uint64_t sceneNodeID)
    {
        SceneNode* pSceneNode = nullptr;
        m_containerLock.Lock();
        {
            uint64_t sceneNodeIndex;
            pSceneNode = m_sceneNodes.CreateAndInsertSceneNode(sceneNodeID, sceneNodeIndex);
            if (pSceneNode != nullptr)
            {
                m_sceneNodeMap.Add(sceneNodeID, sceneNodeIndex);
            }
        }
        m_containerLock.Unlock();


        return pSceneNode;
    }

    void DefaultSceneState::DeleteSceneNode(uint64_t sceneNodeID)
    {
        SceneNode* pSceneNodeToDelete = this->GetSceneNodeByID(sceneNodeID);
        if (pSceneNodeToDelete != nullptr)
        {
            // We remove the scene node from the scene before deleting.
            this->RemoveSceneNode(pSceneNodeToDelete);


            // Now delete for real.
            pSceneNodeToDelete->_SetFlags(pSceneNodeToDelete->_GetFlags() | SceneNode::_IsDeleted);

            m_containerLock.Lock();
            {
                if ((m_stateFlags & StateFlag_IsStepping) != 0)
                {
                    m_sceneNodesDeletedWhileStepping.PushBack(pSceneNodeToDelete);
                }
                else
                {
                    this->DeleteSceneNodeForReal_NoLock(pSceneNodeToDelete);
                }
            }
            m_containerLock.Unlock();
        }
    }


    bool DefaultSceneState::InsertSceneNode(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);
        
        // We need to post an OnSceneNodeAdded() event. If we are currently stepping, it must be deferred, otherwise it will be posted straight away.
        if ((m_stateFlags & StateFlag_IsStepping) != 0)
        {
            pSceneNode->_SetFlags(pSceneNode->_GetFlags() | SceneNode::_IsDisabled);        // <-- Disable the scene node so that it is not updated during this step.

            m_containerLock.Lock();
            {
                m_sceneNodesInsertedWhileStepping.PushBack(pSceneNode);
            }
            m_containerLock.Unlock();
        }
        else
        {
            this->InsertSceneNodeForReal_NoLock(pSceneNode);
        }

        return true;
    }

    void DefaultSceneState::RemoveSceneNode(SceneNode* pSceneNode)
    {
        assert(pSceneNode != NULL);

        pSceneNode->_SetFlags(pSceneNode->_GetFlags() |  SceneNode::_IsDisabled);
        pSceneNode->_SetFlags(pSceneNode->_GetFlags() & ~SceneNode::_IsInScene);

        // We need to post an OnSceneNodeRemoved() event. If we are current stepping, it must be deferred, otherwise it will be posted straight away.
        if ((m_stateFlags & StateFlag_IsStepping) != 0)
        {
            m_containerLock.Lock();
            {
                // If the scene node was inserted during this same step, remove it from the list.
                m_sceneNodesInsertedWhileStepping.RemoveFirstOccuranceOf(pSceneNode);

                // Defer the delete event until the very end.
                m_sceneNodesRemovedWhileStepping.PushBack(pSceneNode);
            }
            m_containerLock.Unlock();
        }
        else
        {
            this->RemoveSceneNodeForReal_NoLock(pSceneNode);
        }
    }

    
    SceneNode* DefaultSceneState::GetSceneNodeByID(uint64_t sceneNodeID)
    {
        SceneNode* pSceneNode = nullptr;

        m_containerLock.Lock();
        {
            auto iSceneNodeIndex = m_sceneNodeMap.Find(sceneNodeID);
            if (iSceneNodeIndex != nullptr)
            {
                pSceneNode = m_sceneNodes.GetSceneNodeByIndex(iSceneNodeIndex->value);
            }
        }
        m_containerLock.Unlock();

        return pSceneNode;
    }


    void DefaultSceneState::SetEventHandler(SceneEventHandler* pEventHandler)
    {
        m_pEventHandler = pEventHandler;
    }

    SceneEventHandler* DefaultSceneState::GetEventHandler()
    {
        return m_pEventHandler;
    }


    void DefaultSceneState::Step(double deltaTimeInSeconds)
    {
        (void)deltaTimeInSeconds;

        // The rules for the stepping pipeline is as follows:
        //   - Events are never posted until the end of the pipeline, but before cleanup. Events are posted on a single thread, that thread being
        //     the same thread that called this method.
        //   - Scene nodes (and their corresponding components) are not deleted for real until the very end of the pipeline.
        //   - The initial step is multi-threaded, with no guarantees on the order in which scene nodes are processed.

        if ((m_stateFlags & StateFlag_IsStepping) == 0)
        {
            m_stateFlags |= StateFlag_IsStepping;
            {
                // Do the main update.
                this->Update(deltaTimeInSeconds);

                // Do the physics simulation.
                this->DoPhysics(deltaTimeInSeconds);

                // Let the callback know about scene nodes whose world position has changed. This only checks non-static objects.
                this->ResolveTransformations();


                // Post events in one go after everything else have been done. This is done on a single thread, on the same one that called Step().
                this->PostEvents();

                // The final thing to do is cleanup everything that has been marked as deleted.
                this->Cleanup();
            }
            m_stateFlags &= ~StateFlag_IsStepping;
        }
    }



    /////////////////////////////////
    // Priate

    void DefaultSceneState::Update(double deltaTimeInSeconds)
    {
        if (m_pEventHandler != nullptr)
        {
            for (size_t iSceneNode = 0; iSceneNode < m_updatingSceneNodes.GetCount(); ++iSceneNode)
            {
                SceneNode* pSceneNode = m_updatingSceneNodes[iSceneNode];
                assert(pSceneNode != nullptr);

                m_pEventHandler->OnSceneNodeUpdate(pSceneNode, deltaTimeInSeconds);
            }
        }
    }

    void DefaultSceneState::DoPhysics(double deltaTimeInSeconds)
    {
        if (m_pEventHandler != nullptr)
        {
            // TODO: Can optimize this part so that it only iterates over scene nodes with a dynamics component attached.
            //
            // The physics system needs to know which scene nodes have had their transformations explicitly set.
            for (size_t iSceneNode = 0; iSceneNode < m_dynamicSceneNodes.GetCount(); ++iSceneNode)
            {
                SceneNode* pSceneNode = m_dynamicSceneNodes[iSceneNode];
                assert(pSceneNode != nullptr);

                if ((pSceneNode->_GetFlags() & (SceneNode::_PositionChanged | SceneNode::_RotationChanged | SceneNode::_ScaleChanged)) != 0)
                {
                    m_pEventHandler->UpdateSceneNodePhysicsTransform(pSceneNode);
                }
            }


            // Now divert control to the callback and get it to update the physics.
            m_pEventHandler->DoPhysics(deltaTimeInSeconds);
        }
    }

    void DefaultSceneState::ResolveTransformations()
    {
        if (m_pEventHandler != nullptr)
        {

        }
    }

    void DefaultSceneState::PostEvents()
    {
        if (m_pEventHandler != nullptr)
        {
            // OnSceneNodeRemoved
            //
            // We do this before OnSceneNodeInserted because we want to support a workflow of: Remove Node -> Change Node -> Insert Node. If we
            // were to do this after OnSceneNodeInsert, that workflow would not not work as expected.
            for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesRemovedWhileStepping.GetCount(); ++iSceneNode)
            {
                auto pSceneNode = m_sceneNodesRemovedWhileStepping[iSceneNode];
                assert(pSceneNode != nullptr);
                {
                    this->RemoveSceneNodeForReal_NoLock(pSceneNode);
                }
            }

            // OnSceneNodeInserted
            for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesInsertedWhileStepping.GetCount(); ++iSceneNode)
            {
                auto pSceneNode = m_sceneNodesInsertedWhileStepping[iSceneNode];
                assert(pSceneNode != nullptr);
                {
                    this->InsertSceneNodeForReal_NoLock(pSceneNode);
                }
            }
        }
    }

    void DefaultSceneState::Cleanup()
    {
        // Delete the scene nodes for real.
        for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesDeletedWhileStepping.GetCount(); ++iSceneNode)
        {
            auto pSceneNode = m_sceneNodesDeletedWhileStepping[iSceneNode];
            assert(pSceneNode != nullptr);
            {
                this->DeleteSceneNodeForReal_NoLock(pSceneNode);
            }
        }


        // Clear the lists.
        m_sceneNodesInsertedWhileStepping.Clear();
        m_sceneNodesRemovedWhileStepping.Clear();
        m_sceneNodesDeletedWhileStepping.Clear();
    }

    bool DefaultSceneState::GetSceneNodeListIndex(SceneNode* pSceneNode, uint64_t &sceneNodeIndexOut)
    {
        assert(pSceneNode != nullptr);
        {
            auto iSceneNodeIndex = m_sceneNodeMap.Find(pSceneNode->GetID());
            if (iSceneNodeIndex != nullptr)
            {
                sceneNodeIndexOut = iSceneNodeIndex->value;
                return true;
            }
            else
            {
                return false;
            }
        }
    }


    void DefaultSceneState::CacheEvent_OnSceneNodeTransformed(SceneNode* pSceneNode)
    {
        m_containerLock.Lock();
        {
            m_sceneNodesTransformedWhileStepping.PushBack(pSceneNode);
        }
        m_containerLock.Unlock();
    }


    void DefaultSceneState::InsertSceneNodeForReal_NoLock(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);


        if (!pSceneNode->IsUpdatesDisabled())
        {
            m_updatingSceneNodes.PushBack(pSceneNode);
        }

        if (!pSceneNode->IsStatic())
        {
            m_dynamicSceneNodes.PushBack(pSceneNode);
        }

        
        // Enable the scene node.
        pSceneNode->_SetFlags(pSceneNode->_GetFlags() & ~SceneNode::_IsDisabled);
        pSceneNode->_SetFlags(pSceneNode->_GetFlags() |  SceneNode::_IsInScene);


        // Post the event.
        if (m_pEventHandler != nullptr)
        {
            m_pEventHandler->OnSceneNodeInserted(pSceneNode);
        }
    }


    void DefaultSceneState::RemoveSceneNodeForReal_NoLock(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        // Post the event.
        if (m_pEventHandler != nullptr)
        {
            m_pEventHandler->OnSceneNodeRemoved(pSceneNode);
        }
    }


    void DefaultSceneState::DeleteSceneNodeForReal_NoLock(SceneNode* pSceneNode, uint64_t sceneNodeListIndex)
    {
        assert(pSceneNode != nullptr);
        {
            m_sceneNodes.DeleteSceneNodeByIndex(sceneNodeListIndex);
            m_sceneNodeMap.RemoveByKey(pSceneNode->GetID());
        }
    }

    void DefaultSceneState::DeleteSceneNodeForReal_NoLock(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);
        {
            uint64_t sceneNodeIndex;
            if (this->GetSceneNodeListIndex(pSceneNode, sceneNodeIndex))
            {
                this->DeleteSceneNodeForReal_NoLock(pSceneNode, sceneNodeIndex);
            }
            else
            {
                // Should never get here.
                assert(false);
            }
        }
    }
}
