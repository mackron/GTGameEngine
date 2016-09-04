// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ScriptDefinition
#define GT_ScriptDefinition

namespace GT
{
    /// Class containing information about a script asset file.
    class ScriptDefinition
    {
    public:

        /// Constructor.
        ScriptDefinition(const char* absolutePath, const char* scriptString);

        /// Destructor.
        ~ScriptDefinition();


        /// Retrieves the absolute path.
        const char* GetAbsolutePath() const;

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

        /// Determines if the script has an OnSceneNodeEnter event handler.
        bool HasOnSceneNodeEnter() const;

        /// Determines if the script has an OnEnterSceneNode event handler.
        bool HasOnEnterSceneNode() const;

        /// Determines if the script has an OnSceneNodeLeave event handler.
        bool HasOnSceneNodeLeave() const;

        /// Determines if the script has an OnLeaveSceneNode event handler.
        bool HasOnLeaveSceneNode() const;

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

        /// Determines if the script has an OnSerialize event handler.
        bool HasOnSerialize() const;

        /// Determines if the script has an OnDeserialize event handler.
        bool HasOnDeserialize() const;

        /// Determines if the script has an OnSerializeGlobalData event handler.
        bool HasOnSerializeGlobalData() const;

        /// Determines if the script has an OnDeserializeGlobalData event handler.
        bool HasOnDeserializeGlobalData() const;

        /// Determines if the script has an OnGamePause event handler.
        bool HasOnGamePause() const;

        /// Determines if the script has an OnGameResume event handler.
        bool HasOnGameResume() const;



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
        String absolutePath;

        /// The script string.
        String scriptString;

        
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

        /// Keeps track of whether or not this definition has an OnSceneNodeEnter event handler.
        bool hasOnSceneNodeEnter;

        /// Keeps track of whether or not this definition has an OnEnterSceneNode event handler.
        bool hasOnEnterSceneNode;

        /// Keeps track of whether or not this definition has an OnSceneNodeLeave event handler.
        bool hasOnSceneNodeLeave;

        /// Keeps track of whether or not this definition has an OnLeaveSceneNode event handler.
        bool hasOnLeaveSceneNode;

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

        /// Keeps track of whether or not this definition has an OnSerialize event handler.
        bool hasOnSerialize;

        /// Keeps track of whether or not this definition has an OnDeserialize event handler.
        bool hasOnDeserialize;

        /// Keeps track of whether or not this definition has an OnSerializeGlobalData event handler.
        bool hasOnSerializeGlobalData;

        /// Keeps track of whether or not this definition has an OnDeserializeGlobalData event handler.
        bool hasOnDeserializeGlobalData;

        /// Keeps track of whether or not this definition has an OnGamePause event handler.
        bool hasOnGamePause;

        /// Keeps track of whether or not this definition has an OnGameResume event handler.
        bool hasOnGameResume;


        /// The list of public variables, in the order they are defined (hopefully).
        Vector<ScriptVariable*> publicVariables;
    };
}

#endif