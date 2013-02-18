// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/ScriptComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ScriptLibrary.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ScriptComponent, "Script");

    ScriptComponent::ScriptComponent(SceneNode &node)
        : Component(node), scripts()
    {
    }

    ScriptComponent::~ScriptComponent()
    {
    }


    ScriptDefinition* ScriptComponent::AddScript(const char* relativePath)
    {
        auto iScript = this->scripts.Find(relativePath);
        if (iScript != nullptr)
        {
            return iScript->value;
        }


        // If we get here it means the script does not already exist and needs to be added.
        auto definition = GTEngine::ScriptLibrary::Acquire(relativePath);
        if (definition != nullptr)
        {
            this->scripts.Add(relativePath, definition);
        }

        return definition;
    }

    void ScriptComponent::RemoveScript(const char* relativePath)
    {
        auto iScript = this->scripts.Find(relativePath);
        if (iScript != nullptr)
        {
            GTEngine::ScriptLibrary::Unacquire(iScript->value);
            this->scripts.RemoveByIndex(iScript->index);
        }
    }


    size_t ScriptComponent::GetScriptCount() const
    {
        return this->scripts.count;
    }

    ScriptDefinition* ScriptComponent::GetScriptDefinitionByIndex(size_t index) const
    {
        return this->scripts.buffer[index]->value;
    }

    const char* ScriptComponent::GetScriptAbsolutePathByIndex(size_t index) const
    {
        return this->scripts.buffer[index]->value->GetAbsolutePath();
    }

    const char* ScriptComponent::GetScriptRelativePathByIndex(size_t index) const
    {
        return this->scripts.buffer[index]->value->GetRelativePath();
    }



    bool ScriptComponent::HasOnUpdate() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts.buffer[i]->value;
            assert(script != nullptr);
            {
                if (script->HasOnUpdate())
                {
                    return true;
                }
            }
        }

        return false;
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ScriptComponent::Serialize(GTCore::Serializer &serializer) const
    {
        (void)serializer;
    }

    void ScriptComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        (void)deserializer;
    }
}