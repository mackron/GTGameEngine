// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIStyleServer
#define GT_GUIStyleServer

namespace GT
{
    class GUIServer;
    class GUIStyleAttributeHandler;

    /**
    *   \brief  Class for managing the styles in a server.
    *
    *   The style server handles everything to do with styling.
    */
    class GUIStyleServer
    {
    public:
    
        /**
        *   \brief  Constructor.
        */
        GUIStyleServer(GUIServer &server);
        
        /**
        *   \brief  Destructor.
        */
        ~GUIStyleServer();
        
        /**
        *   \brief                   Loads a style script string.
        *   \param  script      [in] The string containing the style script. This is \b not a file name.
        *   \param  baseURLPath [in] The base path to use when translating URLs. If this is null, it will use the current directory.
        *   \param  identifier  [in] An identifier string to make it possible to unload a style script.
        *   \return                  True if the script was loaded successfully; false otherwise.
        *
        *   \remarks
        *       The function will still return true if there are warnings, but will return false if there was an error.
        *       \par
        *       It's important to note that if a style class is redefined, it does not replace the old one - it extends it.
        *       Take the following example: If the script below is executed first...
        *       \code
        *       SomeClass
        *       {
        *           width:100%
        *           padding:0px;
        *       }
        *       \endcode
        *       and then the next script was executed afterwards:
        *       SomeClass
        *       {
        *           height:100%
        *           padding:1px;
        *       }
        *       \endcode
        *       it would be equivalent to this:
        *       SomeClass
        *       {
        *           width:100%
        *           height:100%
        *           padding:1px;
        *       }
        *       \endcode
        *       Note how the padding attribute replaced with the newest one (the same rule applies to all attributes).
        *       \par
        *       The script will be associated with the given identifier. It can be unloaded with Unload() by passing the
        *       identifier string.
        */
        bool Load(const char* script, const char* baseURLPath = nullptr, const char* identifier = nullptr);

        /// Unloads a style script based on the given identifier.
        ///
        /// @param identifier         [in] The identifier whose script is being uploaded.
        /// @param firstOccuranceOnly [in] Whether or not only the first occurance should be removed.
        ///
        /// @remarks
        ///     Use 'firstOccuranceOnly' as an optimization when you know there will only be a single occurance of the identifier,
        ///     such as files.
        void Unload(const char* identifier, bool firstOccuranceOnly = false);


        /// Loads a style script from a file.
        ///
        /// @param filePath [in] The path of the file to load.
        ///
        /// @remarks
        ///     The script identifier will be set to the absolute path of the file.
        ///     @par
        ///     An important distinction between this and Load() is that this will unload the file before loading.
        bool LoadFromFile(const char* filePath);

        /// Unloads the given file.
        ///
        /// @param filePath [in] The path of the file to unload.
        ///
        /// @remarks
        ///     This doesn't actually parse the file. Instead, it simply unloads the scripts that have an identifier equal
        ///     to the absolute path.
        void UnloadFile(const char* filePath);

        
    
        
        /**
        *   \brief                     Adds a variable to the server.
        *   \param  name          [in] The name of the variable.
        *   \param  value         [in] The value of the variable.
        *   \param  nameSizeInTs  [in] The size in chars of the name.
        *   \param  valueSizeInTs [in] The size in chars of the value.
        */
        void AddVariable(const char* name, const char* value, ptrdiff_t nameSizeInTs = -1, ptrdiff_t valueSizeInTs = -1);
        
        /// Removes a variable from the server.
        ///
        /// @param name [in] The name of the variable to remove.
        void RemoveVariable(const char* name, ptrdiff_t nameSizeInTs = -1);

        
        /**
        *   \brief  Creates and adds a new style class.
        *
        *   \remarks
        *       Do NOT use this for modifier classes.
        */
        GUIStyleClass* CreateStyleClass(const char *name, ptrdiff_t nameSizeInBytes = -1);

        /**
        *   \brief                  Deletes a style class that was created internally by GUIStyleServer.
        *   \param  styleClass [in] A reference to the style class being deleted.
        */
        void DeleteStyleClass(GUIStyleClass &styleClass, bool refreshHostStacks = true);

        /// Deletes every style class created by the style server.
        void DeleteAllStyleClasses();


        /**
        *   \brief                    Creates and sets a modifier style class for the given class.
        *   \param  baseClass    [in] The base style class that is having it's modifier class created.
        *   \param  modifierType [in] The type of the modifier class being created (StyleClassType_Hovered, StyleClassType_Pushed, etc).
        *   \return                   A pointer to the new modifier style class.
        */
        GUIStyleClass* CreateModifierStyleClass(GUIStyleClass& baseClass, GUIStyleClassType modifierType);

