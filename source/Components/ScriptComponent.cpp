// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Components/ScriptComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GT
{
    static const bool DontPostOnChange = true;

    GTENGINE_IMPL_COMPONENT_ATTRIBS(ScriptComponent, "Script")

    ScriptComponent::ScriptComponent(SceneNode &node)
        : Component(node), scripts(), publicVariables(), hasOnStartupBeenCalled(false)
    {
    }

    ScriptComponent::~ScriptComponent()
    {
        this->Clear();
    }


    ScriptDefinition* ScriptComponent::AddScript(const char* relativePath, bool dontPostOnChange)
    {
        auto definition = this->GetScriptDefinitionByRelativePath(relativePath);
        if (definition == nullptr)
        {
            // If we get here it means the script does not already exist and needs to be added. Note that we want to add these even
            // if we get a null pointer. We do this so the names remain persistant for things like editting tools.
            definition = ScriptLibrary::Acquire(relativePath, nullptr, true);
            if (definition != nullptr)
            {
                this->scripts.PushBack(definition);

                // We need to merge the variables from the new definition into our own. The definition is allowed to be null here, so
                // that'll also need to be checked.
                if (definition != nullptr)
                {
                    this->MergePublicVariables(*definition);
                }

                if (!dontPostOnChange)
                {
                    this->OnChanged();
                }
            }
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

    void ScriptComponent::ReloadScript(size_t index)
    {
        assert(this->scripts[index] != nullptr);
        {
            // All we need to do is merge the public variables and remove and unused ones.
            this->MergePublicVariables(*this->scripts[index]);
            this->RemoveUnusedPublicVariables();
        }
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
                if (Strings::Equal(definition->GetAbsolutePath(), absolutePath))
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
                if (Strings::Equal(definition->GetRelativePath(), relativePath))
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
        assert(this->scripts[index] != nullptr);
        {
            return this->scripts[index]->GetAbsolutePath();
        }
    }

    const char* ScriptComponent::GetScriptRelativePathByIndex(size_t index) const
    {
        assert(this->scripts[index] != nullptr);
        {
            return this->scripts[index]->GetRelativePath();
        }
    }



    bool ScriptComponent::HasOnUpdate() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
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

    bool ScriptComponent::HasOnStartup() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
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
            assert(script != nullptr);
            {
                if (script->HasOnShutdown())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnShow() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnShow())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnHide() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnHide())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnSceneNodeEnter() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnSceneNodeEnter())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnEnterSceneNode() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnEnterSceneNode())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnSceneNodeLeave() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnSceneNodeLeave())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnLeaveSceneNode() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnLeaveSceneNode())
                {
                    return true;
                }
            }
        }

        return false;
    }


    bool ScriptComponent::HasOnMouseMove() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnMouseMove())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnMouseWheel() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnMouseWheel())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnMouseButtonDown() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnMouseButtonDown())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnMouseButtonUp() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnMouseButtonUp())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnMouseButtonDoubleClick() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnMouseButtonDoubleClick())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnKeyPressed() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnKeyPressed())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnKeyReleased() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnKeyReleased())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnSerialize() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnSerialize())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnDeserialize() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnDeserialize())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnSerializeGlobalData() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnSerializeGlobalData())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnDeserializeGlobalData() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnDeserializeGlobalData())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnGamePause() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnGamePause())
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ScriptComponent::HasOnGameResume() const
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->HasOnGameResume())
                {
                    return true;
                }
            }
        }

        return false;
    }



    void ScriptComponent::Clear(bool clearPublicVariables)
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            ScriptLibrary::Unacquire(this->scripts[i]);
        }
        this->scripts.Clear();


        if (clearPublicVariables)
        {
            for (size_t i = 0; i < this->publicVariables.count; ++i)
            {
                delete this->publicVariables[i];
            }
            this->publicVariables.Clear();
        }
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
                if (Strings::Equal(variable->GetName(), name))
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

    void ScriptComponent::Serialize(Serializer &serializer, unsigned int flags) const
    {
        // We will use an intermediary serializer like normal. All we need to save is the relative paths of the scripts we're using.
        BasicSerializer intermediarySerializer;


        // Script paths.
        intermediarySerializer.Write(static_cast<uint32_t>(this->scripts.count));

        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                intermediarySerializer.WriteString(script->GetRelativePath());
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

                case ScriptVariableType_Unknown:
                default: break;
                }
            }
        }


        // OnSerialize
        //
        // This section needs to be skippable in case OnDeserialize() is not implemented properly. To do this we basically need to do this as a sub-chunk.
        if ((flags & SceneNode::NoScriptOnSerialize) == 0)
        {
            BasicSerializer onSerializeSerializer;

            // We want to use the scripting module to retrieve the serialized data because that is the part that's responsible for calling scene node events
            // on the scripting side.
            auto scene = this->GetNode().GetScene();
            if (scene != nullptr)
            {
                auto registeredScript = scene->GetRegisteredScript();
                if (registeredScript != nullptr)
                {
                    GT::DoOnSerialize(*registeredScript, const_cast<SceneNode &>(this->GetNode()), onSerializeSerializer);       // <-- Naughty const_cast here!
                }
            }


            Serialization::ChunkHeader header;
            header.id          = Serialization::ChunkID_ScriptComponent_OnSerialize;
            header.version     = 1;
            header.sizeInBytes = onSerializeSerializer.GetBufferSizeInBytes();

            intermediarySerializer.Write(header);
            intermediarySerializer.Write(onSerializeSerializer.GetBuffer(), header.sizeInBytes);
        }



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ScriptComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ScriptComponent::Deserialize(Deserializer &deserializer, bool noPublicVariableOverride, bool noOnDeserialize)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        assert(header.id == Serialization::ChunkID_ScriptComponent_Main);
        {
            switch (header.version)
            {
            case 1:
                {
                    // We want to clear everything before adding deserializing.
                    this->Clear(!noPublicVariableOverride);


                    uint32_t scriptCount;
                    deserializer.Read(scriptCount);

                    for (uint32_t i = 0; i < scriptCount; ++i)
                    {
                        String relativePath;
                        deserializer.ReadString(relativePath);

                        this->AddScript(relativePath.c_str(), DontPostOnChange);
                    }


                    uint32_t variableCount;
                    deserializer.Read(variableCount);

                    for (uint32_t i = 0; i < variableCount; ++i)
                    {
                        uint32_t type;
                        deserializer.Read(type);

                        String name;
                        deserializer.ReadString(name);


                        switch (type)
                        {
                        case ScriptVariableType_Number:
                            {
                                double value;
                                deserializer.Read(value);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Number && !noPublicVariableOverride)
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
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Vec2 && !noPublicVariableOverride)
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
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Vec3 && !noPublicVariableOverride)
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
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Vec4 && !noPublicVariableOverride)
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
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Boolean && !noPublicVariableOverride)
                                {
                                    static_cast<ScriptVariable_Boolean*>(variable)->SetValue(value);
                                }


                                break;
                            }

                        case ScriptVariableType_String:
                            {
                                String value;
                                deserializer.ReadString(value);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_String && !noPublicVariableOverride)
                                {
                                    static_cast<ScriptVariable_String*>(variable)->SetValue(value.c_str());
                                }


                                break;
                            }

                        case ScriptVariableType_Prefab:
                            {
                                String value;
                                deserializer.ReadString(value);

                                auto variable = this->GetPublicVariableByName(name.c_str());
                                if (variable != nullptr && variable->GetType() == ScriptVariableType_Prefab && !noPublicVariableOverride)
                                {
                                    static_cast<ScriptVariable_Prefab*>(variable)->SetValue(value.c_str());
                                }


                                break;
                            }

                        default: break;
                        }
                    }


                    ///////////////////////////////////////////////
                    // OnDeserialize

                    // There is a change the OnSerialize() data was not serialized. Thus, we need to peek first.
                    if (deserializer.Peek(&header, sizeof(header)) == sizeof(header) && header.id == Serialization::ChunkID_ScriptComponent_OnSerialize)
                    {
                        deserializer.Seek(sizeof(header));

                        if (!noOnDeserialize)
                        {
                            if (header.version == 1)
                            {
                                auto scene = this->GetNode().GetScene();
                                if (scene != nullptr)
                                {
                                    auto registeredScript = scene->GetRegisteredScript();
                                    if (registeredScript != nullptr)
                                    {
                                        // Before we actually do the OnDeserialize, we need to ensure the scene component has been registered with the scripting
                                        // environment. We can assert that the scene node has been instantiated in the scripting environment because all scene
                                        // nodes with a script component will be instantiated in the scripting environment straight after being added to the scene.
                                        GT::RegisterComponent(*registeredScript, this->GetNode(), this->GetName());

                                        // Now that the script component has been registered we can do the OnDeserialize().
                                        GT::DoOnDeserialize(*registeredScript, this->GetNode(), deserializer);
                                    }
                                    else
                                    {
                                        // The scene node is not part of a script. Just seek past the data.
                                        deserializer.Seek(header.sizeInBytes);
                                    }
                                }
                                else
                                {
                                    // The scene node is not part of a scene and thus not part of a script. Just seek past the data.
                                    deserializer.Seek(header.sizeInBytes);
                                }
                            }
                            else
                            {
                                // Unknown chunk version. Just seek past it.
                                deserializer.Seek(header.sizeInBytes);
                            }
                        }
                        else
                        {
                            // We don't want to do OnDeserialize(). Just seek past this chunk.
                            deserializer.Seek(header.sizeInBytes);
                        }
                    }


                    this->OnChanged();
                    break;
                }

            default:
                {
                    g_EngineContext->Logf("Error deserializing ScriptComponent. Main chunk has an unsupported version (%d).", header.version);

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

    void ScriptComponent::RemoveUnusedPublicVariables()
    {
        for (size_t i = 0; i < this->publicVariables.count; )
        {
            auto variable = this->publicVariables[i];
            assert(variable != nullptr);
            {
                if (!this->DoesPublicVariableExistInAnyDefinition(variable->GetName()))
                {
                    delete this->publicVariables[i];
                    this->publicVariables.Remove(i);
                }
                else
                {
                    ++i;
                }
            }
        }
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

    bool ScriptComponent::DoesPublicVariableExistInAnyDefinition(const char* name)
    {
        for (size_t i = 0; i < this->scripts.count; ++i)
        {
            auto script = this->scripts[i];
            assert(script != nullptr);
            {
                if (script->GetPublicVariableByName(name) != nullptr)
                {
                    return true;
                }
            }
        }

        return false;
    }
}
