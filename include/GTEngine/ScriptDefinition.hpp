// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptDefinition_hpp_
#define __GTEngine_ScriptDefinition_hpp_

#include "ScriptVariable.hpp"
#include <GTLib/String.hpp>
#include <GTLib/Vector.hpp>

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

        /// Determines if the script has an OnShow event handler.
        bool HasOnShow() const;

        /// Determines if the script has an OnHide event handler.
        bool HasOnHide() const;

        /// Determines if the script has an OnObjectEnter event handler.
        bool HasOnObjectEnter() const;

        /// Determines if the script has an OnEnterObject event handler.
        bool HasOnEnterObject() const;

        /// Determines if the script has an OnObjectLeave event handler.
        bool HasOnObjectLeave() const;

        /// Determines if the script has an OnLeaveObject event handler.
        bool HasOnLeaveObject() const;

        /// Determines if the script has an OnMouseMove event handler.
        bool HasOnMouseMove() const;

        /// Determines if the script has an OnMouseWheel event handler.
        bool HasOnMouseWheel() const;

        /// Determines if the script has an OnMouseButtonDown event handler.
        bool HasOnMouseButtonDown() const;

        /// Determines if the script has an OnMouseButtonUp event handler.
        bool HasOnMouseButtonUp() const;

        /// Determines if the script has an OnMouseButtonDoubleClick event handler.
        bool HasOnMouseButtonDoubleClick() const;

        /// Determines if the script has an OnKeyPressed event handler.
        bool HasOnKeyPressed() const;

        /// Determines if the script has an OnKeyReleased event handler.
        bool HasOnKeyReleased() const;



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
        GTLib::String absolutePath;

        /// The relative path of the script.
        GTLib::String relativePath;

        /// The script string.
        GTLib::String scriptString;

        
        /// Keeps track of whether or not this definition has an OnUpdate event handler.
        bool hasOnUpdate;

        /// Keeps track of whether or not this definition has an OnStartup event handler.
        bool hasOnStartup;

        /// Keeps track of whether or not this definition has an OnShutdown event handler.
        bool hasOnShutdown;

        /// Keeps track of whether or not this definition has an OnShow event handler.
        bool hasOnShow;

        /// Keeps track of whether or not this definition has an OnHide event handler.
        bool hasOnHide;

        /// Keeps track of whether or not this definition has an OnObjectEnter event handler.
        bool hasOnObjectEnter;

        /// Keeps track of whether or not this definition has an OnEnterObject event handler.
        bool hasOnEnterObject;

        /// Keeps track of whether or not this definition has an OnObjectLeave event handler.
        bool hasOnObjectLeave;

        /// Keeps track of whether or not this definition has an OnLeaveObject event handler.
        bool hasOnLeaveObject;

        /// Keeps track of whether of not this definition has an OnMouseMove event handler.
        bool hasOnMouseMove;

        /// Keeps track of whether of not this definition has an OnMouseWheel event handler.
        bool hasOnMouseWheel;

        /// Keeps track of whether of not this definition has an OnMouseButtonDown event handler.
        bool hasOnMouseButtonDown;

        /// Keeps track of whether of not this definition has an OnMouseButtonUp event handler.
        bool hasOnMouseButtonUp;

        /// Keeps track of whether of not this definition has an OnMouseButtonDoubleClick event handler.
        bool hasOnMouseButtonDoubleClick;

        /// Keeps track of whether of not this definition has an OnKeyPressed event handler.
        bool hasOnKeyPressed;

        /// Keeps track of whether of not this definition has an OnKeyReleased event handler.
        bool hasOnKeyReleased;


        /// The list of public variables, in the order they are defined (hopefully).
        GTLib::Vector<ScriptVariable*> publicVariables;
    };
}

#endif