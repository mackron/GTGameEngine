
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/SceneStateStack.hpp>

namespace GTEngine
{
    SceneStateStackBranch::SceneStateStackBranch(SceneStateStack &stateStackIn, SceneStateStackBranch* parentIn, uint32_t rootFrameIndexIn)
        : stateStack(stateStackIn),
          parent(parentIn), children(), rootFrameIndex(rootFrameIndexIn),
          frames(), currentFrameIndex(rootFrameIndexIn)
    {
    }

    SceneStateStackBranch::~SceneStateStackBranch()
    {
        this->DeleteAllBranches();

        for (size_t i = 0; i < this->frames.count; ++i)
        {
            delete this->frames[i];
        }
    }


    Scene & SceneStateStackBranch::GetScene()
    {
        return this->stateStack.GetScene();
    }

    const Scene & SceneStateStackBranch::GetScene() const
    {
        return this->stateStack.GetScene();
    }


    void SceneStateStackBranch::SetParent(SceneStateStackBranch* newParent, uint32_t newRootFrameIndex)
    {
        this->parent            = newParent;
        this->rootFrameIndex    = newRootFrameIndex;
        this->currentFrameIndex = newRootFrameIndex;


        // Children and local frames need to be deleted.
        this->DeleteAllBranches();


        for (size_t i = 0; i < this->frames.count; ++i)
        {
            auto frame = this->frames[i];
            assert(frame != nullptr);
            {
                delete frame;
            }
        }
        this->frames.Clear();
    }


    size_t SceneStateStackBranch::GetTotalFrameCount() const
    {
        if (this->parent != nullptr)
        {
            return this->rootFrameIndex + 1 + this->frames.count;
        }
        else
        {
            return this->frames.count;
        }
    }


    SceneStateStackBranch* SceneStateStackBranch::CreateBranch()
    {
        assert(this->currentFrameIndex >  this->rootFrameIndex);
        assert(this->currentFrameIndex <= this->rootFrameIndex + this->frames.count);
        {
            auto child = new SceneStateStackBranch(this->stateStack, this, this->currentFrameIndex);
            this->children.PushBack(child);

            return child;
        }
    }

    void SceneStateStackBranch::DeleteBranch(SceneStateStackBranch* childBranch)
    {
        size_t childIndex;
        if (this->children.FindFirstIndexOf(childBranch, childIndex))
        {
            this->_DeleteBranchByIndex(childIndex);
        }
    }

    void SceneStateStackBranch::DeleteAllBranches()
    {
        for (size_t i = 0; i < this->children.count; ++i)
        {
            delete this->children[i];
        }
        this->children.Clear();
    }


    void SceneStateStackBranch::StageInsert(uint64_t sceneNodeID)
    {
        // If a delete command with the scene node is already staged, all we want to do is remove it from the deletes and just
        // ignore everything.
        size_t index;
        if (this->stagedDeletes.FindFirstIndexOf(sceneNodeID, index))
        {
            this->stagedDeletes.Remove(index);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->stagedUpdates.RemoveFirstOccuranceOf(sceneNodeID);

            if (!this->stagedInserts.Exists(sceneNodeID))
            {
                this->stagedInserts.PushBack(sceneNodeID);
            }
        }
    }

    void SceneStateStackBranch::StageDelete(uint64_t sceneNodeID)
    {
        // If an insert command with the scene node is already staged, all we want to do is remove it from the inserts and just
        // ignore everything.
        size_t index;
        if (this->stagedInserts.FindFirstIndexOf(sceneNodeID, index))
        {
            this->stagedInserts.Remove(index);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->stagedUpdates.RemoveFirstOccuranceOf(sceneNodeID);

            if (!this->stagedDeletes.Exists(sceneNodeID))
            {
                this->stagedDeletes.PushBack(sceneNodeID);
            }
        }
    }

    void SceneStateStackBranch::StageUpdate(uint64_t sceneNodeID)
    {
        // We ignore update commands if an insert or delete command is already present.
        if (!this->stagedInserts.Exists(sceneNodeID) &&
            !this->stagedDeletes.Exists(sceneNodeID) &&
            !this->stagedUpdates.Exists(sceneNodeID))
        {
            this->stagedUpdates.PushBack(sceneNodeID);
        }
    }


    void SceneStateStackBranch::ClearStagingArea()
    {
        this->stagedDeletes.Clear();
        this->stagedInserts.Clear();
        this->stagedUpdates.Clear();
    }


    void SceneStateStackBranch::Commit()
    {
        if (this->currentFrameIndex < this->rootFrameIndex)
        {
            // We need to reparent the branch. The new parent will be the one where the current frame index is one of it's local frames.
            auto newParent = this->FindBranch(this->currentFrameIndex);
            assert(newParent != nullptr);
            {
                this->SetParent(newParent, this->currentFrameIndex);
            }
        }



        // All child branches that are rooted after the current frame need to be deleted.
        for (size_t i = 0; i < this->children.count; )
        {
            auto child = this->children[i];
            assert(child != nullptr);
            {
                if (child->GetRootFrameIndex() > this->currentFrameIndex)
                {
                    this->_DeleteBranchByIndex(i);
                }
                else
                {
                    ++i;
                }
            }
        }

        // Now the frames coming after the current one need to be deleted.
        if (this->currentFrameIndex > this->rootFrameIndex)
        {
            while (this->frames.count > this->currentFrameIndex - this->rootFrameIndex)
            {
                auto frame = this->frames.GetBack();
                assert(frame != nullptr);
                {
                    this->frames.PopBack();
                    delete frame;
                }
            }
        }


        // Now we can append the new frame.
        this->frames.PushBack(new SceneStateStackFrame(*this, this->stagedInserts, this->stagedDeletes, this->stagedUpdates));


        // The staging area must be cleared after every commit.
        this->ClearStagingArea();
    }




    /////////////////////////////////////////////
    // Internal Use Only

    void SceneStateStackBranch::_DeleteBranchByIndex(size_t childIndex)
    {
        auto child = this->children[childIndex];
        assert(child != nullptr);
        {
            this->children.Remove(childIndex);
            delete child;
        }
    }





    /////////////////////////////////////////////
    // Private

    SceneStateStackBranch* SceneStateStackBranch::FindBranch(uint32_t frameIndex)
    {
        if (frameIndex <= this->rootFrameIndex)
        {
            if (this->parent != nullptr)
            {
                return this->parent->FindBranch(frameIndex);
            }
        }

        return this;
    }
}
