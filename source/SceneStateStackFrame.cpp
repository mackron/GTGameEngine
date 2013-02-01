
#include <GTEngine/SceneStateStackFrame.hpp>
#include <GTEngine/SceneStateStackBranch.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    SceneStateStackFrame::SceneStateStackFrame(SceneStateStackBranch &branchIn, const SceneStateStackStagingArea &stagingArea)
        : branch(branchIn),
          serializedInserts(), serializedDeletes(), serializedUpdates()
    {
        auto &stagedInserts = stagingArea.GetInserts();
        auto &stagedDeletes = stagingArea.GetDeletes();
        auto &stagedUpdates = stagingArea.GetUpdates();



        for (size_t i = 0; i < stagedInserts.count; ++i)
        {
            auto sceneNodeID         = stagedInserts[i];
            auto sceneNodeSerializer = new GTCore::BasicSerializer;
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedInserts.Add(sceneNodeID, sceneNodeSerializer);
        }

        for (size_t i = 0; i < stagedDeletes.count; ++i)
        {
            auto sceneNodeID         = stagedDeletes.buffer[i]->key;
            auto sceneNodeSerializer = new GTCore::BasicSerializer(*stagedDeletes.buffer[i]->value);
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedDeletes.Add(sceneNodeID, sceneNodeSerializer);
        }

        for (size_t i = 0; i < stagedUpdates.count; ++i)
        {
            auto sceneNodeID         = stagedUpdates[i];
            auto sceneNodeSerializer = new GTCore::BasicSerializer;
            this->SerializeSceneNode(sceneNodeID, *sceneNodeSerializer);

            this->serializedUpdates.Add(sceneNodeID, sceneNodeSerializer);
        }
    }

    SceneStateStackFrame::SceneStateStackFrame(SceneStateStackBranch &branchIn, GTCore::Deserializer &deserializer)
        : branch(branchIn),
          serializedInserts(), serializedDeletes(), serializedUpdates()
    {
        this->Deserialize(deserializer);
    }

    SceneStateStackFrame::~SceneStateStackFrame()
    {
        this->Clear();
    }


    Scene & SceneStateStackFrame::GetScene()
    {
        return this->branch.GetScene();
    }

    const Scene & SceneStateStackFrame::GetScene() const
    {
        return this->branch.GetScene();
    }


    GTCore::BasicSerializer* SceneStateStackFrame::GetSerializer(uint64_t sceneNodeID) const
    {
        auto iSerializer = this->serializedInserts.Find(sceneNodeID);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }

        iSerializer = this->serializedDeletes.Find(sceneNodeID);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }
        
        iSerializer = this->serializedUpdates.Find(sceneNodeID);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }


        return nullptr;
    }



    /////////////////////////////////////////////////
    // Serialization/Deserialization

    void SceneStateStackFrame::Serialize(GTCore::Serializer &serializer) const
    {
        // We need to use an intermediary serializer so we can get an accurate size.
        GTCore::BasicSerializer intermediarySerializer;
        

        // Inserts
        intermediarySerializer.Write(static_cast<uint32_t>(this->serializedInserts.count));
        
        for (size_t i = 0; i < this->serializedInserts.count; ++i)
        {
            auto sceneNodeID         = this->serializedInserts.buffer[i]->key;
            auto sceneNodeSerializer = this->serializedInserts.buffer[i]->value;

            intermediarySerializer.Write(sceneNodeID);

            intermediarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            intermediarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }


        // Deletes
        intermediarySerializer.Write(static_cast<uint32_t>(this->serializedDeletes.count));

        for (size_t i = 0; i < this->serializedDeletes.count; ++i)
        {
            auto sceneNodeID         = this->serializedDeletes.buffer[i]->key;
            auto sceneNodeSerializer = this->serializedDeletes.buffer[i]->value;

            intermediarySerializer.Write(sceneNodeID);

            intermediarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            intermediarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }


        // Updates
        intermediarySerializer.Write(static_cast<uint32_t>(this->serializedUpdates.count));

        for (size_t i = 0; i < this->serializedUpdates.count; ++i)
        {
            auto sceneNodeID         = this->serializedUpdates.buffer[i]->key;
            auto sceneNodeSerializer = this->serializedUpdates.buffer[i]->value;

            intermediarySerializer.Write(sceneNodeID);

            intermediarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            intermediarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneStateStackFrame;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), intermediarySerializer.GetBufferSizeInBytes());
    }

    void SceneStateStackFrame::Deserialize(GTCore::Deserializer &deserializer)
    {
        // We best make sure the frame is cleared before we load anything.
        this->Clear();


        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneStateStackFrame);
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
                            // write that to a new GTCore::BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new GTCore::BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);


                            this->serializedInserts.Add(sceneNodeID, sceneNodeSerializer);


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
                            // write that to a new GTCore::BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new GTCore::BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);


                            this->serializedDeletes.Add(sceneNodeID, sceneNodeSerializer);


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
                            // write that to a new GTCore::BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new GTCore::BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);


                            this->serializedUpdates.Add(sceneNodeID, sceneNodeSerializer);


                            free(serializerData);
                        }



                        break;
                    }

                default:
                    {
                        GTEngine::Log("Error deserializing SceneStateStackFrame. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        break;
                    }
                }
            }
        }
    }



    //////////////////////////////////////////////////////
    // Private

    bool SceneStateStackFrame::SerializeSceneNode(uint64_t sceneNodeID, GTCore::Serializer &serializer) const
    {
        auto sceneNode = this->GetScene().GetSceneNodeByID(sceneNodeID);
        if (sceneNode != nullptr)
        {
            sceneNode->Serialize(serializer);
            return true;
        }

        return false;
    }

    void SceneStateStackFrame::Clear()
    {
        for (size_t i = 0; i < this->serializedInserts.count; ++i)
        {
            delete this->serializedInserts.buffer[i]->value;
        }
        this->serializedInserts.Clear();


        for (size_t i = 0; i < this->serializedDeletes.count; ++i)
        {
            delete this->serializedDeletes.buffer[i]->value;
        }
        this->serializedDeletes.Clear();


        for (size_t i = 0; i < this->serializedUpdates.count; ++i)
        {
            delete this->serializedUpdates.buffer[i]->value;
        }
        this->serializedUpdates.Clear();
    }
}
