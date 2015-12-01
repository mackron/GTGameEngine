// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Components/PrefabComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GT
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(PrefabComponent, "Prefab")

    PrefabComponent::PrefabComponent(SceneNode &node)
        : Component(node),
          prefabAbsolutePath(),
          prefabRelativePath(), localHierarchyID(0)
    {
    }

    PrefabComponent::~PrefabComponent()
    {
    }


    const char* PrefabComponent::GetPrefabAbsolutePath() const
    {
        return this->prefabAbsolutePath.c_str();
    }


    const char* PrefabComponent::GetPrefabRelativePath() const
    {
        return this->prefabRelativePath.c_str();
    }

    void PrefabComponent::SetPrefabRelativePath(const char* relativePath)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        if (!easyvfs_find_absolute_path(g_EngineContext->GetVFS(), relativePath, absolutePath, sizeof(absolutePath))) {
            g_EngineContext->LogWarning("Could not find absolute path of prefab.");
        }

        this->prefabAbsolutePath = absolutePath;


        this->prefabRelativePath = relativePath;
        this->OnChanged();
    }


    uint64_t PrefabComponent::GetLocalHierarchyID() const
    {
        return this->localHierarchyID;
    }

    void PrefabComponent::SetLocalHierarchyID(uint64_t id)
    {
        this->localHierarchyID = id;
        this->OnChanged();
    }
    
    
    SceneNode & PrefabComponent::GetRootSceneNode()
    {
        SceneNode* root = &this->node;
        while (root->GetComponent<PrefabComponent>()->GetLocalHierarchyID() != 1)
        {
            root = this->node.GetParent();
        }
        
        
        assert(root != nullptr);
        return *root;
    }


    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void PrefabComponent::Serialize(Serializer &serializer) const
    {
        BasicSerializer intermediarySerializer;

        intermediarySerializer.WriteString(this->prefabRelativePath);
        intermediarySerializer.Write(this->localHierarchyID);



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_PrefabComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void PrefabComponent::Deserialize(Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        if (header.id == Serialization::ChunkID_PrefabComponent_Main)
        {
            if (header.version == 1)
            {
                String relativePath;
                uint64_t       id;

                deserializer.ReadString(relativePath);
                deserializer.Read(id);

                this->SetPrefabRelativePath(relativePath.c_str());
                this->SetLocalHierarchyID(id);
            }
            else
            {
                g_EngineContext->Logf("Error deserializing ScriptComponent. Main chunk has an unsupported version (%d).", header.version);
                deserializer.Seek(header.sizeInBytes);
            }
        }
    }
}
