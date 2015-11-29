// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Scripting_Editor
#define GT_Scripting_Editor

#include "../GameScript.hpp"

namespace GT
{
    /// Loads the editor library.
    ///
    /// @param script [in] A reference to the script to load the library into.
    ///
    /// @return True if the library is loaded successfully; false otherwise.
    bool LoadEditorLibrary(GT::Script &script);


    namespace SubEditorFFI
    {
        /// Retrieves the absolute path of the file loaded in the given sub-editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the sub-editor.
        int GetAbsolutePath(GT::Script &script);

        /// Retrieves the relative path of the file loaded in the given sub-editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the sub-editor.
        int GetRelativePath(GT::Script &script);


        /// Marks the editor as modified.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the sub-editor to mark as modified.
        int MarkAsModified(GT::Script &script);

        /// Unmarks the editor as modified.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the sub-editor to unmark as modified.
        int UnmarkAsModified(GT::Script &script);
    }

    namespace ModelEditorFFI
    {
        /// Retrieves the names of every mesh making up the model of the given model editor.
        ///
        /// @remark
        ///     Argument 1: A pointer to the model editor.
        int GetMeshNames(GT::Script &script);


        /// Retrieves the relative names of all of the materials in the model of the given model editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int GetMaterials(GT::Script &script);

        /// Retrieves the relative path of the material at the given index.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the mesh whose material is being retrieved.
        int GetMaterialRelativePath(GT::Script &script);

        /// Retrieves a table containing the uniforms of a material.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the mesh whose material's uniforms are being retrieved.
        ///
        ///     The return value is a table. Each item in the table is another table with the properties 'type' and 'value'. For textures, the
        ///     value is a string containing the relative path of the texture.
        int GetMaterialUniformValues(GT::Script &script);

        /// Sets a material on the model loaded in the given model editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the material being set.
        ///     Argument 3: The relative path of the material to use.
        int SetMaterial(GT::Script &script);

        /// Sets the value of a uniform of the given meshes material.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the material/mesh whose uniform is getting set.
        ///     Argument 3: The name of the uniform getting set.
        ///     Argument 4: The value of the uniform.
        int SetMaterialUniform_Float(GT::Script &script);
        int SetMaterialUniform_Float2(GT::Script &script);
        int SetMaterialUniform_Float3(GT::Script &script);
        int SetMaterialUniform_Float4(GT::Script &script);
        int SetMaterialUniform_Texture2D(GT::Script &script);
            

        /// Retrieves the number of bones containing the in the model of the given model editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int GetBoneCount(GT::Script &script);

        /// Retrieves an array containing the names of every bone.
        ///
        /// @remarks
        ///     There is 1 table for every bone in the array. Each item contains two properties which should be self explanatory: 'name' and 'parentName'.
        ///
        ///     Argument 1: A pointer to the model editor.
        int GetBones(GT::Script &script);


        /// Plays the animation segment.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the animation segment to play.
        ///
        ///     The animation will be looped.
        int PlayAnimationSegmentByIndex(GT::Script &script);

        /// Plays the animation track of the model loaded in the given model editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int PlayAnimation(GT::Script &script);

        /// Resumes playback of the model's animation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int ResumeAnimation(GT::Script &script);

        /// Stops the animation track of the model loaded in the given model editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int StopAnimation(GT::Script &script);

        /// Pauses playback of the model's animation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int PauseAnimation(GT::Script &script);

        /// Determines whether or not the model is currently animating.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int IsAnimationPlaying(GT::Script &script);

        /// Determines whether or not the model is currently paused.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int IsAnimationPaused(GT::Script &script);


        /// Adds a new animation segment to the model.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The name of the new animation segment.
        ///     Argument 3: The start frame index.
        ///     Argument 4: The end frame index.
        int AddNewAnimationSegment(GT::Script &script);

        /// Removes an animation segment by it's index.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the animation segment to remove.
        int RemoveAnimationSegmentByIndex(GT::Script &script);

        /// Sets the name of the given animation segment.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the animation segment whose name is being set.
        ///     Argument 3: The new name.
        int SetAnimationSegmentName(GT::Script &script);

        /// Retrieves the name of the given animation segment.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the animation segment whose name is being retrieved.
        int GetAnimationSegmentName(GT::Script &script);

        /// Sets the start and end frame indices of the given animation segment.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the animation segment whose frame indices are being set.
        ///     Argument 3: The start index.
        ///     Argument 4: The end index.
        int SetAnimationSegmentFrameIndices(GT::Script &script);

