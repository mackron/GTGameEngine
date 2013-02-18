// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptComponent_hpp_
#define __GTEngine_ScriptComponent_hpp_

#include "../Component.hpp"
#include "../ScriptDefinition.hpp"
#include <GTCore/Dictionary.hpp>

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


        /// Retrieves the number of scripts used by this component.
        size_t GetScriptCount() const;

        /// Retrieves the definition of the script at the given index.
        ///
        /// @param index [in] The index of the script definition to retrieve.
        ScriptDefinition* GetScriptDefinitionByIndex(size_t index) const;

        /// Retrieves the absolute path of the script at the given index.
        const char* GetScriptAbsolutePathByIndex(size_t index) const;

        /// Retrieves the relative path of the script at the given index.
        const char* GetScriptRelativePathByIndex(size_t index) const;



        /// Determines if any of the attached scripts has an OnUpdate event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs an OnUpdate event posted to the scene node.
        bool HasOnUpdate() const;


        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);



    private:

        /// The list of scripts attached to the component, keyed by the relative path
        GTCore::Dictionary<ScriptDefinition*> scripts;



    GTENGINE_DECL_COMPONENT_END()
}

#endif