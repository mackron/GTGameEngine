// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptDefinition.hpp>
#include <GTCore/Script.hpp>
#include <GTCore/Strings/LineIterator.hpp>
#include <GTCore/Strings/Tokenizer.hpp>

namespace GTEngine
{
    ScriptDefinition::ScriptDefinition(const char* absolutePathIn, const char* relativePathIn, const char* scriptStringIn)
        : absolutePath(absolutePathIn), relativePath(relativePathIn), scriptString(scriptStringIn),
          hasOnUpdate(false), hasOnStartup(false), hasOnShutdown(false), hasOnShow(false), hasOnHide(false)
    {
        // We need to do a parse over the script and retrieve it's public variables.
        this->ParseForPublicVariables();


        // We now need to determine if various events are defined in the script. To do this, we'll need to parse it. For now what we'll do
        // it just load it up in a temporary script and then see if we can find functions with the applicable names.
        //
        // We're also going to use this for retrieving the default value of variables.
        GTCore::Script tempScript;
        tempScript.Execute("self = {}");
        tempScript.Execute(scriptStringIn);

        tempScript.GetGlobal("self");
        assert(tempScript.IsTable(-1));
        {
            for (tempScript.PushNil(); tempScript.Next(-2); tempScript.Pop(1))
            {
                auto name = tempScript.ToString(-2);

                if (tempScript.IsFunction(-1))
                {
                    if (GTCore::Strings::Equal(name, "OnUpdate"))
                    {
                        this->hasOnUpdate = true;
                    }
                    else if (GTCore::Strings::Equal(name, "OnStartup"))
                    {
                        this->hasOnStartup = true;
                    }
                    else if (GTCore::Strings::Equal(name, "OnShutdown"))
                    {
                        this->hasOnShutdown = true;
                    }
                    else if (GTCore::Strings::Equal(name, "OnShow"))
                    {
                        this->hasOnShow = true;
                    }
                    else if (GTCore::Strings::Equal(name, "OnHide"))
                    {
                        this->hasOnHide = true;
                    }
                }
                else
                {
                    // If it's a public variable, we want to set the variable of that variable to that defined in the script.
                    auto variable = this->GetPublicVariableByName(name);
                    if (variable != nullptr)
                    {
                        switch (variable->GetType())
                        {
                        case ScriptVariableType_Number:
                            {
                                if (tempScript.IsNumber(-1))
                                {
                                    static_cast<ScriptVariable_Number*>(variable)->SetValue(tempScript.ToDouble(-1));
                                }

                                break;
                            }

                        case ScriptVariableType_Vec2:
                            {
                                auto variableVec2 = static_cast<ScriptVariable_Vec2*>(variable);

                                if (tempScript.IsNumber(-1))
                                {
                                    double value = tempScript.ToDouble(-2);
                                    variableVec2->SetValue(value, value);
                                }
                                else if (tempScript.IsTable(-1))
                                {
                                    for (tempScript.PushNil(); tempScript.Next(-2); tempScript.Pop(1))
                                    {
                                        if (tempScript.IsNumber(-1))
                                        {
                                            double value = tempScript.ToDouble(-1);

                                            if (tempScript.IsNumber(-2))
                                            {
                                                auto key = tempScript.ToInteger(-2);

                                                if (key == 1)
                                                {
                                                    variableVec2->SetX(value);
                                                }
                                                else if (key == 2)
                                                {
                                                    variableVec2->SetY(value);
                                                }
                                            }
                                            else if (tempScript.IsString(-2))
                                            {
                                                auto key = tempScript.ToString(-2);

                                                if (GTCore::Strings::Equal<false>(key, "x"))
                                                {
                                                    variableVec2->SetX(value);
                                                }
                                                else if (GTCore::Strings::Equal<false>(key, "y"))
                                                {
                                                    variableVec2->SetY(value);
                                                }
                                            }
                                        }
                                    }
                                }

                                break;
                            }

                        case ScriptVariableType_Vec3:
                            {
                                auto variableVec3 = static_cast<ScriptVariable_Vec3*>(variable);

                                if (tempScript.IsNumber(-1))
                                {
                                    double value = tempScript.ToDouble(-2);
                                    variableVec3->SetValue(value, value, value);
                                }
                                else if (tempScript.IsTable(-1))
                                {
                                    for (tempScript.PushNil(); tempScript.Next(-2); tempScript.Pop(1))
                                    {
                                        if (tempScript.IsNumber(-1))
                                        {
                                            double value = tempScript.ToDouble(-1);

                                            if (tempScript.IsNumber(-2))
                                            {
                                                auto key = tempScript.ToInteger(-2);

                                                if (key == 1)
                                                {
                                                    variableVec3->SetX(value);
                                                }
                                                else if (key == 2)
                                                {
                                                    variableVec3->SetY(value);
                                                }
                                                else if (key == 3)
                                                {
                                                    variableVec3->SetZ(value);
                                                }
                                            }
                                            else if (tempScript.IsString(-2))
                                            {
                                                auto key = tempScript.ToString(-2);

                                                if (GTCore::Strings::Equal<false>(key, "x"))
                                                {
                                                    variableVec3->SetX(value);
                                                }
                                                else if (GTCore::Strings::Equal<false>(key, "y"))
                                                {
                                                    variableVec3->SetY(value);
                                                }
                                                else if (GTCore::Strings::Equal<false>(key, "z"))
                                                {
                                                    variableVec3->SetZ(value);
                                                }
                                            }
                                        }
                                    }
                                }

                                break;
                            }

                        case ScriptVariableType_Vec4:
                            {
                                auto variableVec4 = static_cast<ScriptVariable_Vec4*>(variable);

                                if (tempScript.IsNumber(-1))
                                {
                                    double value = tempScript.ToDouble(-2);
                                    variableVec4->SetValue(value, value, value, value);
                                }
                                else if (tempScript.IsTable(-1))
                                {
                                    for (tempScript.PushNil(); tempScript.Next(-2); tempScript.Pop(1))
                                    {
                                        if (tempScript.IsNumber(-1))
                                        {
                                            double value = tempScript.ToDouble(-1);

                                            if (tempScript.IsNumber(-2))
                                            {
                                                auto key = tempScript.ToInteger(-2);

                                                if (key == 1)
                                                {
                                                    variableVec4->SetX(value);
                                                }
                                                else if (key == 2)
                                                {
                                                    variableVec4->SetY(value);
                                                }
                                                else if (key == 3)
                                                {
                                                    variableVec4->SetZ(value);
                                                }
                                                else if (key == 4)
                                                {
                                                    variableVec4->SetW(value);
                                                }
                                            }
                                            else if (tempScript.IsString(-2))
                                            {
                                                auto key = tempScript.ToString(-2);

                                                if (GTCore::Strings::Equal<false>(key, "x"))
                                                {
                                                    variableVec4->SetX(value);
                                                }
                                                else if (GTCore::Strings::Equal<false>(key, "y"))
                                                {
                                                    variableVec4->SetY(value);
                                                }
                                                else if (GTCore::Strings::Equal<false>(key, "z"))
                                                {
                                                    variableVec4->SetZ(value);
                                                }
                                                else if (GTCore::Strings::Equal<false>(key, "w"))
                                                {
                                                    variableVec4->SetW(value);
                                                }
                                            }
                                        }
                                    }
                                }

                                break;
                            }

                        case ScriptVariableType_Boolean:
                            {
                                if (tempScript.IsBoolean(-1))
                                {
                                    static_cast<ScriptVariable_Boolean*>(variable)->SetValue(tempScript.ToBoolean(-1));
                                }
                                else if (tempScript.IsNumber(-1))
                                {
                                    static_cast<ScriptVariable_Boolean*>(variable)->SetValue(tempScript.ToInteger(-1) != 0);
                                }

                                break;
                            }

                        case ScriptVariableType_String:
                            {
                                if (tempScript.IsString(-1))
                                {
                                    static_cast<ScriptVariable_String*>(variable)->SetValue(tempScript.ToString(-1));
                                }

                                break;
                            }

                        case ScriptVariableType_Prefab:
                            {
                                if (tempScript.IsString(-1))
                                {
                                    static_cast<ScriptVariable_Prefab*>(variable)->SetValue(tempScript.ToString(-1));
                                }

                                break;
                            }

                        default: break;
                        }
                    }
                }
            }
        }
        tempScript.Pop(1);
    }

