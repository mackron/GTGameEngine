
#ifndef __GTEngine_Editor_SceneEditor_hpp_
#define __GTEngine_Editor_SceneEditor_hpp_

#include "../Scene.hpp"
#include "../GameScript.hpp"
#include "../Physics/CollisionWorld.hpp"
#include "../ModelLibrary.hpp"
#include "../Texture2DLibrary.hpp"
#include "Editor3DViewportEventHandler.hpp"
#include "SceneEditorSceneEventHandler.hpp"
#include "SceneEditorSceneUpdateManager.hpp"
#include "../DefaultScenePhysicsManager.hpp"
#include "../DefaultSceneCullingManager.hpp"
#include "TransformGizmo.hpp"

#include <GTGUI/Element.hpp>

namespace GTEngine
{
    class Editor;

    class Editor_SceneEditor
    {
    public:

        /// Constructor.
        Editor_SceneEditor(Editor &editor);

        /// Destructor.
        ~Editor_SceneEditor();


        /// Starts up the scene editor.
        void Startup();


        /// Retrieves the editor that owns this sub-editor.
              Editor & GetEditor()       { return this->editor; }
        const Editor & GetEditor() const { return this->editor; }


        /// Loads a new scene.
        ///
        /// @param absolutePath [in] The absolute path of the scene to load.
        ///
        /// @return True if the scene is loaded successfully; false otherwise.
        bool LoadScene(const char* absolutePath, const char* makeRelativeTo);

        /// Saves the given scene.
        ///
        /// @param absolutePath [in] The absolute path of the scene to save.
        ///
        /// @return True if the scene is saved successfully; false otherwise.
        bool SaveScene(const char* absolutePath);

        /// Closes the given scene.
        ///
        /// @param absolutePath [in] The absolute path of the scene to close.
        void CloseScene(const char* absolutePath);


        /// Resets the camera to the default position.
        void ResetCamera();

        /// Sets the rotation of the camera.
        ///
        /// @param xRotation [in] The new x rotation.
        /// @param yRotation [in] The new y rotation.
        void SetCameraRotation(float xRotation, float yRotation);


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

        /// Retrieves the orientation of the selected nodes.
        ///
        /// @remarks
        ///     If there are multiple selections, this will return the default quaternion.
        glm::quat GetSelectionRotation() const;


        ///////////////////////////////////////////////////
        // Editting

        /// Deletes the selected scene nodes.
        void DeleteSelectedSceneNodes();

        /// Duplicates the selected scene nodes, deselects them and then selects the new ones.
        void DuplicateSelectedSceneNodes();


        ///////////////////////////////////////////////////
        // Events

        /// Updates the model editor so that the viewport is rendered.
        void Update(double deltaTimeInSeconds);

        /// Called when a mouse button is pressed.
        void OnMouseButtonDown(GTCore::MouseButton button, int x, int y);

        /// Called when a mouse button is released.
        void OnMouseButtonUp(GTCore::MouseButton button, int x, int y);



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




    private:

        struct State;


        /// Applies the camera rotation to the camera node.
        void ApplyCameraRotation();


        /// Shows and repositions the positioning gizmo.
        void ShowTransformGizmo();

        /// Hides whatever gizmo is currently showing.
        void HideGizmos();

        /// Updates the position of the gizmos.
        void RepositionGizmos();

        /// Updates the scale of the gizmos so that they appear a constant size.
        void RescaleGizmos();

        /// Updates the gizmo so that it's scaled, positioned and rotated properly.
        void UpdateGizmo();

        /// Shows the appropriate handles on the gizmo for the current 
        void ShowGizmoHandles();


        /// Serializes the given scene scene.
        ///
        /// @param state      [in] A reference to the state whose scene is to be serialized.
        /// @param serializer [in] A reference to the serializer to write to.
        void SerializeScene(const State &state, GTCore::Serializer &serializer) const;

        /// Deserializes the currently loaded scene.
        ///
        /// @param state        [in] A reference to the state whose scene is to be deserialized.
        /// @param deserializer [in] A reference to the deserializer to read from.
        void DeserializeScene(State &state, GTCore::Deserializer &deserializer);


