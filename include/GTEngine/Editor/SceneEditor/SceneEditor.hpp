// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneEditor_hpp_
#define __GTEngine_SceneEditor_hpp_

#include "SceneEditorCommand.hpp"
#include "SceneEditorPrefabLinker.hpp"
#include "../SubEditor.hpp"
#include "../TransformGizmo.hpp"
#include "../EditorGrid.hpp"
#include "../EditorAxisArrows.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../SceneEditor/SceneEditorSceneEventHandler.hpp"
#include "../SceneEditor/SceneEditorSceneUpdateManager.hpp"
#include "../SceneEditor/SceneEditorPhysicsManager.hpp"
#include "../../DefaultSceneCullingManager.hpp"
#include "../../Scene.hpp"
#include "../../GameScript.hpp"
#include "../../Physics/CollisionWorld.hpp"
#include "../../ModelLibrary.hpp"
#include "../../Texture2DLibrary.hpp"

namespace GTEngine
{
    /// Class representing a scene editor.
    class SceneEditor : public SubEditor
    {
    public:

        /// Constructor.
        SceneEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~SceneEditor();


        /// Retrieves a reference to the scene.
              Scene & GetScene()       { return this->scene; }
        const Scene & GetScene() const { return this->scene; }


        /// Resets the camera to the default position.
        void ResetCamera();

        /// Sets the rotation of the camera.
        ///
        /// @param xRotation [in] The new x rotation.
        /// @param yRotation [in] The new y rotation.
        void SetCameraRotation(float xRotation, float yRotation);

        /// Retrieves a reference ot the camera scene node.
        const SceneNode & GetCameraSceneNode() const { return this->camera; }


        /// Starts playing the game.
        void StartPlaying();

        /// Pauses playing the game.
        void PausePlaying();

        /// Stops playing the game.
        void StopPlaying();

        /// Determines whether or not the game is playing.
        bool IsPlaying() const;

        /// Determines whether or not the game is paused.
        bool IsPaused() const;


        /// Enables physics simulation of the entire scene.
        void EnablePhysicsSimulation();

        /// Disables the physics simulation of the entire scene, restoring the scene back to normal.
        void DisablePhysicsSimulation();

        /// Determines whether or not the physics simulation is running.
        bool IsPhysicsSimulationEnabled() const;


        /// Retrieves a scene node by it's ID, or null if the scene node is not currently instantiated.
              SceneNode* GetSceneNodeByID(uint64_t id);
        const SceneNode* GetSceneNodeByID(uint64_t id) const;



        /// Shows the grid.
        void ShowGrid();

        /// Hides the grid.
        void HideGrid();

        /// Determines whether or not the grid is showing.
        bool IsShowingGrid() const;


        /// Shows the axis arrows.
        void ShowAxisArrows();

        /// Hides the axis arrows.
        void HideAxisArrows();

        /// Determines whether or not the axis arrows is showing.
        bool IsShowingAxisArrows() const;



        /// Sets the scene name.
        void SetSceneName(const char* sceneName);

        /// Retrieves the scene name.
        const char* GetSceneName() const;

        
        /// Marks background clearing as enabled on the scene.
        void EnableSceneBackgroundClearing(float r, float g, float b, bool markAsModified = true);
        void EnableSceneBackgroundClearing(const glm::vec3 &colour, bool markAsModified = true) { this->EnableSceneBackgroundClearing(colour.x, colour.y, colour.z, markAsModified); }
        
        /// Marks background clearing as disabled on the scene.
        void DisableSceneBackgroundClearing(bool markAsModified = true);

        /// Determines whether or not background clearing is enabled on the scene.
        bool IsSceneBackgroundClearingEnabled() const;

        /// Retrieves the scene's background clear colour.
        const glm::vec3 GetSceneBackgroundClearColour() const;

        
        /// Marks HDR as enabled on the scene.
        void EnableSceneHDR(bool markAsModified = true);

        /// Marks HDR as disabled on the scene.
        void DisableSceneHDR(bool markAsModified = true);

        /// Determines whether or not HDR is enabled on the scene.
        bool IsSceneHDREnabled() const;


