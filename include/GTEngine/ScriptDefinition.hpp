// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptDefinition_hpp_
#define __GTEngine_ScriptDefinition_hpp_

#include "ScriptVariable.hpp"
#include <GTCore/String.hpp>
#include <GTCore/Vector.hpp>

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



        /// Determines if the script has an OnUpdate event handler.
        bool HasOnUpdate() const;

        /// Determines if the script has an OnStartup event handler.
        bool HasOnStartup() const;

        /// Determines if the script has an OnShutdown event handler.
        bool HasOnShutdown() const;


        /// Retrieves the number of public variables.
        size_t GetPublicVariableCount() const;

        /// Retrieves a public variable by it's index.
        ///
        /// @param index [in] The index of the public variable to retrieve.
        ScriptVariable* GetPublicVariableByIndex(size_t index);

        /// Retrieves a pointer to the public variable object by it's name.
        ///
        /// @remarks
        ///     This runs in O(n) time.
        ScriptVariable* GetPublicVariableByName(const char* variableName) const;



    private:

        /// Clears the public variables.
        void ClearPublicVariables();

        /// Parses the given script and fills the list of public variables with initial values.
        ///
        /// @remarks
        ///     This will clear the public variables.
        void ParseForPublicVariables();


    private:

        /// The absolute path of the script.
        GTCore::String absolutePath;

        /// The relative path of the script.
        GTCore::String relativePath;

        /// The script string.
        GTCore::String scriptString;

        
        /// Keeps track of whether or not this definition has an OnUpdate event handler.
        bool hasOnUpdate;

        /// Keeps track of whether or not this definition has an OnStartup event handler.
        bool hasOnStartup;

        /// Keeps track of whether or not this definition has an OnShutdown event handler.
        bool hasOnShutdown;


        /// The list of public variables, in the order they are defined (hopefully).
        GTCore::Vector<ScriptVariable*> publicVariables;
    };
}

#endif