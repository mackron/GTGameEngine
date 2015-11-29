// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/SceneStateStackRestoreCommands.hpp>
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>

namespace GT
{
    SceneStateStackRestoreCommands::SceneStateStackRestoreCommands(unsigned int serializationFlags, unsigned int deserializationFlags)
        : inserts(), deletes(), updates(), hierarchy(), serializationFlags(serializationFlags), deserializationFlags(deserializationFlags)
    {
    }

    SceneStateStackRestoreCommands::~SceneStateStackRestoreCommands()
    {
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto iCommand = this->inserts.buffer[i];
            if (iCommand->value.isOwner)
            {
                delete iCommand->value.serializer;
            }
        }

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto iCommand = this->deletes.buffer[i];
            if (iCommand->value.isOwner)
            {
                delete iCommand->value.serializer;
            }
        }

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto iCommand = this->updates.buffer[i];
            if (iCommand->value.isOwner)
            {
                delete iCommand->value.serializer;
            }
        }
    }


    void SceneStateStackRestoreCommands::AddInsert(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene* scene, BasicSerializer* sceneNodeSerializer)
    {
        // If a delete command with the scene node is already staged, all we want to do is remove it from the deletes and just
        // ignore everything.
        auto iDelete = this->deletes.Find(sceneNodeID);
        if (iDelete != nullptr)
        {
            this->deletes.RemoveByIndex(iDelete->index);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->updates.RemoveByKey(sceneNodeID);

            if (!this->inserts.Exists(sceneNodeID))
            {
                bool ownsSerializer = false;

                if (sceneNodeSerializer == nullptr)
                {
                    if (scene != nullptr)
                    {
                        auto sceneNode = scene->GetSceneNodeByID(sceneNodeID);
                        if (sceneNode != nullptr)
                        {
                            sceneNodeSerializer = new BasicSerializer;
                            sceneNode->Serialize(*sceneNodeSerializer, this->serializationFlags);

                            ownsSerializer = true;
                        }
                    }
                }

                this->inserts.Add(sceneNodeID, Command(sceneNodeSerializer, ownsSerializer));
                this->AddToHierarchy(sceneNodeID, parentSceneNodeID);
            }
        }
    }

    void SceneStateStackRestoreCommands::AddDelete(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene* scene, BasicSerializer* sceneNodeSerializer)
    {
        (void)parentSceneNodeID;
        (void)scene;
        (void)sceneNodeSerializer;


        // If an insert command with the scene node is already staged, all we want to do is remove it from the inserts and just
        // ignore everything.
        auto iInsert = this->inserts.Find(sceneNodeID);
        if (iInsert != nullptr)
        {
            this->inserts.RemoveByIndex(iInsert->index);
        }
        else
        {
            this->updates.RemoveByKey(sceneNodeID);

            if (!this->deletes.Exists(sceneNodeID))
            {
                this->deletes.Add(sceneNodeID, sceneNodeSerializer);
            }
        }


        // Always ensure the node is not referenced in the hierarchy.
        this->RemoveFromHierarchy(sceneNodeID);
    }

    void SceneStateStackRestoreCommands::AddUpdate(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene* scene, BasicSerializer* sceneNodeSerializer)
    {
        // We ignore update commands if an insert or delete command is already present.
        if (!this->inserts.Exists(sceneNodeID) &&
            !this->deletes.Exists(sceneNodeID) &&
            !this->updates.Exists(sceneNodeID))
        {
            bool ownsSerializer = false;

            if (sceneNodeSerializer == nullptr)
            {
                if (scene != nullptr)
                {
                    auto sceneNode = scene->GetSceneNodeByID(sceneNodeID);
                    if (sceneNode != nullptr)
                    {
                        sceneNodeSerializer = new BasicSerializer;
                        sceneNode->Serialize(*sceneNodeSerializer, this->serializationFlags);

                        ownsSerializer = true;
                    }
                }
            }

            this->updates.Add(sceneNodeID, Command(sceneNodeSerializer, ownsSerializer));
        }


        // Always make sure the hierarchy is updated.
        this->AddToHierarchy(sceneNodeID, parentSceneNodeID);
    }


    void SceneStateStackRestoreCommands::AddToHierarchy(uint64_t sceneNodeID, uint64_t parentSceneNodeID)
    {
        this->hierarchy.Add(sceneNodeID, parentSceneNodeID);
    }

    void SceneStateStackRestoreCommands::RemoveFromHierarchy(uint64_t sceneNodeID)
    {
        this->hierarchy.RemoveByKey(sceneNodeID);
    }


    void SceneStateStackRestoreCommands::UpdateToMostRecentSerializers(SceneStateStackBranch &branch, uint32_t startFrameIndex)
    {
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto iCommand = this->inserts.buffer[i];
            assert(iCommand != nullptr);
            {
                auto  sceneNodeID         = iCommand->key;
                auto &sceneNodeSerializer = iCommand->value;

                sceneNodeSerializer = branch.FindMostRecentSerializer(sceneNodeID, startFrameIndex);
            }
        }

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto iCommand = this->deletes.buffer[i];
            assert(iCommand != nullptr);
            {
                auto  sceneNodeID         = iCommand->key;
                auto &sceneNodeSerializer = iCommand->value;

                sceneNodeSerializer = branch.FindMostRecentSerializer(sceneNodeID, startFrameIndex);
            }
        }

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto iCommand = this->updates.buffer[i];
            assert(iCommand != nullptr);
            {
                auto  sceneNodeID         = iCommand->key;
                auto &sceneNodeSerializer = iCommand->value;

                sceneNodeSerializer = branch.FindMostRecentSerializer(sceneNodeID, startFrameIndex);
            }
        }
    }

    void SceneStateStackRestoreCommands::UpdateToMostRecentHierarchy(SceneStateStackBranch &branch, uint32_t startFrameIndex)
    {
        for (size_t i = 0; i < this->hierarchy.count; )
        {
            auto iParentID = this->hierarchy.buffer[i];
            assert(iParentID != nullptr);
            {
                auto sceneNodeID = iParentID->key;

                // If the scene node ID is in the deletes commands, we don't want it to be in the heirarchy.
                if (this->deletes.Exists(sceneNodeID))
                {
                    this->hierarchy.RemoveByKey(sceneNodeID);
                }
                else
                {
                    iParentID->value = branch.FindMostRecentParentSceneNodeID(sceneNodeID, startFrameIndex);
                    ++i;
                }
            }
        }
    }


    void SceneStateStackRestoreCommands::Clear()
    {
        this->inserts.Clear();
        this->deletes.Clear();
        this->updates.Clear();

        this->hierarchy.Clear();
    }

    void SceneStateStackRestoreCommands::Execute(Scene &scene)
    {
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto serializer = this->inserts.buffer[i]->value.serializer;
            assert(serializer != nullptr);
            {
                BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
                scene.CreateNewSceneNode(deserializer, this->deserializationFlags);
            }
        }

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID = this->deletes.buffer[i]->key;
            assert(sceneNodeID != 0);
            {
                scene.RemoveSceneNodeByID(sceneNodeID);
            }
        }

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto sceneNodeID         = this->updates.buffer[i]->key;
            auto sceneNodeSerializer = this->updates.buffer[i]->value.serializer;

            assert(sceneNodeID         != 0);
            assert(sceneNodeSerializer != nullptr);
            {
                auto sceneNode = scene.GetSceneNodeByID(sceneNodeID);
                assert(sceneNode != nullptr);
                {
                    BasicDeserializer deserializer(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
                    sceneNode->Deserialize(deserializer, this->deserializationFlags);
                }
            }
        }


        // Hierarchy re-construction.
        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            auto sceneNodeID       = this->hierarchy.buffer[i]->key;
            auto parentSceneNodeID = this->hierarchy.buffer[i]->value;

            assert(sceneNodeID != 0);
            {
                auto sceneNode = scene.GetSceneNodeByID(sceneNodeID);
                assert(sceneNode != nullptr);
                {
                    if (parentSceneNodeID != 0)
                    {
                        auto parentSceneNode = scene.GetSceneNodeByID(parentSceneNodeID);
                        assert(parentSceneNode != nullptr);
                        {
                            parentSceneNode->AttachChild(*sceneNode);
                        }
                    }
                    else
                    {
                        sceneNode->DetachFromParent(false);
                    }
                }
            }
        }
    }



    /////////////////////////////////////////////////
    // Serialization/Deserialization

    void SceneStateStackRestoreCommands::Serialize(Serializer &serializer) const
    {
        BasicSerializer secondarySerializer;

        // Inserts.
        secondarySerializer.Write(static_cast<uint32_t>(this->inserts.count));

        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            uint64_t sceneNodeID         = this->inserts.buffer[i]->key;
            auto     sceneNodeSerializer = this->inserts.buffer[i]->value.serializer;

            secondarySerializer.Write(sceneNodeID);
            
            secondarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            secondarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }


        // Deletes.
        secondarySerializer.Write(static_cast<uint32_t>(this->deletes.count));

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID         = this->deletes.buffer[i]->key;
            auto sceneNodeSerializer = this->deletes.buffer[i]->value.serializer;

            secondarySerializer.Write(sceneNodeID);

            secondarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            secondarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }


        // Updates.
        secondarySerializer.Write(static_cast<uint32_t>(this->updates.count));

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            uint64_t sceneNodeID         = this->updates.buffer[i]->key;
            auto     sceneNodeSerializer = this->updates.buffer[i]->value.serializer;

            secondarySerializer.Write(sceneNodeID);
            
            secondarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            secondarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }



        // Hierarchy.
        secondarySerializer.Write(static_cast<uint32_t>(this->hierarchy.count));

        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            secondarySerializer.Write(this->hierarchy.buffer[i]->key);
            secondarySerializer.Write(this->hierarchy.buffer[i]->value);
        }




        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneStateStackRestoreCommands;
        header.version     = 1;
        header.sizeInBytes = secondarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(secondarySerializer.GetBuffer(), secondarySerializer.GetBufferSizeInBytes());
    }

    void SceneStateStackRestoreCommands::Deserialize(Deserializer &deserializer)
    {
        // Clear first, just in case.
        this->Clear();

        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneStateStackRestoreCommands);
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

                            // The next chunk of data is the serialized data of the scene node. What we do here is ready the data into a temp buffer, and then
                            // write that to a new BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);

                            this->inserts.Add(sceneNodeID, Command(sceneNodeSerializer, true));


                            free(serializerData);
                        }


                        // Deletes.
                        uint32_t deletesCount;
                        deserializer.Read(deletesCount);

                        for (uint32_t i = 0; i < deletesCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            // The next chunk of data is the serialized data of the scene node. What we do here is ready the data into a temp buffer, and then
                            // write that to a new BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);


                            this->deletes.Add(sceneNodeID, Command(sceneNodeSerializer, true));


                            free(serializerData);
                        }


                        // Updates.
                        uint32_t updatesCount;
                        deserializer.Read(updatesCount);

                        for (uint32_t i = 0; i < updatesCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            // The next chunk of data is the serialized data of the scene node. What we do here is ready the data into a temp buffer, and then
                            // write that to a new BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);

                            this->updates.Add(sceneNodeID, Command(sceneNodeSerializer, true));


                            free(serializerData);
                        }



                        // Hierarchy.
                        uint32_t hierarchyCount;
                        deserializer.Read(hierarchyCount);

                        for (uint32_t i = 0; i < hierarchyCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            uint64_t parentSceneNodeID;
                            deserializer.Read(parentSceneNodeID);

                            this->hierarchy.Add(sceneNodeID, parentSceneNodeID);
                        }


                        break;
                    }


                default:
                    {
                        Log("Error deserializing SceneStateStackRestoreCommands. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        break;
                    }
                }
            }
        }
    }
}
