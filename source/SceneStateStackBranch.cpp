
#include <GTEngine/SceneStateStackBranch.hpp>

namespace GTEngine
{
    SceneStateStackBranch::SceneStateStackBranch(SceneStateStackBranch* parentIn, uint32_t rootFrameIndexIn)
        : parent(parentIn), children(), rootFrameIndex(rootFrameIndexIn),
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


    SceneStateStackBranch* SceneStateStackBranch::CreateBranch()
    {
        assert(this->currentFrameIndex >  this->rootFrameIndex);
        assert(this->currentFrameIndex <= this->rootFrameIndex + this->frames.count);
        {
            auto child = new SceneStateStackBranch(this, this->currentFrameIndex);
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


    void SceneStateStackBranch::AppendFrame()
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
                }
            }
        }
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
