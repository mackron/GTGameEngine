// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/Scene.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>
#include <GTGameEngine/Scene/SceneCallback.hpp>
#include <GTGameEngine/Scene/SceneNodeAllocator_Default.hpp>
#include <GTGameEngine/Scene/SceneSteppingPipeline_Default.hpp>

namespace GT
{
    Scene::Scene(EngineContext &engine, SceneCallback &callback)
        : m_engine(engine),
          m_idAllocator(1),
          m_sceneNodes(),
          m_callback(callback),
          m_pSceneNodeAllocator(nullptr),
          m_pSteppingPipeline(nullptr),
          m_cacheLock(),
          m_sceneNodesToInsert(),
          m_sceneNodesToRemove(),
          m_sceneNodesToDelete()
    {
        if (m_pSceneNodeAllocator == nullptr)
        {
            m_pSceneNodeAllocator = new SceneNodeAllocator_Default;
            m_flags |= Flag_OwnsAllocator;
        }

        if (m_pSteppingPipeline == nullptr)
        {
            m_pSteppingPipeline = new SceneSteppingPipeline_Default(callback);
            m_flags |= Flag_OwnsSteppingPipeline;
        }
    }

    Scene::~Scene()
    {
        if ((m_flags & Flag_OwnsAllocator) != 0)
        {
            delete m_pSceneNodeAllocator;
        }

        if ((m_flags & Flag_OwnsSteppingPipeline) != 0)
        {
            delete m_pSteppingPipeline;
        }
    }


    SceneNode* Scene::CreateSceneNode()
    {
        assert(m_pSceneNodeAllocator != nullptr);
        return m_pSceneNodeAllocator->Allocate(*this, m_idAllocator.AllocateID());
    }

    void Scene::DeleteSceneNode(SceneNode* pSceneNode)
    {
        assert(m_pSceneNodeAllocator != nullptr);
        assert(m_pSteppingPipeline   != nullptr);

        // Remove the scene node first.
        this->RemoveSceneNode(pSceneNode);

        // Now try deleting the node for real. If we're in the middle of a step, we need to defer it until the end of the step.
        if (m_pSteppingPipeline->IsStepping())
        {
            this->CacheDeleteSceneNode(pSceneNode);
        }
        else
        {
            m_pSceneNodeAllocator->Delete(pSceneNode);
        }
    }


    bool Scene::InsertSceneNode(SceneNode* pSceneNode)
    {
        assert(m_pSteppingPipeline != nullptr);

        if (pSceneNode != nullptr)
        {
            assert(this->GetSceneNodeByID(pSceneNode->GetID()) == nullptr);       // <-- The scene node of the same ID should not already exist.

            if (m_pSteppingPipeline->IsStepping())
            {
                this->CacheInsertSceneNode(pSceneNode);
            }
            else
            {
                m_pSteppingPipeline->InsertSceneNode(pSceneNode);
            }


            m_cacheLock.Lock();
            {
                m_sceneNodes.Add(pSceneNode->GetID(), pSceneNode);
            }
            m_cacheLock.Unlock();

            return true;
        }

        return false;
    }

    void Scene::RemoveSceneNode(SceneNode* pSceneNode)
    {
        if (pSceneNode != nullptr)
        {
            if (m_pSteppingPipeline->IsStepping())
            {
                this->CacheRemoveSceneNode(pSceneNode);
            }
            else
            {
                m_pSteppingPipeline->RemoveSceneNode(pSceneNode);
            }


            m_cacheLock.Lock();
            {
                m_sceneNodes.RemoveByKey(pSceneNode->GetID());
            }
            m_cacheLock.Unlock();
        }
    }

    void Scene::RemoveAllSceneNodes()
    {
        while (m_sceneNodes.GetCount() > 0)
        {
            this->RemoveSceneNode(m_sceneNodes.buffer[m_sceneNodes.GetCount() - 1]->value);
        }
    }


    SceneNode* Scene::GetSceneNodeByID(uint64_t sceneNodeID)
    {
        auto iSceneNode = m_sceneNodes.Find(sceneNodeID);
        if (iSceneNode != nullptr)
        {
            return iSceneNode->value;
        }

        return nullptr;
    }

    SceneNode* Scene::GetSceneNodeByName(const char* name)
    {
        // TODO: Add support for "<parent>/<child>/<grandchild>/<etc>" format.

        for (size_t iSceneNode = 0; iSceneNode < m_sceneNodes.GetCount(); ++iSceneNode)
        {
            auto pSceneNode = m_sceneNodes.buffer[iSceneNode]->value;
            assert(pSceneNode != nullptr);

            if (strcmp(pSceneNode->GetName(), name) == 0)
            {
                return pSceneNode;
            }
        }

        return nullptr;
    }

    SceneNode* Scene::GetSceneNodeByIndex(unsigned int index)
    {
        return m_sceneNodes.buffer[index]->value;
    }

    unsigned int Scene::GetSceneNodeCount() const
    {
        return static_cast<unsigned int>(m_sceneNodes.GetCount());
    }


    void Scene::Step(double deltaTimeInSeconds)
    {
        assert(m_pSceneNodeAllocator != nullptr);
        assert(m_pSteppingPipeline   != nullptr);

        // Do the main step.
        m_pSteppingPipeline->Step(deltaTimeInSeconds);


        // Cleanup the caches.
        m_cacheLock.Lock();
        {
            // OnSceneNodeRemoved
            //
            // We do this before OnSceneNodeInserted because we want to support a workflow of: Remove Node -> Change Node -> Insert Node. If we
            // were to do this after OnSceneNodeInsert, that workflow would not not work as expected.
            for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesToRemove.GetCount(); ++iSceneNode)
            {
                auto pSceneNode = m_sceneNodesToRemove[iSceneNode];
                assert(pSceneNode != nullptr);
                {
                    m_pSteppingPipeline->RemoveSceneNode(pSceneNode);
                }
            }
            m_sceneNodesToRemove.Clear();


            // OnSceneNodeInserted
            for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesToInsert.GetCount(); ++iSceneNode)
            {
                auto pSceneNode = m_sceneNodesToInsert[iSceneNode];
                assert(pSceneNode != nullptr);
                {
                    m_pSteppingPipeline->InsertSceneNode(pSceneNode);
                }
            }
            m_sceneNodesToInsert.Clear();



            // Delete the nodes waiting to be deleted.
            for (size_t iSceneNode = 0; iSceneNode < m_sceneNodesToDelete.GetCount(); ++iSceneNode)
            {
                auto pSceneNode = m_sceneNodesToDelete[iSceneNode];
                assert(pSceneNode != nullptr);
                {
                    m_pSceneNodeAllocator->Delete(pSceneNode);
                }
            }
            m_sceneNodesToDelete.Clear();
        }
        m_cacheLock.Unlock();
    }



    void Scene::CacheInsertSceneNode(SceneNode* pSceneNode)
    {
        m_cacheLock.Lock();
        {
            m_sceneNodesToInsert.PushBack(pSceneNode);
        }
        m_cacheLock.Unlock();
    }

    void Scene::CacheRemoveSceneNode(SceneNode* pSceneNode)
    {
        m_cacheLock.Lock();
        {
            m_sceneNodesToRemove.PushBack(pSceneNode);
        }
        m_cacheLock.Unlock();
    }

    void Scene::CacheDeleteSceneNode(SceneNode* pSceneNode)
    {
        m_cacheLock.Lock();
        {
            m_sceneNodesToDelete.PushBack(pSceneNode);
        }
        m_cacheLock.Unlock();
    }
}