        /// Sets the current scene in the scripting environment.
        ///
        /// @param scene     [in] A pointer to the scene to make current. Can be null.
        /// @param elementID [in] The ID of the element containing the scene editor.
        ///
        /// @remarks
        ///     This function simple calls the following script function "Editor.SceneEditor.SetCurrentScript(scene);"
        void SetCurrentSceneInScript(Scene* scene, const char* elementID);

        /// A helper for retrieving the script object.
        GameScript & GetScript();


        /// Initialises the scripting interface.
        void InitialiseScripting();




    private:

        /// A reference to the Editor object that owns this sub-editor.
        Editor &editor;



        /// Structure representing the current state of a loaded scene file.
        struct State
        {
            /// Constructor.
            State(Editor_SceneEditor &sceneEditor);

            /// Destructor.
            ~State();


            /// A helper method for determining if we're dragging a gizmo.
            bool IsDraggingGizmo() const
            {
                return this->isDraggingGizmoX || this->isDraggingGizmoY || this->isDraggingGizmoZ;
            }



            /// A reference to the scene editor that owns this state.
            Editor_SceneEditor &sceneEditor;


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



            /// The list of selected nodes.
            GTCore::Vector<SceneNode*> selectedNodes;



            /// The collision world for doing picking/selecting.
            CollisionWorld pickingWorld;

            /// The transformation gizmo.
            TransformGizmo transformGizmo;


            /// A flat list of every loaded scene node. We need this so we can delete them when the state object is destructed.
            GTCore::Vector<SceneNode*> sceneNodes;



            /// Keeps track of whether or not we're dragging a gizmo.
            bool isDraggingGizmoX;
            bool isDraggingGizmoY;
            bool isDraggingGizmoZ;

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


        private:    // No copying.
            State(const State &);
            State & operator=(const State &);
        };

        /// A pointer to the State object for the currently open model.
        State* currentState;

        /// A map of State objects, mapped to the path of the appropriate model.
        GTCore::Dictionary<State*> loadedStates;



        /////////////////////////////////////////////////
        /// Scripting FFI

        struct SceneEditorFFI
        {
            /// Helper for retrieving the SceneEditor object.
            static Editor_SceneEditor & GetSceneEditor(GTCore::Script &script);


            /// Attempts to select a gizmo with the mouse.
            ///
            /// @remarks
            ///     Returns true if a gizmo was selected.
            static int TryGizmoMouseSelect(GTCore::Script &script);

            /// Performs a mouse selection.
            static int DoMouseSelection(GTCore::Script &script);

            /// Deselects everything.
            static int DeselectAll(GTCore::Script &script);

            /// Selects the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A GTEngine.SceneNode:Create() table. The '_internalPtr' property is a pointer to the C++ SceneNode object.
            static int SelectSceneNode(GTCore::Script &script);

            /// Deselects the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A GTEngine.SceneNode:Create() table. The '_internalPtr' property is a pointer to the C++ SceneNode object.
            static int DeselectSceneNode(GTCore::Script &script);


            /// Deletes all of the selected nodes.
            static int DeleteSelectedSceneNodes(GTCore::Script &script);

            /// Duplicates the selected scene nodes.
            static int DuplicateSelectedSceneNodes(GTCore::Script &script);


            /// Switches the gizmo to translate mode.
            static int SwitchGizmoToTranslateMode(GTCore::Script &script);

            /// Switches the gizmo to rotate mode.
            static int SwitchGizmoToRotateMode(GTCore::Script &script);

            /// Switches the gizmo to scale mode.
            static int SwitchGizmoToScaleMode(GTCore::Script &script);

            /// Switches the gizmo to local space.
            static int SwitchGizmoToLocalSpace(GTCore::Script &script);

            /// Switches the gizmo to global space.
            static int SwitchGizmoToGlobalSpace(GTCore::Script &script);

            /// Toggles the gizmo space.
            static int ToggleGizmoSpace(GTCore::Script &script);

            /// Determines whether or not the gizmo is in local space.
            static int IsGizmoInLocalSpace(GTCore::Script &script);

            /// Determines whether or not the gizmo is in global space.
            static int IsGizmoInGlobalSpace(GTCore::Script &script);
        };



    private:    // No copying.
        Editor_SceneEditor(const Editor_SceneEditor &);
        Editor_SceneEditor & operator=(const Editor_SceneEditor &);
    };
}

#endif