        /// Marks bloom as enabled on the scene.
        void EnableSceneBloom(bool markAsModified = true);

        /// Marks bloom as disabled on the scene.
        void DisableSceneBloom(bool markAsModified = true);

        /// Determines whether or not bloom is enabled on the scene.
        bool IsSceneBloomEnabled() const;
        



        ///////////////////////////////////////////////////
        // Selections
        //
        // When selecting a scene node, various options can be specified to control particular operations. Instead of passing
        // a bunch of booleans, we'll instead use flags. The available flags are specified in the SelectionOptions enum.

        enum SelectionOptions
        {
            SelectionOption_NoScriptNotify = (1 << 1),
            SelectionOption_Force          = (1 << 2),
            SelectionOption_NoStateStaging = (1 << 3)
        };

        /// Attempts to select a gizmo with the mouse.
        ///
        /// @return True if a gizmo was selected.
        bool TryGizmoMouseSelect();

        /// Performs a mouse selection.
        void DoMouseSelection();

        /// Deselects everything.
        void DeselectAll(unsigned int options);

        /// Determines whether or not the given node is selected.
        ///
        /// @param node [in] The node to check.
        bool IsSceneNodeSelected(const SceneNode &node) const;

        /// Determines whether or not an ancestor
        ///
        /// @param node [in] The node to check.
        ///
        /// @remarks
        ///     If the node has no parent, false will be returned.
        bool IsAncestorSelected(const SceneNode &node) const;

        /// Determines if at least one scene node is selected.
        bool IsAnythingSelected() const;

        /// Selects the given scene node.
        ///
        /// @param node  [in] The node to select.
        /// @param force [in] Normally the scene node will not go through a full selection process if it is already marked as selected - this allows that operato to be forced, which is required sometimes.
        void SelectSceneNode(SceneNode &node, unsigned int options);

        /// Selects the given scene nodes.
        ///
        /// @param sceneNodeIDs [in] The IDs of the scene nodes to select.
        void SelectSceneNodes(const GTCore::Vector<uint64_t> &selectedNodeIDs, unsigned int options);

        /// Deselects the given scene node.
        ///
        /// @param node [in] The node to deselect.
        void DeselectSceneNode(SceneNode &node, unsigned int options);

        /// Recursively deselects the given scene node and it's children.
        ///
        /// @param node [in] A reference to the root scene node to deselect.
        void DeselectSceneNodeAndChildren(SceneNode &node, unsigned int options);

        /// Retrieves the center point of the selected nodes.
        glm::vec3 GetSelectionCenterPoint() const;

        /// Retrieves the orientation to use with the gizmo based on the current selection and transform settings.
        ///
        /// @remarks
        ///     If there are multiple selections, this will return the default quaternion.
        glm::quat GetGizmoRotation() const;

        /// Retrieves the number of selected nodes.
        size_t GetSelectedSceneNodeCount() const;

        /// Retrieves a pointer to the first selected node.
        SceneNode* GetFirstSelectedSceneNode();

        /// Fills a vector with the IDs of the selected nodes.
        void GetSelectedSceneNodeIDs(GTCore::Vector<uint64_t> &sceneNodeIDsOut);




        ///////////////////////////////////////////////////
        // Editting

        /// Deletes the selected scene nodes.
        void RemoveSelectedSceneNodes();

        /// Deletes the given scene node.
        void RemoveSceneNode(SceneNode &sceneNodeToRemove);

        /// Deletes the given scene nodes.
        ///
        /// @param sceneNodeIDs [in] A constant reference to the vector containing the scene nodes to delete.
        void RemoveSceneNodes(const GTCore::Vector<uint64_t> &sceneNodeIDs);

        /// Duplicates the selected scene nodes, deselects them and then selects the new ones.
        void DuplicateSelectedSceneNodes();

        /// Duplicates the given scene node, deselects it and the selects the new one.
        ///
        /// @return A reference to the new node.
        SceneNode & DuplicateSceneNode(SceneNode &sceneNodeToDuplicate);


        /// Performs an undo operation.
        void Undo();

