
#include <GTEngine/SceneStateStack.hpp>
#include <GTEngine/Scene.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    SceneStateStack::SceneStateStack(Scene &sceneIn)
        : scene(sceneIn), branches(), masterBranch(*this, nullptr, 0), currentBranch(&masterBranch)
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



    void SceneStateStack::StageInsert(uint64_t sceneNodeID)
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->StageInsert(sceneNodeID);
        }
    }

    void SceneStateStack::StageDelete(uint64_t sceneNodeID)
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->StageDelete(sceneNodeID);
        }
    }

    void SceneStateStack::StageUpdate(uint64_t sceneNodeID)
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->StageUpdate(sceneNodeID);
        }
    }


    void SceneStateStack::Commit()
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->Commit();
        }
    }


    bool SceneStateStack::HasInitialFrame() const
    {
        return this->masterBranch.GetFrameCount() > 0;
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

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
