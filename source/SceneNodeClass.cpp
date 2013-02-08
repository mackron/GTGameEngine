// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneNodeClass.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    SceneNodeClass::SceneNodeClass()
        : serializers(), hierarchy(), nextID(0)
    {
    }

    SceneNodeClass::~SceneNodeClass()
    {
        this->Clear();
    }



    void SceneNodeClass::SetFromSceneNode(const SceneNode &sceneNode)
    {
        // First, we clear everything.
        this->Clear();

        // We need to grab every scene node in the hierarchy, serialize them, and add them to the internal containers. Of course, this needs to be done recursively.
        this->AddSceneNode(sceneNode, 0, 0);
    }



    ////////////////////////////////////////////
    // Serialization.

    bool SceneNodeClass::Serialize(GTCore::Serializer &serializer)
    {
        // We'll need to use an intermediary serializer so we can get an accurate size.
        GTCore::BasicSerializer intermediarySerializer;
        
        // Serializers first.
        intermediarySerializer.Write(static_cast<uint32_t>(this->serializers.count));

        for (size_t i = 0; i < this->serializers.count; ++i)
        {
            auto sceneNodeID         = this->serializers.buffer[i]->key;
            auto sceneNodeSerializer = this->serializers.buffer[i]->value;

            assert(sceneNodeID         != 0);
            assert(sceneNodeSerializer != nullptr);
            {
                intermediarySerializer.Write(sceneNodeID);

                intermediarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
                intermediarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
            }
        }


        // Hierarchy.
        intermediarySerializer.Write(static_cast<uint32_t>(this->hierarchy.count));

        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            uint64_t sceneNodeID       = this->hierarchy.buffer[i]->key;
            uint64_t parentSceneNodeID = this->hierarchy.buffer[i]->value;

            intermediarySerializer.Write(sceneNodeID);
            intermediarySerializer.Write(parentSceneNodeID);
        }


        // Additional Data.
        intermediarySerializer.Write(this->nextID);



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneNodeClass;
        header.version     = 1;
        header.sizeInBytes = static_cast<uint32_t>(intermediarySerializer.GetBufferSizeInBytes());

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);


        return true;
    }

    bool SceneNodeClass::Deserialize(GTCore::Deserializer &deserializer)
    {
        // We want to clear first.
        this->Clear();

        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneNodeClass);
            {
                switch (header.version)
                {
                case 1:
                    {
                        // Serializers.
                        uint32_t serializerCount;
                        deserializer.Read(serializerCount);

                        for (uint32_t i = 0; i < serializerCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);


                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            auto buffer = malloc(serializerSizeInBytes);
                            deserializer.Read(buffer, serializerSizeInBytes);

                            auto serializer = new GTCore::BasicSerializer;
                            serializer->Write(buffer, serializerSizeInBytes);

                            free(buffer);


                            this->serializers.Add(sceneNodeID, serializer);
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


                        // Additional Data.
                        deserializer.Read(this->nextID);


                        break;
                    }


                default:
                    {
                        GTEngine::Log("Error deserializing SceneNode. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        return false;
                    }
                }
            }
        }


        return true;
    }



    /////////////////////////////////////////////////////
    // Private

    void SceneNodeClass::Clear()
    {
        // Serializers.
        for (size_t i = 0; i < this->serializers.count; ++i)
        {
            delete this->serializers.buffer[i]->value;
        }
        this->serializers.Clear();


        // Hierarchy.
        this->hierarchy.Clear();


        this->nextID = 0;
    }

    void SceneNodeClass::AddSceneNode(const SceneNode &sceneNode, uint64_t id, uint64_t parentID)
    {
        // We want to serialize the scene node and add it to the hierarchy first.
        auto serializer = new GTCore::BasicSerializer;
        sceneNode.Serialize(*serializer, SceneNode::NoID);

        if (id == 0)
        {
            id = ++this->nextID;
        }

        this->serializers.Add(id, serializer);


        if (parentID != 0)
        {
            this->hierarchy.Add(id, parentID);
        }

        for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            this->AddSceneNode(*childNode, 0, id);
        }
    }
}
