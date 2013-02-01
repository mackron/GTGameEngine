
#include <GTEngine/SceneStateStackStagingArea.hpp>
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>

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




    /////////////////////////////////////////////////
    // Serialization/Deserialization

    void SceneStateStackStagingArea::Serialize(GTCore::Serializer &serializer) const
    {
        // We need to use an intermediary serializer to get an accurate size.
        GTCore::BasicSerializer intermediarySerializer;


        // Inserts.
        intermediarySerializer.Write(static_cast<uint32_t>(this->inserts.count));

        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            intermediarySerializer.Write(this->inserts[i]);
        }


        // Deletes.
        intermediarySerializer.Write(static_cast<uint32_t>(this->deletes.count));

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID         = this->deletes.buffer[i]->key;
            auto sceneNodeSerializer = this->deletes.buffer[i]->value;

            intermediarySerializer.Write(sceneNodeID);

            intermediarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            intermediarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }


        // Deletes.
        intermediarySerializer.Write(static_cast<uint32_t>(this->updates.count));

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            intermediarySerializer.Write(this->updates[i]);
        }



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneStateStackStagingArea;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), intermediarySerializer.GetBufferSizeInBytes());
    }

    void SceneStateStackStagingArea::Deserialize(GTCore::Deserializer &deserializer)
    {
        // We should clear the staging area just in case.
        this->Clear();


        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneStateStackStagingArea);
            {
                switch (header.version)
                {
                case 1:
                    {
                        // Inserts.
                        uint32_t insertsCount;
                        deserializer.Read(insertsCount);

                        for (uint32_t i = 0; i < insertsCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            this->inserts.PushBack(sceneNodeID);
                        }


                        // Deletes.
                        uint32_t deletesCount;
                        deserializer.Read(deletesCount);

                        for (uint32_t i = 0; i < deletesCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            // The next chunk of data is the serialized data of the scene node. What we do here is ready the data into a temp buffer, and then
                            // write that to a new GTCore::BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new GTCore::BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);


                            this->deletes.Add(sceneNodeID, sceneNodeSerializer);


                            free(serializerData);
                        }


                        // Updates.
                        uint32_t updatesCount;
                        deserializer.Read(updatesCount);

                        for (uint32_t i = 0; i < updatesCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            this->updates.PushBack(sceneNodeID);
                        }


                        break;
                    }


                default:
                    {
                        GTEngine::Log("Error deserializing SceneStateStackStagingArea. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        break;
                    }
                }
            }
        }
    }
}