        /// Performs a redo operation.
        void Redo();

        /// Commits a new frame to the current branch of the scene's state stack.
        void CommitStateStackFrame();

        /// Clears the staging area of the current branch of the scene's state stack.
        void ClearStateStackStagingArea();


        /// Instantiates a scene prefab, positioning the root node at the origin.
        SceneNode* InstantiatePrefab(const char* relativePath);

        
        /// Links a scene node to a prefab.
        ///
        /// @param sceneNode          [in] A reference to the scene node to link.
        /// @param prefabRelativePath [in] The relative path of the prefab to link to.
        bool LinkSceneNodeToPrefab(SceneNode &sceneNode, const char* prefabRelativePath);

        /// Unlinks a scene node from it's prefab.
        ///
        /// @param sceneNode [in] A reference to the scene node that is being unlinked.
        void UnlinkSceneNodeFromPrefab(SceneNode &sceneNode);



        ///////////////////////////////////////////////////
        // Gizmo Control

        /// Switches the gizmo of the current state to translate mode.
        void SwitchGizmoToTranslateMode();

        /// Switches the gizmo of the current state to rotate mode.
        void SwitchGizmoToRotateMode();

        /// Switches the gizmo of the current state to scale mode.
        void SwitchGizmoToScaleMode();

        /// Switches the gizmo of the current state to local space.
        void SwitchGizmoToLocalSpace();

        /// Switches the gizmo of the current state to global space.
        void SwitchGizmoToGlobalSpace();

        /// Toggles the transform space of the gizmo of the current state.
        void ToggleGizmoSpace();

        /// Determines whether or not the gizmo of the current state is in local space.
        bool IsGizmoInLocalSpace() const;

        /// Determines whether or not the gizmo of the current state is in global space.s
        bool IsGizmoInGlobalSpace() const;



        ///////////////////////////////////////////////////
        // Scene Events
        //
        // These events are received from the scene event handler for the currently active scene.

        /// SceneEditorSceneEventHandler::OnSceneNodeAdded()
        void OnSceneNodeAdded(SceneNode &object);

        /// SceneEditorSceneEventHandler::OnSceneNodeRemoved()
        void OnSceneNodeRemoved(SceneNode &object);

