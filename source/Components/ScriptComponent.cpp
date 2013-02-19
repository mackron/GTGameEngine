// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/ScriptComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ScriptComponent, "Script");

    ScriptComponent::ScriptComponent(SceneNode &node)
        : Component(node), scripts(), hasOnStartupBeenCalled(false)
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

            // We need to merge the variables from the new definition into our own. The definition is allowed to be null here, so
            // that'll also need to be checked.
            if (definition != nullptr)
            {
                this->MergePublicVariables(*definition);
            }

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
            // We need to remove the variables defined in the definition being removed.
            this->RemovePublicVariables(*definition);

            ScriptLibrary::Unacquire(definition);
        }


        this->scripts.Remove(index);

        this->OnChanged();
    }

    ScriptDefinition* ScriptComponent::ReloadScript(size_t index, const char* newRelativePath)
    {
        if (this->scripts[index] != nullptr)
        {
            if (!GTCore::Strings::Equal(this->scripts[index]->GetRelativePath(), newRelativePath))
            {
                // Public variables from this definition need to be removed.
                this->RemovePublicVariables(*this->scripts[index]);

                ScriptLibrary::Unacquire(this->scripts[index]);
            }
            else
            {
                // It's the same file. Don't want to do anything.
                return this->scripts[index];
            }
        }

        this->scripts[index] = ScriptLibrary::Acquire(newRelativePath, nullptr, true);

        // Variables from the new definition need to be merged.
        if (this->scripts[index] != nullptr)
        {
            this->MergePublicVariables(*this->scripts[index]);
        }

        this->OnChanged();

        return this->scripts[index];
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

    bool ScriptComponent::HasOnStartup() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            if (script != nullptr)
            {
                if (script->HasOnStartup())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnShutdown() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            if (script != nullptr)
            {
                if (script->HasOnShutdown())
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


        for (size_t i = 0; i < this->publicVariables.count; ++i)
        {
            delete this->publicVariables[i];
        }
        this->publicVariables.Clear();
    }


    void ScriptComponent::MarkOnStartupAsCalled()
    {
        this->hasOnStartupBeenCalled = true;
    }

    void ScriptComponent::UnmarkOnStartupAsCalled()
    {
        this->hasOnStartupBeenCalled = false;
    }

    bool ScriptComponent::HasOnStartupBeenCalled() const
    {
        return this->hasOnStartupBeenCalled;
    }


    size_t ScriptComponent::GetPublicVariableCount() const
    {
        return this->publicVariables.count;
    }

    ScriptVariable* ScriptComponent::GetPublicVariableByIndex(size_t index) const
    {
        return this->publicVariables[index];
    }

    ScriptVariable* ScriptComponent::GetPublicVariableByName(const char* name, size_t &indexOut) const
    {
        for (size_t i = 0; i < this->publicVariables.count; ++i)
        {
            auto variable = this->publicVariables[i];
            assert(variable != nullptr);
            {
                if (GTCore::Strings::Equal(variable->GetName(), name))
                {
                    indexOut = i;
                    return variable;
                }
            }
        }

        return nullptr;
    }


    void ScriptComponent::SetPublicVariableValue(const char* variableName, double x)
    {
        auto variable = this->GetPublicVariableByName(variableName);
        if (variable != nullptr)
        {
            if (variable->GetType() == ScriptVariableType_Number)
            {
                static_cast<ScriptVariable_Number*>(variable)->SetValue(x);
                this->OnChanged();
            }
            else
            {
                // TODO: Implement these conditions!

                // Also valid for vector types in which case we'll set every component to this value.
                if (variable->GetType() == ScriptVariableType_Vec2)
                {
                    static_cast<ScriptVariable_Vec2*>(variable)->SetValue(x, x);
                    this->OnChanged();
                }
                else if (variable->GetType() == ScriptVariableType_Vec3)
                {
                    static_cast<ScriptVariable_Vec3*>(variable)->SetValue(x, x, x);
                    this->OnChanged();
                }
                else if (variable->GetType() == ScriptVariableType_Vec4)
                {
                    static_cast<ScriptVariable_Vec4*>(variable)->SetValue(x, x, x, x);
                    this->OnChanged();
                }
            }
        }
    }

    void ScriptComponent::SetPublicVariableValue(const char* variableName, double x, double y)
    {
        auto variable = this->GetPublicVariableByName(variableName);
        if (variable != nullptr)
        {
            if (variable->GetType() == ScriptVariableType_Vec2)
            {
                static_cast<ScriptVariable_Vec2*>(variable)->SetValue(x, y);
                this->OnChanged();
            }
        }
    }

    void ScriptComponent::SetPublicVariableValue(const char* variableName, double x, double y, double z)
    {
        auto variable = this->GetPublicVariableByName(variableName);
        if (variable != nullptr)
        {
            if (variable->GetType() == ScriptVariableType_Vec3)
            {
                static_cast<ScriptVariable_Vec3*>(variable)->SetValue(x, y, z);
                this->OnChanged();
            }
        }
    }

    void ScriptComponent::SetPublicVariableValue(const char* variableName, double x, double y, double z, double w)
    {
        auto variable = this->GetPublicVariableByName(variableName);
        if (variable != nullptr)
        {
            if (variable->GetType() == ScriptVariableType_Vec4)
            {
                static_cast<ScriptVariable_Vec4*>(variable)->SetValue(x, y, z, w);
                this->OnChanged();
            }
        }
    }

    void ScriptComponent::SetPublicVariableValue(const char* variableName, bool value)
    {
        auto variable = this->GetPublicVariableByName(variableName);
        if (variable != nullptr)
        {
            if (variable->GetType() == ScriptVariableType_Boolean)
            {
                static_cast<ScriptVariable_Boolean*>(variable)->SetValue(value);
                this->OnChanged();
            }
        }
    }

    void ScriptComponent::SetPublicVariableValue(const char* variableName, const char* value)
    {
        auto variable = this->GetPublicVariableByName(variableName);
        if (variable != nullptr)
        {
            if (variable->GetType() == ScriptVariableType_String)
            {
                static_cast<ScriptVariable_String*>(variable)->SetValue(value);
                this->OnChanged();
            }
            else if (variable->GetType() == ScriptVariableType_Prefab)
            {
                static_cast<ScriptVariable_Prefab*>(variable)->SetValue(value);
                this->OnChanged();
            }
        }
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ScriptComponent::Serialize(GTCore::Serializer &serializer) const
    {
        // We will use an intermediary serializer like normal. All we need to save is the relative paths of the scripts we're using.
        GTCore::BasicSerializer intermediarySerializer;

        
        // Script paths.
        intermediarySerializer.Write(static_cast<uint32_t>(this->scripts.count));
        
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            if (script != nullptr)
            {
                intermediarySerializer.WriteString(script->GetRelativePath());
            }
            else
            {
                intermediarySerializer.WriteString("");
            }
        }


        // Public variables.
        intermediarySerializer.Write(static_cast<uint32_t>(this->publicVariables.count));

        for (size_t i = 0; i < this->publicVariables.count; ++i)
        {
            auto variable = this->publicVariables[i];
            assert(variable != nullptr);
            {
                intermediarySerializer.Write(static_cast<uint32_t>(variable->GetType()));
                intermediarySerializer.WriteString(variable->GetName());

                // Data is saved different for the different types.
                switch (variable->GetType())
                {
                case ScriptVariableType_Number:
                    {
                        intermediarySerializer.Write(static_cast<ScriptVariable_Number*>(variable)->GetValue());
                        break;
                    }

                case ScriptVariableType_Vec2:
                    {
                        auto variableVec2 = static_cast<ScriptVariable_Vec2*>(variable);
                        intermediarySerializer.Write(variableVec2->GetX());
                        intermediarySerializer.Write(variableVec2->GetY());
                        
                        break;
                    }

                case ScriptVariableType_Vec3:
                    {
                        auto variableVec3 = static_cast<ScriptVariable_Vec3*>(variable);
                        intermediarySerializer.Write(variableVec3->GetX());
                        intermediarySerializer.Write(variableVec3->GetY());
                        intermediarySerializer.Write(variableVec3->GetZ());

                        break;
                    }

                case ScriptVariableType_Vec4:
                    {
                        auto variableVec4 = static_cast<ScriptVariable_Vec4*>(variable);
                        intermediarySerializer.Write(variableVec4->GetX());
                        intermediarySerializer.Write(variableVec4->GetY());
                        intermediarySerializer.Write(variableVec4->GetZ());
                        intermediarySerializer.Write(variableVec4->GetW());

                        break;
                    }

                case ScriptVariableType_Boolean:
                    {
                        intermediarySerializer.Write(static_cast<ScriptVariable_Boolean*>(variable)->GetValue());
                        break;
                    }

                case ScriptVariableType_String:
                    {
                        intermediarySerializer.WriteString(static_cast<ScriptVariable_String*>(variable)->GetValue());
                        break;
                    }

                case ScriptVariableType_Prefab:
                    {
                        intermediarySerializer.WriteString(static_cast<ScriptVariable_Prefab*>(variable)->GetValue());
                        break;
                    }


                default: break;
                }
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
                        deserializer.ReadString(relativePath);

                        this->AddScript(relativePath.c_str());
                    }


                    uint32_t variableCount;
                    deserializer.Read(variableCount);

                    for (uint32_t i = 0; i < variableCount; ++i)
                    {
                        uint32_t type;
                        deserializer.Read(type);

                        GTCore::String name;
                        deserializer.ReadString(name);


                        switch (type)
                        {
                        case ScriptVariableType_Number:
                            {
                                double value;
                                deserializer.Read(value);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Number)
                                {
                                    static_cast<ScriptVariable_Number*>(variable)->SetValue(value);
                                }

                                break;
                            }

                        case ScriptVariableType_Vec2:
                            {
                                double x;
                                double y;
                                deserializer.Read(x);
                                deserializer.Read(y);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Vec2)
                                {
                                    static_cast<ScriptVariable_Vec2*>(variable)->SetValue(x, y);
                                }


                                break;
                            }

                        case ScriptVariableType_Vec3:
                            {
                                double x;
                                double y;
                                double z;
                                deserializer.Read(x);
                                deserializer.Read(y);
                                deserializer.Read(z);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Vec3)
                                {
                                    static_cast<ScriptVariable_Vec3*>(variable)->SetValue(x, y, z);
                                }


                                break;
                            }

                        case ScriptVariableType_Vec4:
                            {
                                double x;
                                double y;
                                double z;
                                double w;
                                deserializer.Read(x);
                                deserializer.Read(y);
                                deserializer.Read(z);
                                deserializer.Read(w);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Vec4)
                                {
                                    static_cast<ScriptVariable_Vec4*>(variable)->SetValue(x, y, z, w);
                                }


                                break;
                            }

                        case ScriptVariableType_Boolean:
                            {
                                bool value;
                                deserializer.Read(value);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Boolean)
                                {
                                    static_cast<ScriptVariable_Boolean*>(variable)->SetValue(value);
                                }


                                break;
                            }

                        case ScriptVariableType_String:
                            {
                                GTCore::String value;
                                deserializer.ReadString(value);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_String)
                                {
                                    static_cast<ScriptVariable_String*>(variable)->SetValue(value.c_str());
                                }


                                break;
                            }

                        case ScriptVariableType_Prefab:
                            {
                                GTCore::String value;
                                deserializer.ReadString(value);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Prefab)
                                {
                                    static_cast<ScriptVariable_Prefab*>(variable)->SetValue(value.c_str());
                                }


                                break;
                            }

                        default: break;
                        }
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



    ///////////////////////////////////////////////
    // Private

    void ScriptComponent::MergePublicVariables(ScriptDefinition &definition)
    {
        // If a variable of the same name already exists, we leave it be. Otherwise, we just copy it.
        size_t variableCount = definition.GetPublicVariableCount();
        for (size_t i = 0; i < variableCount; ++i)
        {
            auto variable = definition.GetPublicVariableByIndex(i);
            assert(variable != nullptr);
            {
                if (this->GetPublicVariableByName(variable->GetName()) == nullptr)
                {
                    this->publicVariables.PushBack(ScriptVariable::CreateCopy(*variable));
                }
            }
        }
    }

    void ScriptComponent::RemovePublicVariables(ScriptDefinition &definition)
    {
        // When removing these variables, we want to make sure it's not actually used in another definition. If so, we leave it be.
        size_t variableCount = definition.GetPublicVariableCount();
        for (size_t i = 0; i < variableCount; ++i)
        {
            auto variable = definition.GetPublicVariableByIndex(i);
            assert(variable != nullptr);
            {
                if (!this->DoesPublicVariableExistInOtherDefinition(variable->GetName(), definition))
                {
                    this->RemovePublicVariableByName(variable->GetName());
                }
            }
        }
    }

    void ScriptComponent::RemovePublicVariableByName(const char* name)
    {
        size_t index;
        if (this->GetPublicVariableByName(name, index) != nullptr)
        {
            this->RemovePublicVariableByIndex(index);
        }
    }

    void ScriptComponent::RemovePublicVariableByIndex(size_t index)
    {
        delete this->publicVariables[index];
        this->publicVariables.Remove(index);
    }

    bool ScriptComponent::DoesPublicVariableExistInOtherDefinition(const char* name, ScriptDefinition &definitionToExclude)
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script != &definitionToExclude)
                {
                    if (script->GetPublicVariableByName(name) != nullptr)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }
}