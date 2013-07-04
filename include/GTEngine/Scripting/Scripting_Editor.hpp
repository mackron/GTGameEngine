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
        bool LoadEditorLibrary(GTCore::Script &script);


        namespace SubEditorFFI
        {
            /// Retrieves the absolute path of the file loaded in the given sub-editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor.
            int GetAbsolutePath(GTCore::Script &script);

            /// Retrieves the relative path of the file loaded in the given sub-editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor.
            int GetRelativePath(GTCore::Script &script);


            /// Marks the editor as modified.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor to mark as modified.
            int MarkAsModified(GTCore::Script &script);

            /// Unmarks the editor as modified.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the sub-editor to unmark as modified.
            int UnmarkAsModified(GTCore::Script &script);
        }

        namespace ModelEditorFFI
        {
            /// Retrieves the names of every mesh making up the model of the given model editor.
            ///
            /// @remark
            ///     Argument 1: A pointer to the model editor.
            int GetMeshNames(GTCore::Script &script);


            /// Retrieves the relative names of all of the materials in the model of the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetMaterials(GTCore::Script &script);

            /// Sets a material on the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the material being set.
            ///     Argument 3: The relative path of the material to use.
            int SetMaterial(GTCore::Script &script);
            

            /// Retrieves the number of bones containing the in the model of the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetBoneCount(GTCore::Script &script);

            /// Retrieves an array containing the names of every bone.
            ///
            /// @remarks
            ///     There is 1 table for every bone in the array. Each item contains two properties which should be self explanatory: 'name' and 'parentName'.
            ///
            ///     Argument 1: A pointer to the model editor.
            int GetBones(GTCore::Script &script);


            /// Plays the animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment to play.
            ///
            ///     The animation will be looped.
            int PlayAnimationSegmentByIndex(GTCore::Script &script);

            /// Plays the animation track of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int PlayAnimation(GTCore::Script &script);

            /// Resumes playback of the model's animation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int ResumeAnimation(GTCore::Script &script);

            /// Stops the animation track of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int StopAnimation(GTCore::Script &script);

            /// Pauses playback of the model's animation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int PauseAnimation(GTCore::Script &script);

            /// Determines whether or not the model is currently animating.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int IsAnimationPlaying(GTCore::Script &script);

            /// Determines whether or not the model is currently paused.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int IsAnimationPaused(GTCore::Script &script);


            /// Adds a new animation segment to the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The name of the new animation segment.
            ///     Argument 3: The start frame index.
            ///     Argument 4: The end frame index.
            int AddNewAnimationSegment(GTCore::Script &script);

            /// Removes an animation segment by it's index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment to remove.
            int RemoveAnimationSegmentByIndex(GTCore::Script &script);

            /// Sets the name of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose name is being set.
            ///     Argument 3: The new name.
            int SetAnimationSegmentName(GTCore::Script &script);

            /// Retrieves the name of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose name is being retrieved.
            int GetAnimationSegmentName(GTCore::Script &script);

            /// Sets the start and end frame indices of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose frame indices are being set.
            ///     Argument 3: The start index.
            ///     Argument 4: The end index.
            int SetAnimationSegmentFrameIndices(GTCore::Script &script);

            /// Retrieves the start and end frame indices of the given animation segment.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the animation segment whose frame indices are being retrieved.
            ///     
            //      The return value is two integers.
            int GetAnimationSegmentFrameIndices(GTCore::Script &script);

            /// Retrieves a list of animation segments currently attached to the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///
            ///     Each item in the returned list is a table containing 'name', 'startKeyFrame' and 'endKeyFrame'.
            int GetAnimationSegments(GTCore::Script &script);


            /// Retrieves the settings used to build the convex hull of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetConvexHullBuildSettings(GTCore::Script &script);

            /// Shows the convex decomposition of the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int ShowConvexDecomposition(GTCore::Script &script);

            /// Hides the convex decomposition of the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int HideConvexDecomposition(GTCore::Script &script);

            /// Builds the convex decomposition of the model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int BuildConvexDecomposition(GTCore::Script &script);


            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetViewportCameraSceneNodePtr(GTCore::Script &script);


            /// Retrieves the AABB of the model of the given model editor as two math.vec3s.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///
            ///     The return value is two math.vec3s. The first is the lower bounds and the second is the upper bounds.
            int GetModelAABB(GTCore::Script &script);
        }

        namespace MaterialEditorFFI
        {
            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetViewportCameraSceneNodePtr(GTCore::Script &script);
        }

        namespace SceneEditorFFI
        {
            /// Retrieves the C++ pointer to the scene of the given scene editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetScenePtr(GTCore::Script &script);

            /// Deselects everything in the given scene editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DeselectAll(GTCore::Script &script);

            /// Selects the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor containing the scene node.
            ///     Argument 2: A pointer to the scene node to select.
            ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
            int SelectSceneNode(GTCore::Script &script);

            /// Deselects the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor containing the scene node.
            ///     Argument 2: A pointer to the scene node to deselect.
            ///     Argument 3: A boolean controlling whether or not to notify the editor of the change. A value of false means the editor WILL receive notification.
            int DeselectSceneNode(GTCore::Script &script);

            /// Retrieves the number of nodes that are currently selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSelectedSceneNodeCount(GTCore::Script &script);

            /// Retrieves a pointer to the first selected node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetFirstSelectedSceneNodePtr(GTCore::Script &script);

            /// Retrieves an array containing the selected scene node IDs.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSelectedSceneNodeIDs(GTCore::Script &script);

            /// Determines if the given scene node is selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The ID of the scene node.
            int IsSceneNodeSelectedByID(GTCore::Script &script);


            /// Attempts to select a gizmo with the mouse.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///
            ///     Returns true if a gizmo was selected.
            int TryGizmoMouseSelect(GTCore::Script &script);

            /// Performs a mouse selection.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DoMouseSelection(GTCore::Script &script);


            /// Deletes all of the selected nodes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int RemoveSelectedSceneNodes(GTCore::Script &script);

            /// Deletes the given node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node to delete.
            int RemoveSceneNode(GTCore::Script &script);

            /// Duplicates the selected scene nodes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DuplicateSelectedSceneNodes(GTCore::Script &script);

            /// Duplicates the given node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node to duplicate.
            int DuplicateSceneNode(GTCore::Script &script);


            /// Switches the gizmo to translate mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToTranslateMode(GTCore::Script &script);

            /// Switches the gizmo to rotate mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToRotateMode(GTCore::Script &script);

            /// Switches the gizmo to scale mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToScaleMode(GTCore::Script &script);

            /// Switches the gizmo to local space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToLocalSpace(GTCore::Script &script);

            /// Switches the gizmo to global space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int SwitchGizmoToGlobalSpace(GTCore::Script &script);

            /// Toggles the gizmo space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ToggleGizmoSpace(GTCore::Script &script);

            /// Determines whether or not the gizmo is in local space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsGizmoInLocalSpace(GTCore::Script &script);

            /// Determines whether or not the gizmo is in global space.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsGizmoInGlobalSpace(GTCore::Script &script);


            /// Updates the transformation of the selection gizmo.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int UpdateSelectionGizmoTransform(GTCore::Script &script);


            /// Plays the game.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int StartPlaying(GTCore::Script &script);

            /// Pauses the game.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int PausePlaying(GTCore::Script &script);

            /// Stops the game.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int StopPlaying(GTCore::Script &script);

            /// Determines whether or not the game is playing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsPlaying(GTCore::Script &script);

            /// Determines whether or not the game is paused.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsPaused(GTCore::Script &script);

            /// Determines whether or not the game is stopped (the default state).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsStopped(GTCore::Script &script);


            /// Enables physics simulation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnablePhysicsSimulation(GTCore::Script &script);

            /// Disable physics simulation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisablePhysicsSimulation(GTCore::Script &script);

            /// Determines whether or not physics simulation is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsPhysicsSimulationEnabled(GTCore::Script &script);


            /// Commits a state stack frame for undo/redo operations.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int CommitStateStackFrame(GTCore::Script &script);

            /// Clears the state stack staging area for undo/redo operations.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ClearStateStackStagingArea(GTCore::Script &script);

            /// Performs an undo operation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int Undo(GTCore::Script &script);

            /// Performs a redo operation.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int Redo(GTCore::Script &script);


            /// Creates a prefab from the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The absolute path to save the prefab file at.
            ///     Argument 3: The base path to use to convert the absolute path to a relative path.
            ///     Argument 4: A pointer to the scene node.
            int CreatePrefab(GTCore::Script &script);

            /// Instantiates a prefab, returning a pointer to the root node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The relative path of the prefab file.
            ///
            ///     This leaves the prefab positioned at the origin. Reposition the scene node manually if needed.
            int InstantiatePrefab(GTCore::Script &script);

            /// Links a scene node to a prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node being linked.
            ///     Argument 3: The relative path of the prefab to link to.
            ///     Argument 4: Whether or not the given scene node is the source scene node.
            int LinkSceneNodeToPrefab(GTCore::Script &script);

            /// Unlinks a scene node from a prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node being unlinked.
            int UnlinkSceneNodeFromPrefab(GTCore::Script &script);


            /// Retrieves a pointer to the scene node of the given ID.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The ID of the scene node to retrieve.
            int GetSceneNodePtrByID(GTCore::Script &script);

            /// Retrieves a array of pointers to the scene nodes in the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSceneNodes(GTCore::Script &script);


            /// Positions the given scene node in front of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int PositionSceneNodeInFrontOfCamera(GTCore::Script &script);

            /// Sets the transform of the given scene node to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodeTransformToCamera(GTCore::Script &script);

            /// Sets the position of the given scene node to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodePositionToCamera(GTCore::Script &script);

            /// Sets the orientation of the given scene node to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodeOrientationToCamera(GTCore::Script &script);

            /// Sets the scale of the given scene ndoe to that of the camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A pointer to the scene node.
            int SetSceneNodeScaleToCamera(GTCore::Script &script);


            /// Shows the grid if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ShowGrid(GTCore::Script &script);

            /// Hides the grid if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int HideGrid(GTCore::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the scene editor.
            int IsShowingGrid(GTCore::Script &script);


            /// Shows the axis arrows if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int ShowAxisArrows(GTCore::Script &script);

            /// Hides the axis arrows if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int HideAxisArrows(GTCore::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the scene editor.
            int IsShowingAxisArrows(GTCore::Script &script);


            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetViewportCameraSceneNodePtr(GTCore::Script &script);


            /// Sets the name of the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The new name of the scene.
            ///
            ///     The name of a scene is just metadata that games can use if needed. This is not the file name.
            int SetSceneName(GTCore::Script &script);

            /// Retrieves the name of the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///
            ///     The name of a scene is just metadata that games can use if needed. This is not the file name.
            int GetSceneName(GTCore::Script &script);


            /// Enables background clearing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: A math.vec3 containing the new clear colour.
            int EnableSceneBackgroundClearing(GTCore::Script &script);

            /// Disables background clearing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableSceneBackgroundClearing(GTCore::Script &script);

            /// Determines whether or not background clearing is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsSceneBackgroundClearingEnabled(GTCore::Script &script);

            /// Retrieves a math.vec3 containing the background clear colour.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int GetSceneBackgroundClearColour(GTCore::Script &script);


            /// Enables HDR on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnableSceneHDR(GTCore::Script &script);

            /// Disables HDR on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableSceneHDR(GTCore::Script &script);

            /// Determines whether or not HDR is enabled on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsSceneHDREnabled(GTCore::Script &script);


            /// Enables bloom on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnableSceneBloom(GTCore::Script &script);

            /// Disables bloom on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableSceneBloom(GTCore::Script &script);

            /// Determines whether or not bloom is enabled on the scene.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsSceneBloomEnabled(GTCore::Script &script);
        }


        namespace ParticleEditorFFI
        {
            /// Retrieves a pointer to the particle system definition being editted.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle edittor.
            int GetParticleSystemDefinitionPtr(GTCore::Script &script);


            /// Refreshes the viewport to show the current state of the particle system being modified.
            ///
            /// @remarks
            ///     This is called whenever a property of the particle system is modified.
            ///
            ///     Argument 1: A pointer to the particle editor.
            int RefreshViewport(GTCore::Script &script);


            /// Sets the orientation of the preview particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int SetOrientation(GTCore::Script &script);


            /// Shows the grid if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int ShowGrid(GTCore::Script &script);

            /// Hides the grid if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int HideGrid(GTCore::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the particle editor.
            int IsShowingGrid(GTCore::Script &script);


            /// Shows the axis arrows if it isn't showing already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int ShowAxisArrows(GTCore::Script &script);

            /// Hides the axis arrows if it isn't hidden already.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int HideAxisArrows(GTCore::Script &script);

            /// Determines whether or not the grid is showing.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the particle editor.
            int IsShowingAxisArrows(GTCore::Script &script);


            /// Retrieves a pointer to the viewport camera scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle editor.
            int GetViewportCameraSceneNodePtr(GTCore::Script &script);
        }
    }
}

#endif