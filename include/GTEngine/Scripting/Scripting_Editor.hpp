// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Editor_hpp_
#define __GTEngine_Scripting_Editor_hpp_

#include "../GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the editor library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadEditorLibrary(GTLib::Script &script);


        namespace SubEditorFFI
        {
            /// Retrieves the absolute path of the file loaded in the given sub-editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor.
            int GetAbsolutePath(GTLib::Script &script);

            /// Retrieves the relative path of the file loaded in the given sub-editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor.
            int GetRelativePath(GTLib::Script &script);


            /// Marks the editor as modified.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor to mark as modified.
            int MarkAsModified(GTLib::Script &script);

            /// Unmarks the editor as modified.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor to unmark as modified.
            int UnmarkAsModified(GTLib::Script &script);
        }

        namespace ModelEditorFFI
        {
            /// Retrieves the names of every mesh making up the model of the given model editor.
            ///
            /// @remark
            ///     Argument 1: A pointer to the model editor.
            int GetMeshNames(GTLib::Script &script);


            /// Retrieves the relative names of all of the materials in the model of the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetMaterials(GTLib::Script &script);

            /// Retrieves the relative path of the material at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the mesh whose material is being retrieved.
            int GetMaterialRelativePath(GTLib::Script &script);

            /// Retrieves a table containing the uniforms of a material.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the mesh whose material's uniforms are being retrieved.
            ///
            ///     The return value is a table. Each item in the table is another table with the properties 'type' and 'value'. For textures, the
            ///     value is a string containing the relative path of the texture.
            int GetMaterialUniformValues(GTLib::Script &script);

            /// Sets a material on the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the material being set.
            ///     Argument 3: The relative path of the material to use.
            int SetMaterial(GTLib::Script &script);

            /// Sets the value of a uniform of the given meshes material.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the material/mesh whose uniform is getting set.
            ///     Argument 3: The name of the uniform getting set.
            ///     Argument 4: The value of the uniform.
            int SetMaterialUniform_Float(GTLib::Script &script);
            int SetMaterialUniform_Float2(GTLib::Script &script);
            int SetMaterialUniform_Float3(GTLib::Script &script);
            int SetMaterialUniform_Float4(GTLib::Script &script);
            int SetMaterialUniform_Texture2D(GTLib::Script &script);
            

            /// Retrieves the number of bones containing the in the model of the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetBoneCount(GTLib::Script &script);

            /// Retrieves an array containing the names of every bone.
            ///
            /// @remarks
            ///     There is 1 table for every bone in the array. Each item contains two properties which should be self explanatory: 'name' and 'parentName'.
            ///
            ///     Argument 1: A pointer to the model editor.
            int GetBones(GTLib::Script &script);


            /// Plays the animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment to play.
            ///
            ///     The animation will be looped.
            int PlayAnimationSegmentByIndex(GTLib::Script &script);

            /// Plays the animation track of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int PlayAnimation(GTLib::Script &script);

            /// Resumes playback of the model's animation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int ResumeAnimation(GTLib::Script &script);

            /// Stops the animation track of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int StopAnimation(GTLib::Script &script);

            /// Pauses playback of the model's animation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int PauseAnimation(GTLib::Script &script);

            /// Determines whether or not the model is currently animating.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int IsAnimationPlaying(GTLib::Script &script);

            /// Determines whether or not the model is currently paused.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int IsAnimationPaused(GTLib::Script &script);


            /// Adds a new animation segment to the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The name of the new animation segment.
            ///     Argument 3: The start frame index.
            ///     Argument 4: The end frame index.
            int AddNewAnimationSegment(GTLib::Script &script);

            /// Removes an animation segment by it's index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment to remove.
            int RemoveAnimationSegmentByIndex(GTLib::Script &script);

            /// Sets the name of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose name is being set.
            ///     Argument 3: The new name.
            int SetAnimationSegmentName(GTLib::Script &script);

            /// Retrieves the name of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose name is being retrieved.
            int GetAnimationSegmentName(GTLib::Script &script);

            /// Sets the start and end frame indices of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose frame indices are being set.
            ///     Argument 3: The start index.
            ///     Argument 4: The end index.
            int SetAnimationSegmentFrameIndices(GTLib::Script &script);

            /// Retrieves the start and end frame indices of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose frame indices are being retrieved.
            ///     
            //      The return value is two integers.
            int GetAnimationSegmentFrameIndices(GTLib::Script &script);

            /// Retrieves a list of animation segments currently attached to the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///
            ///     Each item in the returned list is a table containing 'name', 'startKeyFrame' and 'endKeyFrame'.
            int GetAnimationSegments(GTLib::Script &script);


            /// Retrieves the settings used to build the convex hull of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetConvexHullBuildSettings(GTLib::Script &script);

            /// Shows the convex decomposition of the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int ShowConvexDecomposition(GTLib::Script &script);

            /// Hides the convex decomposition of the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int HideConvexDecomposition(GTLib::Script &script);

            /// Builds the convex decomposition of the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int BuildConvexDecomposition(GTLib::Script &script);


            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetViewportCameraSceneNodePtr(GTLib::Script &script);


            /// Retrieves the AABB of the model of the given model editor as two math.vec3s.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///
            ///     The return value is two math.vec3s. The first is the lower bounds and the second is the upper bounds.
            int GetModelAABB(GTLib::Script &script);
        }

        namespace MaterialEditorFFI
        {
            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetViewportCameraSceneNodePtr(GTLib::Script &script);
        }

        namespace SceneEditorFFI
        {
            /// Retrieves the C++ pointer to the scene of the given scene editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetScenePtr(GTLib::Script &script);


            /// Sets the insertion position.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A math.vec3 containing the new insertion position.
            int SetInsertionPosition(GTLib::Script &script);

            /// Retrieves the insertion position.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Return: A math.vec3 containing the insertion position, in world space.
            int GetInsertionPosition(GTLib::Script &script);

            /// Updates the insertion position based on the current position of the mouse.
            ///
            /// @remarks
            ///     This will perform a ray cast againt the selection volumes of every scene node, starting from the position of the mouse in the viewport. If
            ///     nothing is hit, it will attempt a ray cast against a small section of the X/Z plane. If that is not hit, it will set the insertion position
            ///     to just in front of the camera.
            ///
            ///     Argument 1: A pointer to the scene editor.
            int UpdateInsertionPositionFromMouse(GTLib::Script &script);

            /// Updates the insertion position such that it is placed just in front of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int UpdateInsertionPositionToInFrontOfCamera(GTLib::Script &script);


            /// Deselects everything in the given scene editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DeselectAll(GTLib::Script &script);

            /// Selects the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor containing the scene node.
            ///     Argument 2: A pointer to the scene node to select.
            ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
            int SelectSceneNode(GTLib::Script &script);

            /// Deselects the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor containing the scene node.
            ///     Argument 2: A pointer to the scene node to deselect.
            ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
            int DeselectSceneNode(GTLib::Script &script);

            /// Retrieves the number of nodes that are currently selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSelectedSceneNodeCount(GTLib::Script &script);

            /// Retrieves a pointer to the first selected node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetFirstSelectedSceneNodePtr(GTLib::Script &script);

            /// Retrieves an array containing the selected scene node IDs.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSelectedSceneNodeIDs(GTLib::Script &script);

            /// Determines if the given scene node is selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The ID of the scene node.
            int IsSceneNodeSelectedByID(GTLib::Script &script);


            /// Attempts to select a gizmo with the mouse.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///
            ///     Returns true if a gizmo was selected.
            int TryGizmoMouseSelect(GTLib::Script &script);

            /// Performs a mouse selection.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DoMouseSelection(GTLib::Script &script);


            /// Deletes all of the selected nodes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int RemoveSelectedSceneNodes(GTLib::Script &script);

            /// Deletes the given node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node to delete.
            int RemoveSceneNode(GTLib::Script &script);

            /// Duplicates the selected scene nodes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DuplicateSelectedSceneNodes(GTLib::Script &script);

            /// Duplicates the given node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node to duplicate.
            int DuplicateSceneNode(GTLib::Script &script);


            /// Switches the gizmo to translate mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToTranslateMode(GTLib::Script &script);

            /// Switches the gizmo to rotate mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToRotateMode(GTLib::Script &script);

            /// Switches the gizmo to scale mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToScaleMode(GTLib::Script &script);

            /// Switches the gizmo to local space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToLocalSpace(GTLib::Script &script);

            /// Switches the gizmo to global space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToGlobalSpace(GTLib::Script &script);

            /// Toggles the gizmo space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ToggleGizmoSpace(GTLib::Script &script);

            /// Determines whether or not the gizmo is in local space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsGizmoInLocalSpace(GTLib::Script &script);

            /// Determines whether or not the gizmo is in global space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsGizmoInGlobalSpace(GTLib::Script &script);

            /// Updates the transformation of the selection gizmo.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int UpdateSelectionGizmoTransform(GTLib::Script &script);

            /// Sets the translation snapping interval.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The new snapping interval.
            int SetTranslationSnappingInterval(GTLib::Script &script);

            /// Retrieves the translation snapping interval.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetTranslationSnappingInterval(GTLib::Script &script);

            /// Enables snapping to the grid.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnableSnapToGrid(GTLib::Script &script);

            /// Disables snapping to the grid.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableSnapToGrid(GTLib::Script &script);

            /// Determines whether or not the editor is snapping to the grid.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsSnappingToGrid(GTLib::Script &script);


            /// Plays the game.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int StartPlaying(GTLib::Script &script);

            /// Pauses the game.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int PausePlaying(GTLib::Script &script);

            /// Stops the game.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int StopPlaying(GTLib::Script &script);

            /// Determines whether or not the game is playing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsPlaying(GTLib::Script &script);

            /// Determines whether or not the game is paused.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsPaused(GTLib::Script &script);

            /// Determines whether or not the game is stopped (the default state).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsStopped(GTLib::Script &script);


            /// Pauses updating of the scene that's currently being played.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int PauseSceneUpdates(GTLib::Script &script);

            /// Resumes updating of the scene that's currently being played.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ResumeSceneUpdates(GTLib::Script &script);


            /// Enables physics simulation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnablePhysicsSimulation(GTLib::Script &script);

            /// Disable physics simulation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisablePhysicsSimulation(GTLib::Script &script);

            /// Determines whether or not physics simulation is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsPhysicsSimulationEnabled(GTLib::Script &script);


            /// Commits a state stack frame for undo/redo operations.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int CommitStateStackFrame(GTLib::Script &script);

            /// Clears the state stack staging area for undo/redo operations.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ClearStateStackStagingArea(GTLib::Script &script);

            /// Performs an undo operation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int Undo(GTLib::Script &script);

            /// Performs a redo operation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int Redo(GTLib::Script &script);


            /// Creates a prefab from the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The absolute path to save the prefab file at.
            ///     Argument 3: The base path to use to convert the absolute path to a relative path.
            ///     Argument 4: A pointer to the scene node.
            int CreatePrefab(GTLib::Script &script);

            /// Instantiates a prefab, returning a pointer to the root node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The relative path of the prefab file.
            ///
            ///     This leaves the prefab positioned at the origin. Reposition the scene node manually if needed.
            int InstantiatePrefab(GTLib::Script &script);

            /// Links a scene node to a prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node being linked.
            ///     Argument 3: The relative path of the prefab to link to.
            ///     Argument 4: Whether or not the given scene node is the source scene node.
            int LinkSceneNodeToPrefab(GTLib::Script &script);

            /// Unlinks a scene node from a prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node being unlinked.
            int UnlinkSceneNodeFromPrefab(GTLib::Script &script);


            /// Retrieves a pointer to the scene node of the given ID.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The ID of the scene node to retrieve.
            int GetSceneNodePtrByID(GTLib::Script &script);

            /// Retrieves a array of pointers to the scene nodes in the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSceneNodes(GTLib::Script &script);


            /// Positions the given scene node in front of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int PositionSceneNodeInFrontOfCamera(GTLib::Script &script);

            /// Sets the position of the given scene node to the current insertion position of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodePositionToInsertionPosition(GTLib::Script &script);

            /// Sets the transform of the given scene node to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodeTransformToCamera(GTLib::Script &script);

            /// Sets the position of the given scene node to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodePositionToCamera(GTLib::Script &script);

            /// Sets the orientation of the given scene node to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodeOrientationToCamera(GTLib::Script &script);

            /// Sets the scale of the given scene ndoe to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodeScaleToCamera(GTLib::Script &script);


            /// Shows the grid if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ShowGrid(GTLib::Script &script);

            /// Hides the grid if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int HideGrid(GTLib::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the scene editor.
            int IsShowingGrid(GTLib::Script &script);


            /// Shows the axis arrows if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ShowAxisArrows(GTLib::Script &script);

            /// Hides the axis arrows if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int HideAxisArrows(GTLib::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the scene editor.
            int IsShowingAxisArrows(GTLib::Script &script);


            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetViewportCameraSceneNodePtr(GTLib::Script &script);


            /// Sets the name of the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The new name of the scene.
            ///
            ///     The name of a scene is just metadata that games can use if needed. This is not the file name.
            int SetSceneName(GTLib::Script &script);

            /// Retrieves the name of the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///
            ///     The name of a scene is just metadata that games can use if needed. This is not the file name.
            int GetSceneName(GTLib::Script &script);


            /// Enables background clearing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A math.vec3 containing the new clear colour.
            int EnableSceneBackgroundClearing(GTLib::Script &script);

            /// Disables background clearing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableSceneBackgroundClearing(GTLib::Script &script);

            /// Determines whether or not background clearing is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsSceneBackgroundClearingEnabled(GTLib::Script &script);

            /// Retrieves a math.vec3 containing the background clear colour.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSceneBackgroundClearColour(GTLib::Script &script);


            /// Enables HDR on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnableSceneHDR(GTLib::Script &script);

            /// Disables HDR on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableSceneHDR(GTLib::Script &script);

            /// Determines whether or not HDR is enabled on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsSceneHDREnabled(GTLib::Script &script);


            /// Enables bloom on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnableSceneBloom(GTLib::Script &script);

            /// Disables bloom on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableSceneBloom(GTLib::Script &script);

            /// Determines whether or not bloom is enabled on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsSceneBloomEnabled(GTLib::Script &script);


            /// Shows the navigation mesh at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The index of the navigation mesh to show.
            int ShowNavigationMesh(GTLib::Script &script);

            /// Hides the navigation mesh at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The index of the navigation mesh to hide.
            int HideNavigationMesh(GTLib::Script &script);
        }


        namespace ParticleEditorFFI
        {
            /// Retrieves a pointer to the particle system definition being editted.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle edittor.
            int GetParticleSystemDefinitionPtr(GTLib::Script &script);


            /// Refreshes the viewport to show the current state of the particle system being modified.
            ///
            /// @remarks
            ///     This is called whenever a property of the particle system is modified.
            ///
            ///     Argument 1: A pointer to the particle editor.
            int RefreshViewport(GTLib::Script &script);


            /// Sets the orientation of the preview particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int SetOrientation(GTLib::Script &script);


            /// Shows the grid if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int ShowGrid(GTLib::Script &script);

            /// Hides the grid if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int HideGrid(GTLib::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the particle editor.
            int IsShowingGrid(GTLib::Script &script);


            /// Shows the axis arrows if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int ShowAxisArrows(GTLib::Script &script);

            /// Hides the axis arrows if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int HideAxisArrows(GTLib::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the particle editor.
            int IsShowingAxisArrows(GTLib::Script &script);


            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int GetViewportCameraSceneNodePtr(GTLib::Script &script);
        }
    }
}

#endif