        /// Retrieves the start and end frame indices of the given animation segment.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///     Argument 2: The index of the animation segment whose frame indices are being retrieved.
        ///     
        //      The return value is two integers.
        int GetAnimationSegmentFrameIndices(GT::Script &script);

        /// Retrieves a list of animation segments currently attached to the model.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///
        ///     Each item in the returned list is a table containing 'name', 'startKeyFrame' and 'endKeyFrame'.
        int GetAnimationSegments(GT::Script &script);


        /// Retrieves the settings used to build the convex hull of the model loaded in the given model editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int GetConvexHullBuildSettings(GT::Script &script);

        /// Shows the convex decomposition of the model.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int ShowConvexDecomposition(GT::Script &script);

        /// Hides the convex decomposition of the model.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int HideConvexDecomposition(GT::Script &script);

        /// Builds the convex decomposition of the model.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int BuildConvexDecomposition(GT::Script &script);


        /// Retrieves a pointer to the viewport camera scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        int GetViewportCameraSceneNodePtr(GT::Script &script);


        /// Retrieves the AABB of the model of the given model editor as two math.vec3s.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the model editor.
        ///
        ///     The return value is two math.vec3s. The first is the lower bounds and the second is the upper bounds.
        int GetModelAABB(GT::Script &script);
    }

    namespace MaterialEditorFFI
    {
        /// Retrieves a pointer to the viewport camera scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetViewportCameraSceneNodePtr(GT::Script &script);
    }

    namespace SceneEditorFFI
    {
        /// Retrieves the C++ pointer to the scene of the given scene editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetScenePtr(GT::Script &script);


        /// Sets the insertion position.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A math.vec3 containing the new insertion position.
        int SetInsertionPosition(GT::Script &script);

        /// Retrieves the insertion position.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Return: A math.vec3 containing the insertion position, in world space.
        int GetInsertionPosition(GT::Script &script);

        /// Updates the insertion position based on the current position of the mouse.
        ///
        /// @remarks
        ///     This will perform a ray cast againt the selection volumes of every scene node, starting from the position of the mouse in the viewport. If
        ///     nothing is hit, it will attempt a ray cast against a small section of the X/Z plane. If that is not hit, it will set the insertion position
        ///     to just in front of the camera.
        ///
        ///     Argument 1: A pointer to the scene editor.
        int UpdateInsertionPositionFromMouse(GT::Script &script);

        /// Updates the insertion position such that it is placed just in front of the camera.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int UpdateInsertionPositionToInFrontOfCamera(GT::Script &script);


        /// Deselects everything in the given scene editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DeselectAll(GT::Script &script);

        /// Selects the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor containing the scene node.
        ///     Argument 2: A pointer to the scene node to select.
        ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
        int SelectSceneNode(GT::Script &script);

        /// Deselects the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor containing the scene node.
        ///     Argument 2: A pointer to the scene node to deselect.
        ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
        int DeselectSceneNode(GT::Script &script);

        /// Retrieves the number of nodes that are currently selected.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetSelectedSceneNodeCount(GT::Script &script);

        /// Retrieves a pointer to the first selected node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetFirstSelectedSceneNodePtr(GT::Script &script);

        /// Retrieves an array containing the selected scene node IDs.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetSelectedSceneNodeIDs(GT::Script &script);

        /// Determines if the given scene node is selected.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The ID of the scene node.
        int IsSceneNodeSelectedByID(GT::Script &script);


        /// Attempts to select a gizmo with the mouse.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///
        ///     Returns true if a gizmo was selected.
        int TryGizmoMouseSelect(GT::Script &script);

        /// Performs a mouse selection.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DoMouseSelection(GT::Script &script);


        /// Deletes all of the selected nodes.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int RemoveSelectedSceneNodes(GT::Script &script);

        /// Deletes the given node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node to delete.
        int RemoveSceneNode(GT::Script &script);

        /// Duplicates the selected scene nodes.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DuplicateSelectedSceneNodes(GT::Script &script);

        /// Duplicates the given node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node to duplicate.
        int DuplicateSceneNode(GT::Script &script);


        /// Switches the gizmo to translate mode.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int SwitchGizmoToTranslateMode(GT::Script &script);

        /// Switches the gizmo to rotate mode.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int SwitchGizmoToRotateMode(GT::Script &script);

        /// Switches the gizmo to scale mode.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int SwitchGizmoToScaleMode(GT::Script &script);

