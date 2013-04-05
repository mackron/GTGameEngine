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
        }

        namespace ModelEditorFFI
        {
            /// Retrieves the relative names of all of the materials in the model of the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetMaterials(GTCore::Script &script);

            /// Retrieves the settings used to build the convex hull of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int GetConvexHullBuildSettings(GTCore::Script &script);

            /// Plays the animation track of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int PlayAnimation(GTCore::Script &script);

            /// Stops the animation track of the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            int StopAnimation(GTCore::Script &script);

            /// Sets a material on the model loaded in the given model editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the model editor.
            ///     Argument 2: The index of the material being set.
            ///     Argument 3: The relative path of the material to use.
            int SetMaterial(GTCore::Script &script);

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


            /// Instantiates a prefab, returning a pointer to the root node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            ///     Argument 2: The relative path of the prefab file.
            ///
            ///     This leaves the prefab positioned at the origin. Reposition the scene node manually if needed.
            int InstantiatePrefab(GTCore::Script &script);


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


            /// Enables HDR.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnableHDR(GTCore::Script &script);

            /// Disables HDR.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableHDR(GTCore::Script &script);

            /// Determines whether or not HDR is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsHDREnabled(GTCore::Script &script);


            /// Enables bloom.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int EnableBloom(GTCore::Script &script);

            /// Disables bloom.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int DisableBloom(GTCore::Script &script);

            /// Determines whether or not bloom is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene editor.
            int IsBloomEnabled(GTCore::Script &script);
        }
    }
}

#endif