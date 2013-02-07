// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneNodeClass.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    SceneNodeClass::SceneNodeClass()
        : serializers(), hierarchy()
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
        this->AddSceneNode(sceneNode, static_cast<size_t>(-1));
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
            auto sceneNodeSerializer = this->serializers[i];
            assert(sceneNodeSerializer != nullptr);
            {
                intermediarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
                intermediarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
            }
        }


        // Hierarchy.
        intermediarySerializer.Write(static_cast<uint32_t>(this->hierarchy.count));

        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            uint32_t sceneNodeIndex       = static_cast<uint32_t>(this->hierarchy.buffer[i]->key);
            uint32_t parentSceneNodeIndex = static_cast<uint32_t>(this->hierarchy.buffer[i]->value);

            intermediarySerializer.Write(sceneNodeIndex);
            intermediarySerializer.Write(parentSceneNodeIndex);
        }



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
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            auto buffer = malloc(serializerSizeInBytes);
                            deserializer.Read(buffer, serializerSizeInBytes);

                            auto serializer = new GTCore::BasicSerializer;
                            serializer->Write(buffer, serializerSizeInBytes);

                            free(buffer);

                            this->serializers.PushBack(serializer);
                        }


                        // Hierarchy.
                        uint32_t hierarchyCount;
                        deserializer.Read(hierarchyCount);

                        for (uint32_t i = 0; i < hierarchyCount; ++i)
                        {
                            uint32_t sceneNodeIndex;
                            deserializer.Read(sceneNodeIndex);

                            uint32_t parentSceneNodeIndex;
                            deserializer.Read(parentSceneNodeIndex);

                            this->hierarchy.Add(sceneNodeIndex, parentSceneNodeIndex);
                        }


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
            delete this->serializers[i];
        }
        this->serializers.Clear();


        // Hierarchy.
        this->hierarchy.Clear();
    }

    void SceneNodeClass::AddSceneNode(const SceneNode &sceneNode, size_t parentIndex)
    {
        // We want to serialize the scene node and add it to the hierarchy first.
        auto serializer = new GTCore::BasicSerializer;
        sceneNode.Serialize(*serializer, SceneNode::NoID | SceneNode::NoEditorMetadataComponent);

        this->serializers.PushBack(serializer);
        
        // We will grab this index of the node in the hierarchy, which will be the count, minus 1.
        size_t sceneNodeIndex = this->serializers.count - 1;


        if (parentIndex != static_cast<size_t>(-1))
        {
            this->hierarchy.Add(sceneNodeIndex, parentIndex);
        }

        for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            this->AddSceneNode(*childNode, sceneNodeIndex);
        }
    }
}
