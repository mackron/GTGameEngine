// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Scene_hpp_
#define __GTEngine_Scripting_Scene_hpp_

#include "../GameScript.hpp"
#include "../Scene.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the scene library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadSceneLibrary(GTCore::Script &script);


        /// Registers a scene to the given script.
        ///
        /// @param script [in] A reference to the script object to register the scene with.
        /// @param scene  [in] A reference to the scene to register.
        bool RegisterScene(GTCore::Script &script, Scene &scene);

        /// Unregisters the given scene from the given script.
        ///
        /// @param script [in] A reference to the script object to unregister the scene from.
        /// @param scene  [in] A reference to the scene to unregister.
        void UnregisterScene(GTCore::Script &script, Scene &scene);


        namespace SceneFFI
        {
            /// Adds a scene node to the given scene.
            ///
            /// @remarks
            ///     Argument 1: The internal pointer to the scene that is having the scene node added to it.
            ///     Argument 2: The internal pointer to the scene node being added to the scene.
            int AddSceneNode(GTCore::Script &script);

            /// Removes a scene node from the given scene.
            ///
            /// @remarks
            ///     Argument 1: The internal pointer to the scene that is having the scene node removed from it.
            ///     Argument 2: The internal pointer to the scene node being removed from the scene.
            int RemoveSceneNode(GTCore::Script &script);

            /// Creates a new empty scene node by the given scene.
            ///
            /// @remarks
            ///     Argument 1: The internal pointer to the scene that is having the scene node added to it.
            ///     Argument 2: Optional. If present, can be a string which will assume a prefab is being spawned.
            ///     Return:     The internal pointer to the new scene node.
            int CreateNewSceneNode(GTCore::Script &script);


            /// Retrieves a table containing pointers to every scene node in the scene. The key is the scene node ID and value is the C++ pointer.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene.
            ///     Return:     An ID/Pointer map of every scene node in the scene.
            int GetSceneNodePtrs(GTCore::Script &script);


            /// Retrieves the C++ pointer of the scene node with the given ID.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene.
            ///     Argument 2: The scene node ID.
            int GetSceneNodePtrByID(GTCore::Script &script);


            /// Determines if the scene is paused.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene.
            int IsPaused(GTCore::Script &script);


            /// Sets the camera of the attached viewport at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene.
            ///     Argument 2: A pointer to the scene node.
            ///     Argument 3: The index of the viewport whose camera is being set.
            ///
            ///     If the index is invalid, nothing will be modified.
            int SetViewportCamera(GTCore::Script &script);

            /// Sets the aspect ratio of the camera attached to the given viewport to viewports width divided by height.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene.
            ///     Argument 2: The index of the viewport.
            int ApplyViewportCameraAspectRatio(GTCore::Script &script);
        }
    }
}

#endif