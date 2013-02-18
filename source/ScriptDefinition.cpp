// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptDefinition.hpp>
#include <GTCore/Script.hpp>

namespace GTEngine
{
    ScriptDefinition::ScriptDefinition(const char* absolutePathIn, const char* relativePathIn, const char* scriptStringIn)
        : absolutePath(absolutePathIn), relativePath(relativePathIn), scriptString(scriptStringIn),
          hasOnUpdate(false)
    {
        // We need to do a parse over the script and retrieve it's public variables.



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
                }
            }
        }
        tempScript.Pop(1);
    }

    ScriptDefinition::~ScriptDefinition()
    {
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
}