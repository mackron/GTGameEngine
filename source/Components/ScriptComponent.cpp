// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/ScriptComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ScriptComponent, "Script");

    ScriptComponent::ScriptComponent(SceneNode &node)
        : Component(node), scripts()
    {
    }

    ScriptComponent::~ScriptComponent()
    {
        this->Clear();
    }


    ScriptDefinition* ScriptComponent::AddScript(const char* relativePath)
    {
        auto definition = this->GetScriptDefinitionByRelativePath(relativePath);
        if (definition == nullptr)
        {
            // If we get here it means the script does not already exist and needs to be added. Note that we want to add these even
            // if we get a null pointer. We do this so the names remain persistant for things like editting tools.
            auto definition = GTEngine::ScriptLibrary::Acquire(relativePath, nullptr, true);
            this->scripts.PushBack(definition);

            this->OnChanged();
        }

        return definition;
    }

    void ScriptComponent::RemoveScript(const char* relativePath)
    {
        size_t index;
        auto definition = this->GetScriptDefinitionByRelativePath(relativePath, index);
        if (definition != nullptr)
        {
            this->RemoveScriptByIndex(index);
        }
    }

    void ScriptComponent::RemoveScriptByIndex(size_t index)
    {
        auto definition = this->scripts[index];
        if (definition != nullptr)
        {
            ScriptLibrary::Unacquire(definition);
        }

        this->scripts.Remove(index);

        this->OnChanged();
    }


    size_t ScriptComponent::GetScriptCount() const
    {
        return this->scripts.count;
    }

    ScriptDefinition* ScriptComponent::GetScriptDefinitionByIndex(size_t index) const
    {
        return this->scripts[index];
    }

    ScriptDefinition* ScriptComponent::GetScriptDefinitionByAbsolutePath(const char* absolutePath, size_t &indexOut)
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto definition = this->scripts[i];
            if (definition != nullptr)
            {
                if (GTCore::Strings::Equal(definition->GetAbsolutePath(), absolutePath))
                {
                    indexOut = i;
                    return definition;
                }
            }
        }

        return nullptr;
    }

    ScriptDefinition* ScriptComponent::GetScriptDefinitionByRelativePath(const char* relativePath, size_t &indexOut)
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto definition = this->scripts[i];
            if (definition != nullptr)
            {
                if (GTCore::Strings::Equal(definition->GetRelativePath(), relativePath))
                {
                    indexOut = i;
                    return definition;
                }
            }
        }

        return nullptr;
    }


    const char* ScriptComponent::GetScriptAbsolutePathByIndex(size_t index) const
    {
        if (this->scripts[index] != nullptr)
        {
            return this->scripts[index]->GetAbsolutePath();
        }

        return nullptr;
    }

    const char* ScriptComponent::GetScriptRelativePathByIndex(size_t index) const
    {
        if (this->scripts[index] != nullptr)
        {
            return this->scripts[index]->GetRelativePath();
        }

        return nullptr;
    }



    bool ScriptComponent::HasOnUpdate() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            if (script != nullptr)
            {
                if (script->HasOnUpdate())
                {
                    return true;
                }
            }
        }

        return false;
    }


    void ScriptComponent::Clear()
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            ScriptLibrary::Unacquire(this->scripts[i]);
        }
        this->scripts.Clear();
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ScriptComponent::Serialize(GTCore::Serializer &serializer) const
    {
        // We will use an intermediary serializer like normal. All we need to save is the relative paths of the scripts we're using.
        GTCore::BasicSerializer intermediarySerializer;
        intermediarySerializer.Write(static_cast<uint32_t>(this->scripts.count));
        
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            if (script != nullptr)
            {
                intermediarySerializer.Write(script->GetRelativePath());
            }
            else
            {
                intermediarySerializer.WriteString("");
            }
        }



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ScriptComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ScriptComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        // We want to clear everything before adding deserializing.
        this->Clear();

        Serialization::ChunkHeader header;
        deserializer.Read(header);
        assert(header.id == Serialization::ChunkID_ScriptComponent_Main);
        {
            switch (header.version)
            {
            case 1:
                {
                    uint32_t scriptCount;
                    deserializer.Read(scriptCount);

                    for (uint32_t i = 0; i < scriptCount; ++i)
                    {
                        GTCore::String relativePath;
                        deserializer.Read(relativePath);

                        this->AddScript(relativePath.c_str());
                    }

                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing ScriptComponent. Main chunk has an unsupported version (%d).", header.version);

                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }
    }
}