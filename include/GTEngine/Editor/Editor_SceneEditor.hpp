
#ifndef __GTEngine_Editor_SceneEditor_hpp_
#define __GTEngine_Editor_SceneEditor_hpp_

#include "../Scene.hpp"
#include "../GameScript.hpp"
#include "../Physics/CollisionWorld.hpp"
#include "Editor3DViewportEventHandler.hpp"
#include "SceneEditorSceneEventHandler.hpp"
#include "PositionGizmo.hpp"
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
        bool LoadScene(const char* absolutePath);

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



        ///////////////////////////////////////////////////
        // Selections

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
        /// @param node [in] The node to select.
        void SelectSceneNode(SceneNode &node);

        /// Deselects the given scene node.
        ///
        /// @param node [in] The node to deselect.
        void DeselectSceneNode(SceneNode &node);


        ///////////////////////////////////////////////////
        // Editting

        /// Deletes the selected scene nodes.
        void DeleteSelectedSceneNodes();


        ///////////////////////////////////////////////////
        // Events

        /// Updates the model editor so that the viewport is rendered.
        void Update(double deltaTimeInSeconds);



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

        /// SceneEditorSceneEventHandler::OnSceneNodeScale().
        void OnSceneNodeScale(SceneNode &node);




    private:

        /// Applies the camera rotation to the camera node.
        void ApplyCameraRotation();

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



            /// A reference to the scene editor that owns this state.
            Editor_SceneEditor &sceneEditor;


            /// The scene object for actually displaying the scene.
            Scene scene;

            /// The event handler to attach to the scene.
            SceneEditorSceneEventHandler sceneEventHandler;


            /// The viewport that the scene will be drawn from.
            SceneViewport viewport;

            /// The scene node acting as the camera for the viewport.
            SceneNode camera;


            /// The event handler for the 3D viewport.
            Editor3DViewportEventHandler viewportEventHandler;

            float cameraXRotation;      ///< The camera's current X rotation.
            float cameraYRotation;      ///< The camera's current Y rotation.


            /// The list of selected nodes.
            GTCore::Vector<SceneNode*> selectedNodes;


            /// The collision world for doing picking/selecting.
            CollisionWorld pickingWorld;


            /// The positioning gizmo.
            PositionGizmo positionGizmo;


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
        };



    private:    // No copying.
        Editor_SceneEditor(const Editor_SceneEditor &);
        Editor_SceneEditor & operator=(const Editor_SceneEditor &);
    };
}

#endif
