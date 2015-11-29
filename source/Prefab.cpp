// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Prefab.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GT
{
    Prefab::Prefab(const char* absolutePathIn, const char* relativePathIn)
        : absolutePath(absolutePathIn), relativePath(relativePathIn), serializers(), hierarchy(), nextID(0)
    {
    }

    Prefab::~Prefab()
    {
        this->Clear();
    }


    const char* Prefab::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }

    const char* Prefab::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }


    void Prefab::SetFromSceneNode(const SceneNode &sceneNode)
    {
        // First, we clear everything.
        this->Clear();

        // We need to grab every scene node in the hierarchy, serialize them, and add them to the internal containers. Of course, this needs to be done recursively.
        this->AddSceneNode(sceneNode, 1, 0);        // We pass 1 here for the ID because we want the root scene node to always have that ID.
    }


    BasicSerializer* Prefab::GetSerializerByID(uint64_t id)
    {
        auto iSerializer = this->serializers.Find(id);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }

        return nullptr;
    }

    const BasicSerializer* Prefab::GetSerializerByID(uint64_t id) const
    {
        auto iSerializer = this->serializers.Find(id);
        if (iSerializer != nullptr)
        {
            return iSerializer->value;
        }

        return nullptr;
    }


    uint64_t Prefab::GetRootID() const
    {
        return 1;
    }


    void Prefab::Clear()
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

    void Prefab::AddSceneNode(const SceneNode &sceneNode, uint64_t id, uint64_t parentID)
    {
        id = this->AddSingleSceneNode(sceneNode, id, parentID);

        for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
        {
            this->AddSceneNode(*childNode, 0, id);
        }
    }

    uint64_t Prefab::AddSingleSceneNode(const SceneNode &sceneNode, uint64_t id, uint64_t parentID)
    {
        auto serializer = new BasicSerializer;
        sceneNode.Serialize(*serializer, SceneNode::NoID | SceneNode::ForceDeselected);

        if (id == 0)
        {
            if (this->serializers.count == 0)
            {
                id = 1;
            }
            else
            {
                id = ++this->nextID;
            }
        }

        if (this->nextID <= id)
        {
            this->nextID = id;
        }


        this->serializers.Add(id, serializer);


        if (parentID != 0)
        {
            this->hierarchy.Add(id, parentID);
        }


        return id;
    }


    void Prefab::GetChildIDs(uint64_t parentID, Vector<uint64_t> &childIDs) const
    {
        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            if (this->hierarchy.buffer[i]->value == parentID)
            {
                childIDs.PushBack(this->hierarchy.buffer[i]->key);
            }
        }
    }



    ////////////////////////////////////////////
    // Serialization.

    bool Prefab::Serialize(Serializer &serializer)
    {
        // We'll need to use an intermediary serializer so we can get an accurate size.
        BasicSerializer intermediarySerializer;
        
        // Serializers first.
        intermediarySerializer.Write(static_cast<uint32_t>(this->serializers.count));

        for (size_t i = 0; i < this->serializers.count; ++i)
        {
            uint64_t sceneNodeID         = this->serializers.buffer[i]->key;
            auto     sceneNodeSerializer = this->serializers.buffer[i]->value;

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
        header.id          = Serialization::ChunkID_Prefab;
        header.version     = 1;
        header.sizeInBytes = static_cast<uint32_t>(intermediarySerializer.GetBufferSizeInBytes());

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);


        return true;
    }

    bool Prefab::Deserialize(Deserializer &deserializer)
    {
        // We want to clear first.
        this->Clear();

        Serialization::ChunkHeader header;
        if (deserializer.Read(header) > 0)
        {
            assert(header.id == Serialization::ChunkID_Prefab);
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

                            auto serializer = new BasicSerializer;
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
                        Log("Error deserializing SceneNode. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        return false;
                    }
                }
            }
        }


        return true;
    }


    bool Prefab::WriteToFile()
    {
#if 0
        auto file = IO::Open(this->absolutePath.c_str(), IO::OpenMode::Write);
        if (file != nullptr)
        {
            FileSerializer serializer(file);
            this->Serialize(serializer);


            IO::Close(file);
            return true;
        }
#endif

        easyvfs_file* pFile = easyvfs_open(g_EngineContext->GetVFS(), this->absolutePath.c_str(), EASYVFS_READ, 0);
        if (pFile != nullptr)
        {
            FileSerializer serializer(pFile);
            this->Serialize(serializer);

            easyvfs_close(pFile);
            return true;
        }
        else
        {
            Log("Can not open file: '%s'.", this->absolutePath.c_str());
        }

        return false;
    }
}
