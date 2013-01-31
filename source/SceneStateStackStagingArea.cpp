
#include <GTEngine/SceneStateStackStagingArea.hpp>
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneStateStackStagingArea::SceneStateStackStagingArea(SceneStateStackBranch &branchIn)
        : branch(branchIn), inserts(), deletes(), updates()
    {
    }

    SceneStateStackStagingArea::~SceneStateStackStagingArea()
    {
        this->Clear();
    }


    Scene & SceneStateStackStagingArea::GetScene()
    {
        return this->branch.GetScene();
    }

    const Scene & SceneStateStackStagingArea::GetScene() const
    {
        return this->branch.GetScene();
    }


    void SceneStateStackStagingArea::StageInsert(uint64_t sceneNodeID)
    {
        // If a delete command with the scene node is already staged, what we actually want to do is remove the delete command and
        // turn the insert into an update.
        auto iDelete = this->deletes.Find(sceneNodeID);
        if (iDelete != nullptr)
        {
            // Remove the delete command.
            delete iDelete->value;
            this->deletes.RemoveByIndex(iDelete->index);

            // Convert to an update command.
            this->inserts.RemoveFirstOccuranceOf(sceneNodeID);
            this->StageUpdate(sceneNodeID);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->updates.RemoveFirstOccuranceOf(sceneNodeID);

            if (!this->inserts.Exists(sceneNodeID))
            {
                this->inserts.PushBack(sceneNodeID);
            }
        }
    }

    void SceneStateStackStagingArea::StageDelete(uint64_t sceneNodeID)
    {
        // If an insert command with the scene node is already staged, all we want to do is remove it from the inserts and just
        // ignore everything.
        size_t index;
        if (this->inserts.FindFirstIndexOf(sceneNodeID, index))
        {
            this->inserts.Remove(index);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->updates.RemoveFirstOccuranceOf(sceneNodeID);

            if (!this->deletes.Exists(sceneNodeID))
            {
                auto sceneNode = this->GetScene().GetSceneNodeByID(sceneNodeID);
                assert(sceneNode != nullptr);
                {
                    auto sceneNodeSerializer = new GTCore::BasicSerializer;
                    sceneNode->Serialize(*sceneNodeSerializer);

                    this->deletes.Add(sceneNodeID, sceneNodeSerializer);
                }
            }
        }
    }

    void SceneStateStackStagingArea::StageUpdate(uint64_t sceneNodeID)
    {
        // We ignore update commands if an insert or delete command is already present.
        if (!this->inserts.Exists(sceneNodeID) &&
            !this->deletes.Exists(sceneNodeID) &&
            !this->updates.Exists(sceneNodeID))
        {
            this->updates.PushBack(sceneNodeID);
        }
    }


    void SceneStateStackStagingArea::Clear()
    {
        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            delete this->deletes.buffer[i]->value;
        }

        this->inserts.Clear();
        this->deletes.Clear();
        this->updates.Clear();
    }


    void SceneStateStackStagingArea::GetRestoreCommands(SceneStateStackRestoreCommands &commands)
    {
        // We need to do opposites. Inserts become deletes, deletes become inserts and updates are back traced until we find the most recent one.
        
        // Inserts
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto sceneNodeID         = this->inserts[i];
            commands.deletes.Add(sceneNodeID, nullptr);
        }

        // Deletes
        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID         = this->deletes.buffer[i]->key;
            auto sceneNodeSerializer = this->deletes.buffer[i]->value;
            commands.inserts.Add(sceneNodeID, sceneNodeSerializer);
        }

        // Updates
        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto sceneNodeID         = this->updates[i];
            auto sceneNodeSerializer = this->branch.FindMostRecentSerializer(sceneNodeID, this->branch.GetCurrentFrameIndex());
            commands.updates.Add(sceneNodeID, sceneNodeSerializer);
        }
    }
}
