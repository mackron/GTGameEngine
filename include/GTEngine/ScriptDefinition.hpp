// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptDefinition_hpp_
#define __GTEngine_ScriptDefinition_hpp_

#include <GTCore/String.hpp>

namespace GTEngine
{
    /// Class containing information about a script asset file.
    class ScriptDefinition
    {
    public:

        /// Constructor.
        ScriptDefinition(const char* absolutePath, const char* relativePath, const char* scriptString);

        /// Destructor.
        ~ScriptDefinition();


        /// Retrieves the absolute path.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path.
        const char* GetRelativePath() const;


        /// Retrieves the script string.
        const char* GetScriptString() const;



    private:

        /// The absolute path of the script.
        GTCore::String absolutePath;

        /// The relative path of the script.
        GTCore::String relativePath;

        /// The script string.
        GTCore::String scriptString;


    };
}

#endif