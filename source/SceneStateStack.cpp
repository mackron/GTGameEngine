// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneStateStack.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>

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
        return this->masterBranch.GetLocalFrameCount() > 0;
    }


    void SceneStateStack::Seek(int step)
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->Seek(step);
        }
    }

    void SceneStateStack::RevertStagingArea()
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->RevertStagingArea();
        }
    }

    void SceneStateStack::ClearStagingArea()
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->ClearStagingArea();
        }
    }

    void SceneStateStack::ApplyToScene()
    {
        assert(this->currentBranch != nullptr);
        {
            this->currentBranch->ApplyToScene();
        }
    }



    /////////////////////////////////////////////////
    // Serialization/Deserialization

    void SceneStateStack::Serialize(GTCore::Serializer &serializer) const
    {
        // We need to use an intermediary here so we can grab the size properly.
        GTCore::BasicSerializer intermediarySerializer;

        // The hierarchy.
        GTCore::Map<uint32_t, uint32_t> hierarchy;

        for (size_t i = 0; i < this->branches.count; ++i)
        {
            auto branch = this->branches.buffer[i]->value;
            assert(branch != nullptr);
            {
                uint32_t branchID = this->branches.buffer[i]->key;

                if (branch->GetParent() != nullptr)
                {
                    hierarchy.Add(branchID, this->GetBranchID(*branch->GetParent()));
                }
            }
        }


        intermediarySerializer.Write(static_cast<uint32_t>(hierarchy.count));
        
        for (size_t i = 0; i < hierarchy.count; ++i)
        {
            uint32_t branchID       = hierarchy.buffer[i]->key;
            uint32_t parentBranchID = hierarchy.buffer[i]->value;

            intermediarySerializer.Write(branchID);
            intermediarySerializer.Write(parentBranchID);
        }




        // Branches.
        intermediarySerializer.Write(static_cast<uint32_t>(this->branches.count));

        for (size_t i = 0; i < this->branches.count; ++i)
        {
            auto branch = this->branches.buffer[i]->value;
            assert(branch != nullptr);
            {
                uint32_t branchID = this->branches.buffer[i]->key;

                intermediarySerializer.Write(branchID);
                branch->Serialize(intermediarySerializer);
            }
        }




        // Now we can write everything to the main serializer.
        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneStateStack;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void SceneStateStack::Deserialize(GTCore::Deserializer &deserializer)
    {
        // We need to clear the stack before deserializing.
        this->Clear();


        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneStateStack);
            {
                switch (header.version)
                {
                case 1:
                    {
                        // Hierarchy
                        GTCore::Map<uint32_t, uint32_t> hierarchy;

                        uint32_t hierarchyCount;
                        deserializer.Read(hierarchyCount);

                        for (uint32_t i = 0; i < hierarchyCount; ++i)
                        {
                            uint32_t branchID;
                            deserializer.Read(branchID);

                            uint32_t parentBranchID;
                            deserializer.Read(parentBranchID);

                            hierarchy.Add(branchID, parentBranchID);
                        }


                        // Branches.
                        //
                        // The way branches are created is such that the parent's will always be the first to be deserialized. Thus, we can safely straight away
                        // create the branches and attach.
                        //
                        // The first branch will always be the master branch, and needs to be handled differently.
                        uint32_t branchCount;
                        deserializer.Read(branchCount);

                        for (size_t i = 0; i < branchCount; ++i)
                        {
                            uint32_t branchID;
                            deserializer.Read(branchID);

                            if (branchID == 0)      // <-- Is it the master branch?
                            {
                                assert(i == 0);     // <-- Master branch must always be the first one.
                                {
                                    this->masterBranch.Deserialize(deserializer);
                                }
                            }
                            else
                            {
                                auto iParentID = hierarchy.Find(branchID);
                                assert(iParentID != nullptr);
                                {
                                    auto parentBranch = this->GetBranchByID(iParentID->value);
                                    assert(parentBranch != nullptr);                                // <-- Parents should always be created first, so this can be asserted.
                                    {
                                        this->branches.Add(branchID, parentBranch->CreateBranch(deserializer));
                                    }
                                }
                            }
                        }


                        break;
                    }

                default:
                    {
                        GTEngine::Log("Error deserializing SceneStateStack. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        break;
                    }
                }
            }
        }
    }

    void SceneStateStack::Clear()
    {
        this->masterBranch.Clear();
        
        this->branches.Clear();
        this->branches.Add(0, &this->masterBranch);

        this->currentBranch = &this->masterBranch;
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

    uint32_t SceneStateStack::GetBranchID(const SceneStateStackBranch &branch) const
    {
        assert(this->branches.count > 0);
        {
            for (size_t i = 0; i < this->branches.count; ++i)
            {
                if (this->branches.buffer[i]->value == &branch)
                {
                    return this->branches.buffer[i]->key;
                }
            }
        }

        // We should never actually get here.
        assert(false);
        return 0;
    }

    SceneStateStackBranch* SceneStateStack::GetBranchByID(uint32_t branchID) const
    {
        assert(this->branches.count > 0);
        {
            auto iBranch = this->branches.Find(branchID);
            if (iBranch != nullptr)
            {
                return iBranch->value;
            }
        }

        return nullptr;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
