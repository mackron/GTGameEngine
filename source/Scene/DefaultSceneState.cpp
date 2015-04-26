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
          m_sceneNodesCreatedWhileStepping(),
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


        // We need to post an OnSceneNodeInserted() event. If we are currently stepping, it must be deferred, otherwise it will be posted straight away.
        if (pSceneNode != nullptr)
        {
            if ((m_stateFlags & StateFlag_IsStepping) != 0)
            {
                pSceneNode->_SetFlags(pSceneNode->_GetFlags() | SceneNode::_IsDisabled);        // <-- Disable the scene node so that it is not updated during this step.

                m_containerLock.Lock();
                {
                    m_sceneNodesCreatedWhileStepping.PushBack(pSceneNode);
                }
                m_containerLock.Unlock();
            }
            else
            {
                if (m_pEventHandler != nullptr)
                {
                    m_pEventHandler->OnSceneNodeAdded(pSceneNode);
                }
            }
        }

        return pSceneNode;
    }

    void DefaultSceneState::DeleteSceneNode(uint64_t sceneNodeID)
    {
        uint64_t   sceneNodeIndex = 0;
        SceneNode* pSceneNodeToDelete = nullptr;

        m_containerLock.Lock();
        {
            auto iSceneNodeIndex = m_sceneNodeMap.Find(sceneNodeID);
            if (iSceneNodeIndex != nullptr)
            {
                sceneNodeIndex = iSceneNodeIndex->value;

                pSceneNodeToDelete = m_sceneNodes.GetSceneNodeByIndex(sceneNodeIndex);
                assert(pSceneNodeToDelete != nullptr);
                {
                    // Return early if the node is already marked for deletion.
                    if ((pSceneNodeToDelete->_GetFlags() & SceneNode::_IsDeleted) == 0)
                    {
                        pSceneNodeToDelete->_SetFlags(pSceneNodeToDelete->_GetFlags() | SceneNode::_IsDeleted);
                    }
                    else
                    {
                        // The scene node is already marked for deletion.
                        m_containerLock.Unlock();
                        return;
                    }
                }
            }
        }
        m_containerLock.Unlock();


        // We need to post an OnSceneNodeRemoved() event. If we are current stepping, it must be deferred, otherwise it will be posted straight away.
        if (pSceneNodeToDelete != nullptr)
        {
            if ((m_stateFlags & StateFlag_IsStepping) != 0)
            {
                m_containerLock.Lock();
                {
                    // If the scene node was adding during this same step, remove it from the list.
                    m_sceneNodesCreatedWhileStepping.RemoveFirstOccuranceOf(pSceneNodeToDelete);

                    // Defer the delete event until the very end.
                    m_sceneNodesDeletedWhileStepping.PushBack(pSceneNodeToDelete);
                }
                m_containerLock.Unlock();
            }
            else
            {
                if (m_pEventHandler != nullptr)
                {
                    // Post the event before deleting for real.
                    m_pEventHandler->OnSceneNodeRemoved(pSceneNodeToDelete);
                    
                    // Delete the scene node for real.
                    this->DeleteSceneNodeForReal_NoLock(pSceneNodeToDelete, sceneNodeIndex);
                }
            }
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
        // The rules for the stepping pipeline is as follows:
        //   - Events are never posted until the end of the pipeline, but before cleanup. Events are posted on a single thread, that thread being
        //     the same thread that called this method.
        //   - Scene nodes (and their corresponding components) are not deleted for real until the very end of the pipeline.
        //   - The initial step is multi-threaded, with no guarantees on the order in which scene nodes are processed.

        if ((m_stateFlags & StateFlag_IsStepping) == 0)
        {
            m_stateFlags |= StateFlag_IsStepping;
            {




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

    void DefaultSceneState::PostEvents()
    {
        if (m_pEventHandler != nullptr)
        {
            // OnSceneNodeInserted
            for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesCreatedWhileStepping.GetCount(); ++iSceneNode)
            {
                auto pSceneNode = m_sceneNodesCreatedWhileStepping[iSceneNode];
                assert(pSceneNode != nullptr);
                {
                    // Enable the scene node before posting the event.
                    pSceneNode->_SetFlags(pSceneNode->_GetFlags() & ~SceneNode::_IsDisabled);

                    // Post the event.
                    m_pEventHandler->OnSceneNodeAdded(pSceneNode);
                }
            }

            // OnSceneNodeRemoved. Note that we don't clean up at this point. This is done in Cleanup().
            for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesDeletedWhileStepping.GetCount(); ++iSceneNode)
            {
                auto pSceneNode = m_sceneNodesDeletedWhileStepping[iSceneNode];
                assert(pSceneNode != nullptr);
                {
                    // Post the event.
                    m_pEventHandler->OnSceneNodeRemoved(pSceneNode);
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
        m_sceneNodesCreatedWhileStepping.Clear();
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