    ScriptDefinition::~ScriptDefinition()
    {
        this->ClearPublicVariables();
    }


    const char* ScriptDefinition::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }

    const char* ScriptDefinition::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }

    const char* ScriptDefinition::GetScriptString() const
    {
        return this->scriptString.c_str();
    }



    bool ScriptDefinition::HasOnUpdate() const
    {
        return this->hasOnUpdate;
    }

    bool ScriptDefinition::HasOnStartup() const
    {
        return this->hasOnStartup;
    }

    bool ScriptDefinition::HasOnShutdown() const
    {
        return this->hasOnShutdown;
    }

    bool ScriptDefinition::HasOnShow() const
    {
        return this->hasOnShow;
    }

    bool ScriptDefinition::HasOnHide() const
    {
        return this->hasOnHide;
    }



    size_t ScriptDefinition::GetPublicVariableCount() const
    {
        return this->publicVariables.count;
    }

    ScriptVariable* ScriptDefinition::GetPublicVariableByIndex(size_t index)
    {
        return this->publicVariables[index];
    }

    ScriptVariable* ScriptDefinition::GetPublicVariableByName(const char* variableName) const
    {
        for (size_t i = 0; i < this->publicVariables.count; ++i)
        {
            auto variable = this->publicVariables[i];
            assert(variable != nullptr);
            {
                if (GTCore::Strings::Equal(variable->GetName(), variableName))
                {
                    return variable;
                }
            }
        }

        return nullptr;
    }



    //////////////////////////////////////////////
    // Private

    void ScriptDefinition::ClearPublicVariables()
    {
        for (size_t i = 0; i < this->publicVariables.count; ++i)
        {
            delete this->publicVariables[i];
        }
        this->publicVariables.Clear();
    }

    void ScriptDefinition::ParseForPublicVariables()
    {
        this->ClearPublicVariables();

        auto markerString           = "--[[public:";
        auto markerStringLengthInTs = GTCore::Strings::SizeInTs(markerString);

        auto selfString             = "self.";
        auto selfStringLengthInTs   = GTCore::Strings::SizeInTs(selfString);


        // We just iterate over line by line and check for a "--[[public:" string.
        GTCore::Strings::LineIterator line(this->scriptString.c_str());
        while (line)
        {
            auto lineStart      = GTCore::Strings::TrimStart(line.start, line.end - line.start);
            auto lineEnd        = line.end;
            auto lineLengthInTs = lineEnd - lineStart;

            if (lineLengthInTs > markerStringLengthInTs)
            {
                if (GTCore::Strings::Equal(markerString, lineStart, markerStringLengthInTs))
                {
                    auto typeStart = lineStart + markerStringLengthInTs;
                    auto typeEnd   = GTCore::Strings::FindFirst(typeStart, lineEnd - typeStart, "]]", 2);
                    auto markerEnd = typeEnd;
                    
                    if (markerEnd != nullptr && lineEnd > markerEnd + 2)
                    {
                        typeEnd = GTCore::Strings::TrimEnd(typeStart, typeEnd - typeStart);

                        // We use a simple tokenizer to grab the name. It should be the first token, and should begin with "self.". We start this after the
                        // metadata marker ("--[[public:<type>]]"). The +2 simply moves the string past the "]]"
                        GTCore::Strings::Tokenizer token(markerEnd + 2);
                        if (token)
                        {
                            if ((token.end - token.start) > selfStringLengthInTs)
                            {
                                if (GTCore::Strings::Equal(selfString, token.start, selfStringLengthInTs))
                                {
                                    // We'll have a name. We can now construct the variable.
                                    auto nameStart = token.start + selfStringLengthInTs;
                                    auto nameEnd   = token.end;
                                    GTCore::String variableName(nameStart, nameEnd - nameStart);
                                    
                                    if (GTCore::Strings::Equal("number", typeStart, typeEnd - typeStart))
                                    {
                                        this->publicVariables.PushBack(new ScriptVariable_Number(variableName.c_str()));
                                    }
                                    else if (GTCore::Strings::Equal("vec2", typeStart, typeEnd - typeStart))
                                    {
                                        this->publicVariables.PushBack(new ScriptVariable_Vec2(variableName.c_str()));
                                    }
                                    else if (GTCore::Strings::Equal("vec3", typeStart, typeEnd - typeStart))
                                    {
                                        this->publicVariables.PushBack(new ScriptVariable_Vec3(variableName.c_str()));
                                    }
                                    else if (GTCore::Strings::Equal("vec4", typeStart, typeEnd - typeStart))
                                    {
                                        this->publicVariables.PushBack(new ScriptVariable_Vec4(variableName.c_str()));
                                    }
                                    else if (GTCore::Strings::Equal("bool", typeStart, typeEnd - typeStart))
                                    {
                                        this->publicVariables.PushBack(new ScriptVariable_Boolean(variableName.c_str()));
                                    }
                                    else if (GTCore::Strings::Equal("string", typeStart, typeEnd - typeStart))
                                    {
                                        this->publicVariables.PushBack(new ScriptVariable_String(variableName.c_str()));
                                    }
                                    else if (GTCore::Strings::Equal("prefab", typeStart, typeEnd - typeStart))
                                    {
                                        this->publicVariables.PushBack(new ScriptVariable_Prefab(variableName.c_str()));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            ++line;
        }
    }
}