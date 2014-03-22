// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/PrefabComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(PrefabComponent, "Prefab")

    PrefabComponent::PrefabComponent(SceneNode &node)
        : Component(node),
          prefabRelativePath(), localHierarchyID(0)
    {
    }

    PrefabComponent::~PrefabComponent()
    {
    }


    const char* PrefabComponent::GetPrefabRelativePath() const
    {
        return this->prefabRelativePath.c_str();
    }

    void PrefabComponent::SetPrefabRelativePath(const char* relativePath)
    {
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

    void PrefabComponent::Serialize(GTLib::Serializer &serializer) const
    {
        GTLib::BasicSerializer intermediarySerializer;

        intermediarySerializer.WriteString(this->prefabRelativePath);
        intermediarySerializer.Write(this->localHierarchyID);



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_PrefabComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void PrefabComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        if (header.id == Serialization::ChunkID_PrefabComponent_Main)
        {
            if (header.version == 1)
            {
                GTLib::String relativePath;
                uint64_t       id;

                deserializer.ReadString(relativePath);
                deserializer.Read(id);

                this->SetPrefabRelativePath(relativePath.c_str());
                this->SetLocalHierarchyID(id);
            }
            else
            {
                GTEngine::Log("Error deserializing ScriptComponent. Main chunk has an unsupported version (%d).", header.version);
                deserializer.Seek(header.sizeInBytes);
            }
        }
    }
}
