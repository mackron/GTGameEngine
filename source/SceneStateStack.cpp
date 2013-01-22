
#include <GTEngine/SceneStateStack.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneStateStack::SceneStateStack(Scene &sceneIn)
        : scene(sceneIn), branches(), masterBranch(nullptr, 0), currentBranch(&masterBranch)
    {
        // Add the master branch.
        this->branches.Add(0, &this->masterBranch);
    }

    SceneStateStack::~SceneStateStack()
    {
    }


    uint32_t SceneStateStack::CreateBranch()
    {
        assert(this->currentBranch != nullptr);
        {
            uint32_t newID = this->GetNewBranchID();
            assert(newID > 0);
            {
                this->branches.Add(newID, this->currentBranch->CreateBranch());
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


    void SceneStateStack::AppendFrame()
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->AppendFrame();
        }
    }



    ////////////////////////////////////////////////////////
    // Private

    uint32_t SceneStateStack::GetNewBranchID() const
    {
        assert(this->branches.count > 0);
        {
            return this->branches.buffer[this->branches.count - 1]->key + 1;
        }
    }
}