        /// Switches the gizmo to local space.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int SwitchGizmoToLocalSpace(GT::Script &script);

        /// Switches the gizmo to global space.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int SwitchGizmoToGlobalSpace(GT::Script &script);

        /// Toggles the gizmo space.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int ToggleGizmoSpace(GT::Script &script);

        /// Determines whether or not the gizmo is in local space.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsGizmoInLocalSpace(GT::Script &script);

        /// Determines whether or not the gizmo is in global space.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsGizmoInGlobalSpace(GT::Script &script);

        /// Updates the transformation of the selection gizmo.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int UpdateSelectionGizmoTransform(GT::Script &script);

        /// Sets the translation snapping interval.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The new snapping interval.
        int SetTranslationSnappingInterval(GT::Script &script);

        /// Retrieves the translation snapping interval.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetTranslationSnappingInterval(GT::Script &script);

        /// Enables snapping to the grid.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int EnableSnapToGrid(GT::Script &script);

        /// Disables snapping to the grid.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DisableSnapToGrid(GT::Script &script);

        /// Determines whether or not the editor is snapping to the grid.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsSnappingToGrid(GT::Script &script);


        /// Plays the game.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int StartPlaying(GT::Script &script);

        /// Pauses the game.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int PausePlaying(GT::Script &script);

        /// Stops the game.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int StopPlaying(GT::Script &script);

        /// Determines whether or not the game is playing.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsPlaying(GT::Script &script);

        /// Determines whether or not the game is paused.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsPaused(GT::Script &script);

        /// Determines whether or not the game is stopped (the default state).
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsStopped(GT::Script &script);


        /// Pauses updating of the scene that's currently being played.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int PauseSceneUpdates(GT::Script &script);

        /// Resumes updating of the scene that's currently being played.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int ResumeSceneUpdates(GT::Script &script);

        /// Determines whether or not scene updates are current paused in the editor.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Return:     True if scene updates are paused; false otherwise.
        int IsSceneUpdatesPaused(GT::Script &script);


        /// Enables physics simulation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int EnablePhysicsSimulation(GT::Script &script);

        /// Disable physics simulation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DisablePhysicsSimulation(GT::Script &script);

        /// Determines whether or not physics simulation is enabled.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsPhysicsSimulationEnabled(GT::Script &script);


        /// Commits a state stack frame for undo/redo operations.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int CommitStateStackFrame(GT::Script &script);

        /// Clears the state stack staging area for undo/redo operations.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int ClearStateStackStagingArea(GT::Script &script);

        /// Performs an undo operation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int Undo(GT::Script &script);

        /// Performs a redo operation.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int Redo(GT::Script &script);


        /// Creates a prefab from the given scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The absolute path to save the prefab file at.
        ///     Argument 3: The base path to use to convert the absolute path to a relative path.
        ///     Argument 4: A pointer to the scene node.
        int CreatePrefab(GT::Script &script);

        /// Instantiates a prefab, returning a pointer to the root node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The relative path of the prefab file.
        ///
        ///     This leaves the prefab positioned at the origin. Reposition the scene node manually if needed.
        int InstantiatePrefab(GT::Script &script);

        /// Links a scene node to a prefab.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node being linked.
        ///     Argument 3: The relative path of the prefab to link to.
        ///     Argument 4: Whether or not the given scene node is the source scene node.
        int LinkSceneNodeToPrefab(GT::Script &script);

        /// Unlinks a scene node from a prefab.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node being unlinked.
        int UnlinkSceneNodeFromPrefab(GT::Script &script);


        /// Retrieves a pointer to the scene node of the given ID.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The ID of the scene node to retrieve.
        int GetSceneNodePtrByID(GT::Script &script);

        /// Retrieves a array of pointers to the scene nodes in the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetSceneNodes(GT::Script &script);


        /// Positions the given scene node in front of the camera.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node.
        int PositionSceneNodeInFrontOfCamera(GT::Script &script);

        /// Sets the position of the given scene node to the current insertion position of the camera.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node.
        int SetSceneNodePositionToInsertionPosition(GT::Script &script);

        /// Sets the transform of the given scene node to that of the camera.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node.
        int SetSceneNodeTransformToCamera(GT::Script &script);

        /// Sets the position of the given scene node to that of the camera.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node.
        int SetSceneNodePositionToCamera(GT::Script &script);

