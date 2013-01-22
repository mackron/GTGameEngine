
#include <GTEngine/SceneStateStack.hpp>

namespace GTEngine
{
    SceneStateStack::SceneStateStack()
        : branches(), currentBranch(nullptr)
    {
        // The master branch.
        uint32_t masterID = this->CreateBranch();
        assert(masterID == 0);
        {
            this->SwitchBranch(masterID);
        }
    }

    SceneStateStack::~SceneStateStack()
    {
        for (size_t i = 0; i < this->branches.count; ++i)
        {
            delete this->branches.buffer[i]->value;
        }
    }


    uint32_t SceneStateStack::CreateBranch()
    {
        assert(this->branches.count > 0);
        {
            uint32_t newID = this->GetNewBranchID();

            if (this->currentBranch != nullptr)
            {
                this->branches.Add(newID, new SceneStateStackBranch(this->currentBranch, this->currentBranch->GetCurrentFrameIndex()));
            }
            else
            {
                this->branches.Add(newID, new SceneStateStackBranch(nullptr, 0));
            }

            return newID;
        }
    }

    bool SceneStateStack::SwitchBranch(uint32_t branchID)
    {
        auto iBranch = this->branches.Find(branchID);
        if (iBranch != nullptr)
        {
            auto branch = iBranch->value;
            assert(branch != nullptr);
            {
                this->currentBranch = branch;
            }

            return true;
        }

        return false;
    }






    ////////////////////////////////////////////////////////
    // Private

    uint32_t SceneStateStack::GetNewBranchID() const
    {
        if (this->branches.count > 0)
        {
            return this->branches.buffer[this->branches.count - 1]->key + 1;
        }
        else
        {
            return 0;
        }
    }
}