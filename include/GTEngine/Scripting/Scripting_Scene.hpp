// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Scripting_Scene_hpp_
#define __GTEngine_Scripting_Scene_hpp_

#include "../GameScript.hpp"
#include "../Scene.hpp"

namespace GT
{
    /// Loads the scene library.
    ///
    /// @param script [in] A reference to the script to load the library into.
    ///
    /// @return True if the library is loaded successfully; false otherwise.
    bool LoadSceneLibrary(GT::Script &script);


    /// Registers a scene to the given script.
    ///
    /// @param script [in] A reference to the script object to register the scene with.
    /// @param scene  [in] A reference to the scene to register.
    bool RegisterScene(GT::Script &script, Scene &scene);

    /// Unregisters the given scene from the given script.
    ///
    /// @param script [in] A reference to the script object to unregister the scene from.
    /// @param scene  [in] A reference to the scene to unregister.
    void UnregisterScene(GT::Script &script, Scene &scene);


    namespace SceneFFI
    {
        /// Adds a scene node to the given scene.
        ///
        /// @remarks
        ///     Argument 1: The internal pointer to the scene that is having the scene node added to it.
        ///     Argument 2: The internal pointer to the scene node being added to the scene.
        int AddSceneNode(GT::Script &script);

        /// Removes a scene node from the given scene.
        ///
        /// @remarks
        ///     Argument 1: The internal pointer to the scene that is having the scene node removed from it.
        ///     Argument 2: The internal pointer to the scene node being removed from the scene.
        int RemoveSceneNode(GT::Script &script);

        /// Creates a new empty scene node by the given scene.
        ///
        /// @remarks
        ///     Argument 1: The internal pointer to the scene that is having the scene node added to it.
        ///     Argument 2: Optional. If present, can be a string which will assume a prefab is being spawned.
        ///     Return:     The internal pointer to the new scene node.
        int CreateNewSceneNode(GT::Script &script);


        /// Retrieves a table containing pointers to every scene node in the scene. The key is the scene node ID and value is the C++ pointer.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Return:     An ID/Pointer map of every scene node in the scene.
        int GetSceneNodePtrs(GT::Script &script);


        /// Retrieves the C++ pointer of the scene node with the given ID.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The scene node ID.
        int GetSceneNodePtrByID(GT::Script &script);

        /// Retrieves the first occurance of the scene node with the given name.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The name of the scene node.
        int GetSceneNodePtrByName(GT::Script &script);

        /// Retrieves the ID of the first occurance of the scene node with the given name.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The name of the scene node.
        int GetSceneNodeIDByName(GT::Script &script);


        /// Determines if the scene is paused.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        int IsPaused(GT::Script &script);


        /// Sets the camera of the attached viewport at the given index.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: A pointer to the scene node.
        ///     Argument 3: The index of the viewport whose camera is being set.
        ///
        ///     If the index is invalid, nothing will be modified.
        int SetViewportCamera(GT::Script &script);

        /// Sets the aspect ratio of the camera attached to the given viewport to viewports width divided by height.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The index of the viewport.
        int ApplyViewportCameraAspectRatio(GT::Script &script);


        /// Determines whether or not script events are blocked.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        int IsScriptEventsBlocked(GT::Script &script);


        /// Sets the walkable height for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The new walkable height.
        int SetWalkableHeight(GT::Script &script);

        /// Sets the walkable radius for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The new walkable radius.
        int SetWalkableRadius(GT::Script &script);

        /// Sets the walkable slope angle for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The new walkable slope angle.
        int SetWalkableSlopeAngle(GT::Script &script);

        /// Sets the walkable climb height for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The new walkable climb height.
        int SetWalkableClimbHeight(GT::Script &script);

        /// Retrieves the walkable height for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        int GetWalkableHeight(GT::Script &script);

        /// Retrieves the walkable radius for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        int GetWalkableRadius(GT::Script &script);

        /// Retrieves the walkable slope angle for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        int GetWalkableSlopeAngle(GT::Script &script);

        /// Retrieves the walkable climb height for navigation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        int GetWalkableClimbHeight(GT::Script &script);

        /// Builds the navigation mesh at the given index.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        int BuildNavigationMesh(GT::Script &script);


        /// Calculates the ray to use for picking on the given viewport.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The x position of the mouse on the viewport.
        ///     Argument 3: The y position of the mouse on the viewport.
        ///     Argument 4: The index of the viewport whose picking ray is being retrieved.
        int CalculateViewportPickingRay(GT::Script &script);

        /// Performs a simple ray test on the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: The start position of the ray as a math.vec3.
        ///     Argument 3: The end position of the ray as a math.vec3.
        ///
        ///     This returns a pointer to the scene node that is closest to the start position of the ray.
        int RayTest(GT::Script &script);


        /// Sets the gravity of the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///     Argument 2: A math.vec3 containing the new gravity value.
        int SetGravity(GT::Script &script);

        /// Retrieves the gravity of the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene.
        ///
        ///     The return value is a math.vec3.
        int GetGravity(GT::Script &script);
    }
}

#endif