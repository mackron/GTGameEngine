
#ifndef __GTEngine_Editor_SceneEditor_hpp_
#define __GTEngine_Editor_SceneEditor_hpp_

#include "../Scene.hpp"
#include "Editor3DViewportEventHandler.hpp"
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
        // Events.

        /// Updates the model editor so that the viewport is rendered.
        void Update(double deltaTimeInSeconds);



    private:

        /// Applies the camera rotation to the camera node.
        void ApplyCameraRotation();



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

            /// The viewport that the scene will be drawn from.
            SceneViewport viewport;

            /// The scene node acting as the camera for the viewport.
            SceneNode camera;


            /// The event handler for the 3D viewport.
            Editor3DViewportEventHandler viewportEventHandler;

            float cameraXRotation;      ///< The camera's current X rotation.
            float cameraYRotation;      ///< The camera's current Y rotation.


            struct _GUI
            {
                _GUI()
                    : Main(nullptr), Viewport(nullptr)
                {
                }

                GTGUI::Element* Main;
                GTGUI::Element* Viewport;

            }GUI;


        private:    // No copying.
            State(const State &);
            State & operator=(const State &);
        };

        /// A pointer to the State object for the currently open model.
        State* currentState;

        /// A map of State objects, mapped to the path of the appropriate model.
        GTCore::Dictionary<State*> loadedStates;



    private:    // No copying.
        Editor_SceneEditor(const Editor_SceneEditor &);
        Editor_SceneEditor & operator=(const Editor_SceneEditor &);
    };
}

#endif