        /**
        *   \brief  Deletes a modifier style class for the given class.
        */
        void DeleteModifierStyleClass(GUIStyleClass& baseClass, GUIStyleClassType modifierType);

        /**
        *   \brief  Deletes all modifier classes of the given base class.
        */
        void DeleteAllModifierStyleClasses(GUIStyleClass& baseClass);
        
        
        /**
        *   \brief                       Retrieves the value of a variable.
        *   \param  name            [in] A pointer to the start of the name of the variable.
        *   \param  nameSizeInBytes [in] The size in bytes of the name.
        *   \return                      A pointer to the null terminated string containing the variable value.
        *
        *   \remarks
        *       If the variable can not be found, null is returned.
        */
        const char* GetVariable(const char* name, ptrdiff_t nameSizeInBytes = -1);
        
        /**
        *   \brief                 Retrieves a pointer to the style class of the given name, or null if it doesn't exist.
        *   \param  className [in] The name of the class to retrieve.
        *   \return                A pointer to the style of the given name, or null if the style class doesn't exist.
        *
        *   \remarks
        *       Use GetDefaultStyleClass() to retrieve a reference to the default style class.
        */
        GUIStyleClass* GetStyleClass(const char* className, ptrdiff_t nameSizeInBytes = -1);
        
        
        /// Retrieves a pointer to the default style class.
        ///
        /// @return A pointer to the default style class.
        ///
        /// @remarks
        ///     The default style class is the * class.
        GUIStyleClass* GetDefaultStyleClass();
        
        /// Retrieves the style class of the root element.
        GUIStyleClass* GetRootElementStyleClass();


        /// Sets a style attribute for the given class.
        ///
        /// @param styleClass    [in] A reference to the style class whose attribute is being set.
        /// @param name          [in] The name of the attribute being set.
        /// @param nameSizeInTs  [in] The size of <name> in chars.
        /// @param value         [in] The new value to set as a string.
        /// @param valueSizeInTs [in] The size of <value> in chars.
        ///
        /// @return True if the value was set correctly; false otherwise.
        bool SetGUIStyleAttribute(GUIStyleClass &styleClass, const char* name, ptrdiff_t nameSizeInTs, const char* value, ptrdiff_t valueSizeInTs);

        /// Unsets a style attribute for the given class.
        ///
        /// @param styleClass   [in] A reference to the style class whose attribute is being unset.
        /// @param name         [in] The name of the attribute being unset.
        /// @param nameSizeInTs [in] The size of the name string, in chars. Set to -1 if the string is null-terminated.
        bool UnsetGUIStyleAttribute(GUIStyleClass &styleClass, const char* name, ptrdiff_t nameSizeInTs = -1);

        /// Retrieves the stringified value of an attribute.
        ///
        /// @param styleClass    [in] A reference to the style class whose attribute value is being retrieved.
        /// @param name          [in] The name of the attribute being set.
        /// @param nameSizeInTs  [in] The size of <name> in chars.
        ///
        /// @return The stringified value of the given attribute. Will return an empty string if anything goes wrong.
        GT::String GetGUIStyleAttribute(GUIStyleClass &styleClass, const char* name, ptrdiff_t nameSizeInTs);
        

        /// Clears the error stack.
        void ClearErrors();

        /// Retrieves the last error, removing it from the stack.
        ///
        /// @param errorOut [out] A reference to the object that will receive the error.
        ///
        /// @return True if an error was returned; false otherwise.
        bool GetLastError(GUIStyleScriptError &errorOut);

        /// Called when a compilation error is received.
        ///
        /// @param error [in] A reference to the structure containing the error information.
        void OnCompilerError(const GUIStyleScriptError &error);


        
    private:
    
        /**
        *   \brief  Loads defaults into the style server.
        */
        bool LoadDefaults();

        /**
        *   \brief                  Adds a style class to the server.
        *   \param  styleClass [in] A reference to the style class being added.
        */
        void AddStyleClass(GUIStyleClass &styleClass);

        /**
        *   \brief                  Removes a style class from the server.
        *   \param  styleClass [in] A reference to the style class being removed.
        *
        *   \remarks
        *       This does not delete the style class. Use DeleteStyleClass() to delete a style class that was created by GUIStyleServer.
        */
        void RemoveStyleClass(GUIStyleClass &styleClass);
        


        /// Loads the style attribute handlers. This will be called in the constructor.
        void LoadGUIStyleAttributeHandlers();

        /// Unloads all style attribute handlers. This will be called in the destructor. Handlers will be deleted with 'delete'.
        void UnloadGUIStyleAttributeHandlers();

        /// Adds a style attribute handler to the map of attribute handlers.
        ///
        /// @param attributeHandler [in] A reference to the attribute handler being added.
        void AddGUIStyleAttributeHandler(GUIStyleAttributeHandler &attributeHandler);