        /// SceneEditorSceneEventHandler::OnSceneNodeNameChanged()
        void OnSceneNodeNameChanged(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeParentChanged()
        void OnSceneNodeParentChanged(SceneNode &node, SceneNode* previousParent);

        /// SceneEditorSceneEventHandler::OnSceneNodeTransform()
        void OnSceneNodeTransform(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeScale()
        void OnSceneNodeScale(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeHide()
        void OnSceneNodeHide(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeShow()
        void OnSceneNodeShow(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeComponentAdded()
        void OnSceneNodeComponentAdded(SceneNode &node, Component &component);

        /// SceneEditorSceneEventHandler::OnSceneNodeComponentRemoved()
        void OnSceneNodeComponentRemoved(SceneNode &node, Component &component);

        /// SceneEditorSceneEventHandler::OnSceneNodeComponentChanged()
        void OnSceneNodeComponentChanged(SceneNode &node, Component &component);

        /// SceneEditorSceneEventHandler::OnStateStackFrameCommitted()
        void OnStateStackFrameCommitted();


        ///////////////////////////////////////////////////
        // GUI Events.
        //
        // These events are received in response to certain GUI events.

        /// Called when the main viewport is resized.
        void OnViewportSize();

        /// Called when the main viewport has the mouse wheel turned while hovered over it.
        void OnViewportMouseWheel();


        ///////////////////////////////////////////////////
        // Misc. Events.
        
        /// Called when the camera node is traansformed.
        void OnCameraTransformed();


        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::GetMainElement()
              GTGUI::Element* GetMainElement()        { return this->GUI.Main; }
        const GTGUI::Element* GetMainElement( ) const { return this->GUI.Main; }

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        bool Save();

        /// SubEditor::Update()
        void OnUpdate(double deltaTimeInSeconds);

        /// SubEditor::OnMouseButtonDown()
        void OnMouseButtonDown(GTCore::MouseButton button, int x, int y);

        /// SubEditor::OnMouseButtonUp()
        void OnMouseButtonUp(GTCore::MouseButton button, int x, int y);

        /// SubEditor::OnFileUpdate()
        void OnFileUpdate(const DataFilesWatcher::Item &item);



    private:

        ///////////////////////////////////////////////////
        // Private Methods.

        /// Creates all of the system nodes (camera, gizmos, grid, etc) and adds them to the scene.
        void CreateAndAddSystemNodes();

        /// Applies the camera rotation to the camera node.
        void ApplyCameraRotation();

        /// Serializes the scene.
        ///
        /// @param serializer        [in] A reference to the serializer to write to.
        /// @param serializeMetadata [in] Whether or not metadata such as the camera position should be serialized.
        void SerializeScene(GTCore::Serializer &serializer, bool serializeMetadata = true) const;

        /// Deserializes the scene.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        void DeserializeScene(GTCore::Deserializer &deserializer);

        /// Helper method for serializing the nodes defined in the given vector.
        ///
        /// @param sceneNodeIDs [in] The IDs of the scene nodes to serialize.
        /// @param serializer   [in] The serializer.
        ///
        /// @remarks
        ///     This just does a simple for loop over the scene nodes and serializes each one.
        void SerializeSceneNodes(const GTCore::Vector<size_t> &sceneNodeIDs, GTCore::Serializer &serializer);

        /// Helper method for deserializing the nodes defined in the given vector, adding them to the scene if necessary.
        ///
        /// @param sceneNodeIDs [in] The IDs of the scene nodes to deserialize.
        /// @param deserializer [in] The deserializer.
        ///
        /// @remarks
        ///     For any nodes that are not in the scene, they will be instantiated and re-added.
        void DeserializeSceneNodes(const GTCore::Vector<size_t> &sceneNodeIDs, GTCore::Deserializer &deserializer);



        /// Shows and repositions the positioning gizmo.
        void ShowGizmo();

        /// Hides whatever gizmo is currently showing.
        void HideGizmo();


        /// Updates the transformation of the gizmo.
        void UpdateGizmoTransform(bool onlyUpdateVisibleHandles = true);

        /// Updates the gizmo so that it's scaled, positioned and rotated properly.
        void UpdateGizmo();

        /// Shows the appropriate handles on the gizmo for the current 
        void ShowGizmoHandles();

        /// Determines whether or not the transform gizmo is being dragged.
        bool IsDraggingGizmo() const;


        /// Posts an OnSelectionChanged() event to the scene editor.
        void PostOnSelectionChangedEventToScript();

        /// Posts an OnSceneNodeAdded() event to the scene editor.
        void PostOnSceneNodeAddedToScript(SceneNode &node);

        /// Posts an OnSceneNodeRemoved() event to the scene editor.
        void PostOnSceneNodeRemovedToScript(SceneNode &node);

        /// Posts an OnSceneNodeNameChanged() event to the scene editor.
        void PostOnSceneNodeNameChangedToScript(SceneNode &node);

        /// Posts an OnSceneNodeParentChanged() event to the scene editor.
        void PostOnSceneNodeParentChangedToScript(SceneNode &node, SceneNode* previousParent);

        /// Posts an OnSceneNodePrefabChanged() event to the scene editor.
        void PostOnSceneNodePrefabChanged(SceneNode &node);


        /// Updates the Transform panel for the selected node, if any.
        void UpdatePropertiesTransformPanel();

        /// Updates the script properties on the editor GUI.
        void UpdateSelecteSceneNodeScriptPropertiesGUI();

        /// Updates the view menu on the editor GUI.
        void UpdateViewportMenuGUI();

        /// Refreshes the scene properties panel.
        void RefreshScenePropertiesPanel();


        /// Refreshes the selections.
        ///
        /// @remarks
        ///     What this does is first finds all of the nodes marked as selected. Then, it deselects everything. Finally, it cycles through the 
        ///     nodes retrieved in the first pass and selects them.
        ///     @par
        ///     This method needs to be called whenever the state of the scene has changed so that the correct objects are selected.
        void ReselectSceneNodes(unsigned int options);

        /// Recursively copies the given node and selects the copy.
        ///
        /// @param nodeToCopy [in] A reference to the scene node to copy.
        /// @param parentNode [in] A pointer to the parent node. Can be null.
        ///
        /// @return A reference to the new node.
        SceneNode & CopySceneNodeAndChildren(SceneNode &nodeToCopy, SceneNode* parentNode);


        /// Updates the icon on the "Play" button.
        void UpdatePlaybackControls();


        /// Re-links all scene nodes that are linked to the given prefab.
        ///
        /// @param prefabRelativePath [in] The relative path of the prefab.
        void RelinkSceneNodesLinkedToPrefab(const char* prefabRelativePath);

        /// Re-links all scene nodes that are linked to prefabs.
        void RelinkSceneNodesLinkedToPrefabs();


#if 0
        /// Recursively maps a scene node to a prefab.
        void MapSceneNodeToPrefab(SceneNode &sceneNode, Prefab &prefab, size_t &prefabSceneNodeIndex);

        /// Recursively creates the missing children from the prefab, but does not add them to the scene straight away.
        ///
        /// @remarks
        ///     This does not link the new nodes to the parent. Needs to be done at a higher level.
        void CreateMissingChildPrefabSceneNodes(Prefab &prefab, const SceneNode &sceneNode, uint64_t sceneNodePrefabID, GTCore::Vector<SceneNode*> &newSceneNodes);

        /// Determines whether or not a descendant scene node is linked to a different prefab.
        bool IsDescendantLinkedToDifferentPrefab(SceneNode &sceneNode) const;

        /// Updates every scene node linked to the given prefab.
        void UpdateAllSceneNodesLinkedToPrefab(const char* prefabRelativePath);

        /// Updates every scene node that is linked any prefab.
        void UpdateAllSceneNodesLinkedToPrefabs();

        /// Retrieves the relative paths of every prefab that is referenced by the scene.
        void GetAllReferencedPrefabs(GTCore::Vector<GTCore::String> &prefabRelativePaths);
#endif


        /// Updates every scene node linked to the given script.
        void UpdateAllSceneNodesLinkedToScript(const char* scriptRelativePath);


        /// Resets all particle systems.
        void ResetAllParticleSystems();


    private:

        /// The viewport that the scene will be drawn from.
        //SceneViewport viewport;

        /// The scene node acting as the camera for the viewport.
        SceneNode camera;

        /// The event handler to apply to the camera.
        struct CameraEventHandler : public SceneNodeEventHandler
        {
            /// Constructor.
            CameraEventHandler(SceneEditor &ownerIn)
                : SceneNodeEventHandler(), owner(ownerIn)
            {
            }


            /// SceneNodeEventHandler::OnTransform().
            void OnTransform(SceneNode &)
            {
                this->owner.OnCameraTransformed();
            }


            /// The scene editor that owns the camera.
            SceneEditor &owner;

        }cameraEventHandler;

        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The update manager to use with the scene.
        SceneEditorSceneUpdateManager updateManager;

        /// The update manager to use for physics.
        SceneEditorPhysicsManager physicsManager;

        /// The update manager to use for culling.
        DefaultSceneCullingManager cullingManager;

        /// The scene object for actually displaying the scene.
        Scene scene;

        /// The event handler to attach to the scene.
        SceneEditorSceneEventHandler sceneEventHandler;


        /// The list of selected nodes.
        GTCore::Vector<uint64_t> selectedNodes;

        /// The list of selected nodes before playing. This is used to reselect the nodes when playing is stopped.
        GTCore::Vector<uint64_t> selectedNodesBeforePlaying;

        /// The list of selected nodes before doing the physics simulation.
        GTCore::Vector<uint64_t> selectedNodesBeforePhysicsSimulation;


        /// The collision world for doing picking/selecting.
        CollisionWorld pickingWorld;


        /// The transformation gizmo.
        TransformGizmo transformGizmo;

        /// The axis that the gizmo is being dragged on. If the x axis is being dragged, this will be set to (1, 0, 0), etc.
        glm::vec3 gizmoDragAxis;

        /// When dragging a gizmo axis with the mouse, the x and y movement has a different level of influence as it's being dragged. We keep track of
        /// that here. This is a normalised vector.
        glm::vec2 gizmoDragFactor;

        /// The gragging mode (translate, rotate, scale)
        enum GizmoDragMode
        {
            GizmoDragMode_None,
            GizmoDragMode_Translate,
            GizmoDragMode_Rotate,
            GizmoDragMode_Scale

        }gizmoDragMode;

        /// The current transform mode of the gizmo.
        enum GizmoTransformMode
        {
            GizmoTransformMode_Translate,
            GizmoTransformMode_Rotate,
            GizmoTransformMode_Scale

        }gizmoTransformMode;

        /// The current transform space to use with the gizmo.
        enum GizmoTransformSpace
        {
            GizmoTransformSpace_Global,
            GizmoTransformSpace_Local

        }gizmoTransformSpace;


        /// The position to use for when we snap objects to the grid. The way it works that when we are snap-dragging, we move this
        /// and then have the positions of selected objects snap to the closest grid position.
        glm::vec3 snapTranslation;
        float     snapAngle;
        glm::vec3 snapScale;

        /// Keeps track of whether or not we are snapping.
        bool isSnapping;


        /// The size of a snapping segment for translations.
        float translateSnapSize;

        /// the size of a snapping segment for rotations.
        //float rotateSnapSize;

        /// The size of a snapping segment for scale.
        //float scaleSnapSize;
            

        /// Keeps track of whether or not an object was transformed with the gizmo.
        bool transformedObjectWithGizmo;


        /// Keeps track of whether or not we're deserializing. We use this to control whether or not the scene should be marked as modified and stack
        /// frames committed.
        bool isDeserializing;

        /// Keeps track of whether or not we are changing the scene via the state stack.
        bool isUpdatingFromStateStack;


        /// Keeps track of whether or not the game is playing.
        bool isPlaying;

        /// Keeps track of whether or not the game is paused.
        bool isPaused;


        /// Keeps track of whether or not the game was playing when the editor is hidden.
        bool wasPlayingBeforeHide;



        /// Structure containing the GUI elements of the editor.
        struct _GUI
        {
            _GUI()
                : Main(nullptr), Viewport(nullptr), PropertiesPanel(nullptr)
            {
            }

            GTGUI::Element* Main;
            GTGUI::Element* Viewport;
            GTGUI::Element* PropertiesPanel;

        }GUI;


        /// The viewport event handler to we can detect when it is resized.
        struct ViewportEventHandler : public Editor3DViewportEventHandler
        {
            /// Constructor.
            ViewportEventHandler(SceneEditor &ownerIn, Game &game, SceneViewport &viewport)
                : Editor3DViewportEventHandler(game, viewport), owner(ownerIn)
            {
            }


            /// Called after the element has been resized.
            void OnSize(GTGUI::Element &element)
            {
                Editor3DViewportEventHandler::OnSize(element);
                owner.OnViewportSize();
            }

            /// Called when the mouse wheel is moved.
            void OnMouseWheel(GTGUI::Element &element, int delta, int x, int y)
            {
                Editor3DViewportEventHandler::OnMouseWheel(element, delta, x, y);
                owner.OnViewportMouseWheel();
            }


            /// The owner of the viewport.
            SceneEditor &owner;

        }viewportEventHandler;



        /// The grid.
        EditorGrid grid;

        /// Keeps track of whether or not the grid is showing.
        bool isShowingGrid;

        /// Keeps track of whether or not the grid was showing before playing.
        bool wasShowingGridBeforePlaying;


        /// The axis arrows.
        EditorAxisArrows axisArrows;

        /// Keeps track of whether or not the axis arrows is showing.
        bool isShowingAxisArrows;

        /// Keeps track of whether or not the axis arrows showing before playing.
        bool wasShowingAxisArrowsBeforePlaying;


        /// The prefab linker for managing the linking of scene nodes to prefabs.
        SceneEditorPrefabLinker prefabLinker;
    };
}

#endif
