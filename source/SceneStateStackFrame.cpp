
#include <GTEngine/SceneStateStackFrame.hpp>
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneStateStackFrame::SceneStateStackFrame(SceneStateStackBranch &branchIn, const SceneStateStackStagingArea &stagingArea)
        : branch(branchIn),
          serializedInserts(), serializedDeletes(), serializedUpdates()
    {
        auto &stagedInserts = stagingArea.GetInserts();
        auto &stagedDeletes = stagingArea.GetDeletes();
        auto &stagedUpdates = stagingArea.GetUpdates();



        for (size_t i = 0; i < stagedInserts.count; ++i)
        {
            auto sceneNodeID         = stagedInserts[i];
            auto sceneNodeSerializer = new GTCore::BasicSerializer;
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedInserts.Add(sceneNodeID, sceneNodeSerializer);
        }

        for (size_t i = 0; i < stagedDeletes.count; ++i)
        {
            auto sceneNodeID         = stagedDeletes.buffer[i]->key;
            auto sceneNodeSerializer = new GTCore::BasicSerializer(*stagedDeletes.buffer[i]->value);
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedDeletes.Add(sceneNodeID, sceneNodeSerializer);
        }

        for (size_t i = 0; i < stagedUpdates.count; ++i)
        {
            auto sceneNodeID         = stagedUpdates[i];
            auto sceneNodeSerializer = new GTCore::BasicSerializer;
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedUpdates.Add(sceneNodeID, sceneNodeSerializer);
        }
    }

    SceneStateStackFrame::~SceneStateStackFrame()
    {
        for (size_t i = 0; i < this->serializedInserts.count; ++i)
        {
            delete this->serializedInserts.buffer[i]->value;
        }

        for (size_t i = 0; i < this->serializedDeletes.count; ++i)
        {
            delete this->serializedDeletes.buffer[i]->value;
        }

        for (size_t i = 0; i < this->serializedUpdates.count; ++i)
        {
            delete this->serializedUpdates.buffer[i]->value;
        }
    }


    Scene & SceneStateStackFrame::GetScene()
    {
        return this->branch.GetScene();
    }

    const Scene & SceneStateStackFrame::GetScene() const
    {
        return this->branch.GetScene();
    }


    GTCore::BasicSerializer* SceneStateStackFrame::GetSerializer(uint64_t sceneNodeID) const
    {
        auto iSerializer = this->serializedInserts.Find(sceneNodeID);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }

        iSerializer = this->serializedDeletes.Find(sceneNodeID);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }
        
        iSerializer = this->serializedUpdates.Find(sceneNodeID);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }


        return nullptr;
    }



    //////////////////////////////////////////////////////
    // Private

    bool SceneStateStackFrame::SerializeSceneNode(uint64_t sceneNodeID, GTCore::Serializer &serializer) const
    {
        auto sceneNode = this->GetScene().GetSceneNodeByID(sceneNodeID);
        if (sceneNode != nullptr)
        {
            sceneNode->Serialize(serializer);
            return true;
        }

        return false;
    }
}
