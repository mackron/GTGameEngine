// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_hpp_
#define __GTEngine_Scripting_hpp_

#include "Scripting/Scripting_Game.hpp"
#include "Scripting/Scripting_Math.hpp"
#include "Scripting/Scripting_Rendering.hpp"
#include "Scripting/Scripting_Audio.hpp"
#include "Scripting/Scripting_Scene.hpp"
#include "Scripting/Scripting_SceneNode.hpp"
#include "Scripting/Scripting_Components.hpp"
#include "Scripting/Scripting_Particles.hpp"
#include "Scripting/Scripting_Editor.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the GTEngine script library into the given script.
        ///
        /// @param script [in] The script to load the GTEngine script library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This only loads the content in the GTEngine namespace/table. This will NOT load any game-specific functionality.
        bool LoadGTEngineScriptLibrary(GTCore::Script &script);



        /// Post a global MouseMove event to everything relevant.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param mousePosX [in] The x mouse position.
        /// @param mousePosY [in] The y mouse position.
        void PostEvent_OnMouseMove(GTCore::Script &script, int mousePosX, int mousePosY);

        /// Post a global MouseWheel event to everything relevant.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param mousePosX [in] The x mouse position.
        /// @param mousePosY [in] The y mouse position.
        /// @param delta     [in] The scroll delta.
        void PostEvent_OnMouseWheel(GTCore::Script &script, int mousePosX, int mousePosY, int delta);

        /// Post a global MouseButtonDown event to everything relevant.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param mousePosX [in] The x mouse position.
        /// @param mousePosY [in] The y mouse position.
        /// @param button    [in] The button code.
        void PostEvent_OnMouseButtonDown(GTCore::Script &script, int mousePosX, int mousePosY, GTCore::MouseButton button);

        /// Post a global MouseButtonUp event to everything relevant.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param mousePosX [in] The x mouse position.
        /// @param mousePosY [in] The y mouse position.
        /// @param button    [in] The button code.
        void PostEvent_OnMouseButtonUp(GTCore::Script &script, int mousePosX, int mousePosY, GTCore::MouseButton button);

        /// Post a global MouseButtonDoubleClick event to everything relevant.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param mousePosX [in] The x mouse position.
        /// @param mousePosY [in] The y mouse position.
        /// @param button    [in] The button code.
        void PostEvent_OnMouseButtonDoubleClick(GTCore::Script &script, int mousePosX, int mousePosY, GTCore::MouseButton button);

        /// Post a global KeyPressed event to everything relevant.
        ///
        /// @param script [in] A reference to the main script object.
        /// @param key    [in] The key code.
        void PostEvent_OnKeyPressed(GTCore::Script &script, GTCore::Key key);

        /// Post a global KeyReleased event to everything relevant.
        ///
        /// @param script [in] A reference to the main script object.
        /// @param key    [in] The key code.
        void PostEvent_OnKeyReleased(GTCore::Script &script, GTCore::Key key);



        ///////////////////////////////////////////////////////////////
        // Script Definitions

        /// Loads a scene node script, replacing the old one if it exists.
        ///
        /// @param script             [in] A reference to the script to load the scene node script into.
        /// @param scriptRelativePath [in] The relative path of the script.
        /// @param scriptString       [in] The actual script content.
        bool LoadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath, const char* scriptString);

        /// Unloads a scene node script.
        ///
        /// @param script             [in] A reference to the script to unload the scene node script from.
        /// @param scriptRelativePath [in] The relative path of the script to unload.
        void UnloadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath);

        /// Synchronizes the script definitions with the script library.
        ///
        /// @remarks
        ///     This will unload any no-longer-loaded definitions, load newly loaded ones and refresh existing ones.
        ///     @par
        ///     This does not update any scene nodes connected to the applicable definitions.
        void SyncScriptDefinitionsWithLibrary(GTCore::Script &script);



        namespace FFI
        {
            /// Retrieves the executable directory.
            int GetExecutableDirectory(GTCore::Script &script);

            /// Determines if the given file path is a model file, based on it's extension.
            int IsModelFile(GTCore::Script &script);

            /// Determines if the given file path is an image file, based on it's extension.
            int IsImageFile(GTCore::Script &script);

            /// Determines if the given file path is a sound file, based on it's extension.
            int IsSoundFile(GTCore::Script &script);

            /// Determines if the given file path is a scene file, based on it's extension.
            int IsSceneFile(GTCore::Script &script);

            /// Determines if the given file path is a prefab file, based on it's extension.
            int IsPrefabFile(GTCore::Script &script);

            /// Determines if the given file path is a script file, based on it's extension.
            int IsScriptFile(GTCore::Script &script);


            /// Creates a prefab file from a scene node.
            ///
            /// @remarks
            ///     Argument 1: The absolute path of the file to create or replace.
            ///     Argument 2: The path to make the absolute path relative to.
            ///     Argument 3: A pointer to the scene node to create the prefrab file from.
            int CreatePrefab(GTCore::Script &script);
        }
    }
}

#endif