        /// Sets the orientation of the given scene node to that of the camera.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node.
        int SetSceneNodeOrientationToCamera(GT::Script &script);

        /// Sets the scale of the given scene ndoe to that of the camera.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A pointer to the scene node.
        int SetSceneNodeScaleToCamera(GT::Script &script);


        /// Shows the grid if it isn't showing already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int ShowGrid(GT::Script &script);

        /// Hides the grid if it isn't hidden already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int HideGrid(GT::Script &script);

        /// Determines whether or not the grid is showing.
        ///
        /// @remakrs
        ///     Argument 1: A pointer to the scene editor.
        int IsShowingGrid(GT::Script &script);


        /// Shows the axis arrows if it isn't showing already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int ShowAxisArrows(GT::Script &script);

        /// Hides the axis arrows if it isn't hidden already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int HideAxisArrows(GT::Script &script);

        /// Determines whether or not the grid is showing.
        ///
        /// @remakrs
        ///     Argument 1: A pointer to the scene editor.
        int IsShowingAxisArrows(GT::Script &script);


        /// Retrieves a pointer to the viewport camera scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetViewportCameraSceneNodePtr(GT::Script &script);


        /// Sets the name of the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The new name of the scene.
        ///
        ///     The name of a scene is just metadata that games can use if needed. This is not the file name.
        int SetSceneName(GT::Script &script);

        /// Retrieves the name of the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///
        ///     The name of a scene is just metadata that games can use if needed. This is not the file name.
        int GetSceneName(GT::Script &script);


        /// Enables background clearing.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: A math.vec3 containing the new clear colour.
        int EnableSceneBackgroundClearing(GT::Script &script);

        /// Disables background clearing.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DisableSceneBackgroundClearing(GT::Script &script);

        /// Determines whether or not background clearing is enabled.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsSceneBackgroundClearingEnabled(GT::Script &script);

        /// Retrieves a math.vec3 containing the background clear colour.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int GetSceneBackgroundClearColour(GT::Script &script);


        /// Enables HDR on the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int EnableSceneHDR(GT::Script &script);

        /// Disables HDR on the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DisableSceneHDR(GT::Script &script);

        /// Determines whether or not HDR is enabled on the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsSceneHDREnabled(GT::Script &script);


        /// Enables bloom on the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int EnableSceneBloom(GT::Script &script);

        /// Disables bloom on the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int DisableSceneBloom(GT::Script &script);

        /// Determines whether or not bloom is enabled on the scene.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        int IsSceneBloomEnabled(GT::Script &script);


        /// Shows the navigation mesh at the given index.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The index of the navigation mesh to show.
        int ShowNavigationMesh(GT::Script &script);

        /// Hides the navigation mesh at the given index.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the scene editor.
        ///     Argument 2: The index of the navigation mesh to hide.
        int HideNavigationMesh(GT::Script &script);
    }


    namespace ParticleEditorFFI
    {
        /// Retrieves a pointer to the particle system definition being editted.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the particle edittor.
        int GetParticleSystemDefinitionPtr(GT::Script &script);


        /// Refreshes the viewport to show the current state of the particle system being modified.
        ///
        /// @remarks
        ///     This is called whenever a property of the particle system is modified.
        ///
        ///     Argument 1: A pointer to the particle editor.
        int RefreshViewport(GT::Script &script);


        /// Sets the orientation of the preview particle system.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the particle editor.
        int SetOrientation(GT::Script &script);


        /// Shows the grid if it isn't showing already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the particle editor.
        int ShowGrid(GT::Script &script);

        /// Hides the grid if it isn't hidden already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the particle editor.
        int HideGrid(GT::Script &script);

        /// Determines whether or not the grid is showing.
        ///
        /// @remakrs
        ///     Argument 1: A pointer to the particle editor.
        int IsShowingGrid(GT::Script &script);


        /// Shows the axis arrows if it isn't showing already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the particle editor.
        int ShowAxisArrows(GT::Script &script);

        /// Hides the axis arrows if it isn't hidden already.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the particle editor.
        int HideAxisArrows(GT::Script &script);

        /// Determines whether or not the grid is showing.
        ///
        /// @remakrs
        ///     Argument 1: A pointer to the particle editor.
        int IsShowingAxisArrows(GT::Script &script);


        /// Retrieves a pointer to the viewport camera scene node.
        ///
        /// @remarks
        ///     Argument 1: A pointer to the particle editor.
        int GetViewportCameraSceneNodePtr(GT::Script &script);
    }
}

#endif