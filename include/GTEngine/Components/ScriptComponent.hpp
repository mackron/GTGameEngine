// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptComponent_hpp_
#define __GTEngine_ScriptComponent_hpp_

#include "../Component.hpp"
#include "../ScriptDefinition.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(ScriptComponent)
    public:

        /// Constructor.
        ScriptComponent(SceneNode &node);

        /// Destructor.
        ~ScriptComponent();



        /// Adds a script by it's file name.
        ///
        /// @param relativePath [in] The relative path of the file to add.
        ///
        /// @remarks
        ///     If the script is already loaded, this will do nothing and return the existing definition.
        ScriptDefinition* AddScript(const char* relativePath);

        /// Removes a script by it's file name.
        ///
        /// @param relativePath [in] The relative path of the file to remove.
        void RemoveScript(const char* relativePath);

        /// Removes a script by it's index.
        ///
        /// @param index [in] The index of the script to remove.
        void RemoveScriptByIndex(size_t index);

        /// Reloads a script at the given index.
        ///
        /// @param index           [in] The index of the item being reloaded.
        /// @param newRelativePath [in] The relative path of the new script.
        ScriptDefinition* ReloadScript(size_t index, const char* newRelativePath);



        /// Retrieves the number of scripts used by this component.
        size_t GetScriptCount() const;

        /// Retrieves the definition of the script at the given index.
        ///
        /// @param index [in] The index of the script definition to retrieve.
        ScriptDefinition* GetScriptDefinitionByIndex(size_t index) const;


        /// Retrieves the definition of the script by the absolute path.
        ScriptDefinition* GetScriptDefinitionByAbsolutePath(const char* absolutePath, size_t &indexOut);
        ScriptDefinition* GetScriptDefinitionByAbsolutePath(const char* absolutePath)
        {
            size_t devnull;
            return this->GetScriptDefinitionByAbsolutePath(absolutePath, devnull);
        }

        /// Retrieves the definition of the script by the relative path.
        ScriptDefinition* GetScriptDefinitionByRelativePath(const char* relativePath, size_t &indexOut);
        ScriptDefinition* GetScriptDefinitionByRelativePath(const char* relativePath)
        {
            size_t devnull;
            return this->GetScriptDefinitionByRelativePath(relativePath, devnull);
        }

        

        /// Retrieves the absolute path of the script at the given index.
        const char* GetScriptAbsolutePathByIndex(size_t index) const;

        /// Retrieves the relative path of the script at the given index.
        const char* GetScriptRelativePathByIndex(size_t index) const;



        /// Determines if any of the attached scripts has an OnUpdate event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs an OnUpdate event posted to the scene node.
        bool HasOnUpdate() const;

        /// Determines if any of the attached scripts has an OnStartup event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnStartup() const;

        /// Determines if any of the attached scripts has an OnStartup event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnShutdown() const;


        /// Clears the component of all scripts.
        void Clear();


        /// Sets the internal flag keeping track of whether of not the OnStartup event has been called.
        void MarkOnStartupAsCalled();

        /// Unsets the internal flag keeping track of whether or not the OnStartup event has been called.
        void UnmarkOnStartupAsCalled();

        /// Determines whether or not the internal flag keeping track of whether or not the OnStartup event has been called is set.
        bool HasOnStartupBeenCalled() const;



        /// Retrieves a pointer to a public variable by it's name.
        ///
        /// @param name [in] The name of the variable to retrieve.
        ///
        /// @remarks
        ///     This will return null if the variable does not exist.
        ///     @par
        ///     This runs in O(n) time.
        ScriptVariable* GetPublicVariableByName(const char* name, size_t &indexOut) const;
        ScriptVariable* GetPublicVariableByName(const char* name) const
        {
            size_t devnull;
            return this->GetPublicVariableByName(name, devnull);
        }


        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);



    private:

        /// Merges the variables from the given script definition into our own.
        void MergePublicVariables(ScriptDefinition &definition);

        /// Removes the public variables from the given script definition.
        void RemovePublicVariables(ScriptDefinition &definition);

        /// Removes a public variable by it's name.
        void RemovePublicVariableByName(const char* name);

        /// Removes a public variable by it's index.
        void RemovePublicVariableByIndex(size_t index);

        /// Determines if the given variable exists in a definition other than the one specified.
        bool DoesPublicVariableExistInOtherDefinition(const char* name, ScriptDefinition &definitionToExclude);


    private:

        /// The list of scripts attached to the component.
        GTCore::Vector<ScriptDefinition*> scripts;

        /// We keep a list of every public variable here. We keep these separate from the definitions because we want to allow these to have
        /// their own values.
        GTCore::Vector<ScriptVariable*> publicVariables;

        /// This keeps track of whether or not the OnStartup event has been called. When OnStartup is called, this will be set to true. When
        /// OnShutdown has been called, it will revert back to false. We use this for keeping track of whether or not OnStartup and OnShutdown
        /// events should be called in certain situations. We only ever want one OnShutdown for every OnStartup and vice versa.
        bool hasOnStartupBeenCalled;


    GTENGINE_DECL_COMPONENT_END()
}

#endif