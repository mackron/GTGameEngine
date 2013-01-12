
#ifndef __GTEngine_SceneEditor_hpp_
#define __GTEngine_SceneEditor_hpp_

#include "../SubEditor.hpp"
#include "../TransformGizmo.hpp"
#include "../Editor3DViewportEventHandler.hpp"
#include "../SceneEditorSceneEventHandler.hpp"
#include "../SceneEditorSceneUpdateManager.hpp"
#include "../../DefaultScenePhysicsManager.hpp"
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



        ///////////////////////////////////////////////////
        // Selections

        /// Attempts to select a gizmo with the mouse.
        ///
        /// @return True if a gizmo was selected.
        bool TryGizmoMouseSelect();

        /// Performs a mouse selection.
        void DoMouseSelection();

        /// Deselects everything.
        void DeselectAll();

        /// Determines whether or not the given node is selected.
        ///
        /// @param node [in] The node to check.
        bool IsSceneNodeSelected(const SceneNode &node) const;

        /// Selects the given scene node.
        ///
        /// @param node  [in] The node to select.
        /// @param force [in] Normally the scene node will not go through a full selection process if it is already marked as selected - this allows that operato to be forced, which is required sometimes.
        void SelectSceneNode(SceneNode &node, bool force = false);

        /// Deselects the given scene node.
        ///
        /// @param node [in] The node to deselect.
        void DeselectSceneNode(SceneNode &node);

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



        ///////////////////////////////////////////////////
        // Editting

        /// Deletes the selected scene nodes.
        void DeleteSelectedSceneNodes();

        /// Duplicates the selected scene nodes, deselects them and then selects the new ones.
        void DuplicateSelectedSceneNodes();



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

        /// SceneEditorSceneEventHandler::OnObjectAdded()
        void OnObjectAdded(SceneObject &object);

        /// SceneEditorSceneEventHandler::OnObjectRemoved()
        void OnObjectRemoved(SceneObject &object);

        /// SceneEditorSceneEventHandler::OnObjectRefreshed()
        void OnObjectRefreshed(SceneObject &object);

        /// SceneEditorSceneEventHandler::OnSceneNodeTransform()
        void OnSceneNodeTransform(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeScale()
        void OnSceneNodeScale(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeHide()
        void OnSceneNodeHide(SceneNode &node);

        /// SceneEditorSceneEventHandler::OnSceneNodeShow()
        void OnSceneNodeShow(SceneNode &node);




        ///////////////////////////////////////////////////
        // Virtual Methods.

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



    private:

        ///////////////////////////////////////////////////
        // Private Methods.

        /// Applies the camera rotation to the camera node.
        void ApplyCameraRotation();

        /// Serializes the scene.
        ///
        /// @param serializer [in] A reference to the serializer to write to.
        void SerializeScene(GTCore::Serializer &serializer) const;

        /// Deserializes the scene.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        void DeserializeScene(GTCore::Deserializer &deserializer);


        /// Shows and repositions the positioning gizmo.
        void ShowTransformGizmo();

        /// Hides whatever gizmo is currently showing.
        void HideGizmo();

        /// Updates the position of the gizmos.
        void RepositionGizmo();

        /// Updates the scale of the gizmos so that they appear a constant size.
        void RescaleGizmo();

        /// Updates the gizmo so that it's scaled, positioned and rotated properly.
        void UpdateGizmo();

        /// Shows the appropriate handles on the gizmo for the current 
        void ShowGizmoHandles();

        /// Determines whether or not the transform gizmo is being dragged.
        bool IsDraggingGizmo() const;


        /// Posts an OnSelectionChanged() event to the scene editor.
        void PostOnSelectionChangedEventToScript();


    private:

        /// The viewport that the scene will be drawn from.
        SceneViewport viewport;

        /// The scene node acting as the camera for the viewport.
        SceneNode camera;

        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The update manager to use with the scene.
        SceneEditorSceneUpdateManager updateManager;

        /// The update manager to use for physics.
        DefaultScenePhysicsManager physicsManager;

        /// The update manager to use for culling.
        DefaultSceneCullingManager cullingManager;

        /// The scene object for actually displaying the scene.
        Scene scene;

        /// The event handler to attach to the scene.
        SceneEditorSceneEventHandler sceneEventHandler;

        /// The event handler for the 3D viewport.
        Editor3DViewportEventHandler viewportEventHandler;


        /// A flat list of every loaded scene node. We need this so we can delete them when the state object is destructed.
        GTCore::Vector<SceneNode*> sceneNodes;

        /// The list of selected nodes.
        GTCore::Vector<SceneNode*> selectedNodes;


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
        float rotateSnapSize;

        /// The size of a snapping segment for scale.
        float scaleSnapSize;
            

        /// Keeps track of whether or not an object was transformed with the gizmo.
        bool transformedObjectWithGizmo;


        /// Structure containing the GUI elements of the editor.
        struct _GUI
        {
            _GUI()
                : Main(nullptr), Viewport(nullptr), Panel(nullptr)
            {
            }

            GTGUI::Element* Main;
            GTGUI::Element* Viewport;
            GTGUI::Element* Panel;

        }GUI;
    };
}

#endif