        /// Posts an error.
        void PostError(const GUIStyleScriptError &error);


        /// Merges a style script compiler class object with the given GUIStyleClass object.
        ///
        /// @param dest   [in] The destination style class.
        /// @param source [in] The source style class.
        ///
        /// @remarks
        ///     A merge will append the includes, replace existing attribute and add new attributes.
        void MergeStyleClass(GUIStyleClass &dest, const GUIStyleScriptCompilerClass &source);


        /// Unloads a compiler by it's index in the stack.
        ///
        /// @param compilerIndex [in] The index of the compiler to unload.
        void UnloadCompilerByIndex(size_t compilerIndex);

        /// Unloads a compiler variable.
        ///
        /// @param compilerVariable [in] A reference to the compiler variable.
        /// @param compilerIndex    [in] The index of the compiler in the stack.
        void UnloadCompilerVariable(const GUIStyleScriptCompilerVariable &compilerVariable, size_t compilerIndex);

        /// Determines whether or not the given variable is defined after the compiler at the given index.
        ///
        /// @param variableName  [in] The name of the variable.
        /// @param compilerIndex [in] The index of the compiler that owns the variable in question.
        bool HasMoreRecentlyDefinedCompilerVariable(const char* variableName, size_t compilerIndex) const;

        /// Finds the previously defined copmiler variable.
        ///
        /// @param className     [in] The name of the variable.
        /// @param compilerIndex [in] The index of the compiler that owns the variable in question.
        const GUIStyleScriptCompilerVariable* FindPreviouslyDefinedCompilerVariable(const char* variableName, size_t compilerIndex) const;


        /// Unloads a compiler class.
        ///
        /// @param compilerClass [in] A reference to the compiler class.
        /// @param modifierName  [in] The name of the modifier of the given class. This should be nullptr if it is the main base class.
        /// @param compilerIndex [in] The index of the compiler in the stack.
        void UnloadCompilerClass(const GUIStyleScriptCompilerClass &compilerClass, const char* modifierName, size_t compilerIndex);

        /// Determines whether or not the given class attribute is defined after the compiler at the given index.
        ///
        /// @param className     [in] The name of the class.
        /// @param modifierName  [in] The name of the class modifier, or nullptr if it's the base class.
        /// @param attributeName [in] The name of the attribute.
        /// @param compilerIndex [in] The index of the compiler that owns the class attribute in question.
        bool HasMoreRecentlyDefinedCompilerAttribute(const char* className, const char* modifierName, const char* attributeName, size_t compilerIndex) const;

        /// Finds the previously defined copmiler attribute.
        ///
        /// @param className     [in] The name of the class.
        /// @param modifierName  [in] The name of the class modifier, or nullptr if it's the base class.
        /// @param attributeName [in] The name of the attribute.
        /// @param compilerIndex [in] The index of the compiler that owns the class attribute in question.
        const GUIStyleScriptCompilerClassAttribute* FindPreviouslyDefinedCompilerAttribute(const char* className, const char* modifierName, const char* attributeName, size_t compilerIndex) const;


        
    private:

        /// A reference to the main parent server.
        GUIServer &server;
    
        /// The list of variables, index by their name.
        GT::Dictionary<char*> variables;


        /// The list of style classes, index by their name.
        GUIStyleClassTree classes;
        
        /// A pointer to the default style class. This is the style class with the name "*".
        GUIStyleClass* defaultStyleClass;
        
        /// A pointer to the style class of the root element.
        GUIStyleClass* rootElementStyleClass;
        

        /// The map of style attribute handlers. Basically, we map an object to each style attribute ('width', 'background-color', etc). We then access
        /// those objects based on the attribute name. When we have the object, we call virtual methods which will perform the assignment or retrieval.
        /// This map is filled in the LoadGUIStyleAttributeHandlers() method, which will be called in the constructor.
        GT::Dictionary<GUIStyleAttributeHandler*> attributeHandlers;

        /// The stack of script errors.
        GT::Vector<GUIStyleScriptError> errorStack;

        /// The stack of compilers representing the order in which style scripts were loaded. We use this for unloading. When a style is loaded, a
        /// pointer to the compiler object will be placed at the end of this list. When a compiler is removed from the list, the server will look at
        /// this stack in determining how to modify the style classes and variables appopriately.
        GT::Vector<GUIStyleScriptCompiler*> compilerStack;
        

        
    public: // Testing methods. Delete these later.
    
        void PrintDetails();

    
    private:    // No copying.
        GUIStyleServer(const GUIStyleServer &);
        GUIStyleServer & operator=(const GUIStyleServer &);
    };
}

#endif
