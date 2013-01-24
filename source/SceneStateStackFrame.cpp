
#include <GTEngine/SceneStateStackFrame.hpp>
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneStateStackFrame::SceneStateStackFrame(SceneStateStackBranch &branchIn, const GTCore::Vector<uint64_t> &insertIDs, const GTCore::Vector<uint64_t> &deleteIDs, const GTCore::Vector<uint64_t> &updateIDs)
        : branch(branchIn),
          serializedInserts(), serializedDeletes(), serializedUpdates()
    {
        for (size_t i = 0; i < insertIDs.count; ++i)
        {
            auto sceneNodeID         = insertIDs[i];
            auto sceneNodeSerializer = new GTCore::BasicSerializer;
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedInserts.Add(sceneNodeID, sceneNodeSerializer);
        }

        for (size_t i = 0; i < deleteIDs.count; ++i)
        {
            auto sceneNodeID         = insertIDs[i];
            auto sceneNodeSerializer = new GTCore::BasicSerializer;
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedDeletes.Add(sceneNodeID, sceneNodeSerializer);
        }

        for (size_t i = 0; i < updateIDs.count; ++i)
        {
            auto sceneNodeID         = insertIDs[i];
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
