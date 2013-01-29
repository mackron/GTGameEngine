
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/SceneStateStack.hpp>
#include <GTEngine/Scene.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    SceneStateStackBranch::SceneStateStackBranch(SceneStateStack &stateStackIn, SceneStateStackBranch* parentIn, uint32_t rootFrameIndexIn)
        : stateStack(stateStackIn),
          parent(parentIn), children(), rootFrameIndex(rootFrameIndexIn),
          frames(), currentFrameIndex(rootFrameIndexIn),
          stagingArea(*this)
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


        this->ClearStagingArea();
    }


    uint32_t SceneStateStackBranch::GetMaxFrameIndex() const
    {
        if (this->parent != nullptr)
        {
            return this->rootFrameIndex + this->frames.count;
        }
        else
        {
            assert(this->frames.count > 0);
            {
                return this->frames.count - 1;
            }
        }
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


    SceneStateStackFrame* SceneStateStackBranch::GetFrameAtIndex(uint32_t index) const
    {
        assert(index <= this->GetMaxFrameIndex());

        if (this->parent != nullptr)
        {
            if (index > this->rootFrameIndex)
            {
                return this->frames[index - this->rootFrameIndex - 1];
            }
            else
            {
                return this->parent->GetFrameAtIndex(index);
            }
        }
        else
        {
            // It's the master branch.
            return this->frames[index];
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
        this->stagingArea.StageInsert(sceneNodeID);
    }

    void SceneStateStackBranch::StageDelete(uint64_t sceneNodeID)
    {
        this->stagingArea.StageDelete(sceneNodeID);
    }

    void SceneStateStackBranch::StageUpdate(uint64_t sceneNodeID)
    {
        this->stagingArea.StageUpdate(sceneNodeID);
    }


    void SceneStateStackBranch::ClearStagingArea()
    {
        this->stagingArea.Clear();
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
        if (this->parent == nullptr || this->currentFrameIndex > this->rootFrameIndex)
        {
            uint32_t newCount = (this->parent == nullptr) ? this->currentFrameIndex + 1 : this->currentFrameIndex - this->rootFrameIndex;

            while (this->frames.count > newCount)
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
        this->frames.PushBack(new SceneStateStackFrame(*this, this->stagingArea));

        // The newly committed frame is the current one.
        this->currentFrameIndex = this->GetMaxFrameIndex();


        // The staging area must be cleared after every commit.
        this->ClearStagingArea();
    }


    void SceneStateStackBranch::Seek(int step)
    {
        auto &scene = this->GetScene();

        bool reenableStaging = scene.IsStateStackStagingEnabled();
        scene.DisableStateStackStaging();
        {
            auto newFrameIndex = static_cast<uint32_t>(GTCore::Clamp(static_cast<int>(this->currentFrameIndex) + step, 0, static_cast<int>(this->GetMaxFrameIndex())));


            // The first thing to do is revert the staging area.
            this->RevertStagingArea();


            // With the staging area reverted, we can now grab the revert commands like normal.
            SceneStateStackRestoreCommands restoreCommands;
            this->GetRestoreCommands(newFrameIndex, restoreCommands);

            // And now we just execute the commands.
            restoreCommands.Execute(this->GetScene());


            // The current frame index can now be changed. This must be done last.
            this->currentFrameIndex = newFrameIndex;
        }
        if (reenableStaging) { this->GetScene().EnableStateStackStaging(); }
    }

    void SceneStateStackBranch::RevertStagingArea()
    {
        auto &scene = this->GetScene();

        bool reenableStaging = scene.IsStateStackStagingEnabled();
        scene.DisableStateStackStaging();
        {
            // We'll need to grab the revert commands first.
            SceneStateStackRestoreCommands restoreCommands;
            this->stagingArea.GetRestoreCommands(restoreCommands);

            // Now we execute the commands.
            restoreCommands.Execute(this->GetScene());
        }
        if (reenableStaging) { this->GetScene().EnableStateStackStaging(); }
    }

    void SceneStateStackBranch::ApplyToScene()
    {
        // Just seeking nowhere will actually revert everything to show the current state.
        this->Seek(0);
    }

    void SceneStateStackBranch::GetRestoreCommands(uint32_t newFrameIndex, SceneStateStackRestoreCommands &commands)
    {
        uint32_t oldFrameIndex = this->currentFrameIndex;

        // We need to do this differently depending on whether or not we are moving backwards. If we're going forward, we just insert
        // commands like normal, making sure we keep the most up-to-date serialized data. If we're going backwards, we want to invert
        // the operations (insert = delete, delete = insert and update = most recent).
        //
        // What we'll do is add the commands with a null serializer and then once they're all added, we'll find the most recent serializers.
        if (newFrameIndex > oldFrameIndex)
        {
            // Moving forwards.
            for (size_t i = oldFrameIndex + 1; i <= newFrameIndex; ++i)
            {
                auto frame = this->GetFrameAtIndex(i);
                assert(frame != nullptr);
                {
                    auto &inserts = frame->GetInserts();
                    auto &deletes = frame->GetDeletes();
                    auto &updates = frame->GetUpdates();

                    for (size_t i = 0; i < inserts.count; ++i)
                    {
                        commands.AddInsert(inserts.buffer[i]->key, nullptr);
                    }

                    for (size_t i = 0; i < deletes.count; ++i)
                    {
                        commands.AddDelete(deletes.buffer[i]->key, nullptr);
                    }

                    for (size_t i = 0; i < updates.count; ++i)
                    {
                        commands.AddUpdate(updates.buffer[i]->key, nullptr);
                    }
                }
            }
        }
        else if (newFrameIndex < oldFrameIndex)
        {
            // Moving backwards.
            for (size_t i = oldFrameIndex + 1; i > newFrameIndex + 1; --i)
            {
                auto frame = this->GetFrameAtIndex(i - 1);
                assert(frame != nullptr);
                {
                    auto &inserts = frame->GetInserts();
                    auto &deletes = frame->GetDeletes();
                    auto &updates = frame->GetUpdates();

                    for (size_t i = 0; i < inserts.count; ++i)
                    {
                        commands.AddDelete(inserts.buffer[i]->key, nullptr);
                    }

                    for (size_t i = 0; i < deletes.count; ++i)
                    {
                        commands.AddInsert(deletes.buffer[i]->key, nullptr);
                    }

                    for (size_t i = 0; i < updates.count; ++i)
                    {
                        commands.AddUpdate(updates.buffer[i]->key, nullptr);
                    }
                }
            }
        }


        // And now we need to update to the most recent serializers.
        commands.UpdateToMostRecentSerializers(*this, newFrameIndex);
    }


    GTCore::BasicSerializer* SceneStateStackBranch::FindMostRecentSerializer(uint64_t sceneNodeID, uint32_t startFrameIndex) const
    {
        // We start from the current frame and then loop backwards until we find a frame with serialized data for the given scene node.

        for (uint32_t i = startFrameIndex + 1; i > 0; --i)
        {
            auto frame = this->GetFrameAtIndex(i - 1);
            assert(frame != nullptr);
            {
                auto serializer = frame->GetSerializer(sceneNodeID);
                if (serializer != nullptr)
                {
                    return serializer;
                }
            }
        }

        return nullptr;
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

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

