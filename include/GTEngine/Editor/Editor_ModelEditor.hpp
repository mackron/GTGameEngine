
#ifndef __GTEngine_Editor_ModelEditor_hpp_
#define __GTEngine_Editor_ModelEditor_hpp_

#include "../Scene.hpp"
#include "../ConvexHullBuildSettings.hpp"
#include "Editor3DViewportEventHandler.hpp"
#include <GTGUI/Element.hpp>

namespace GTEngine
{
    class Editor;

    class Editor_ModelEditor
    {
    public:

        /// Constructor.
        Editor_ModelEditor(Editor &editor);

        /// Destructor.
        ~Editor_ModelEditor();


        /// Starts up the model editor.
        void Startup();

        /// Loads a new model.
        ///
        /// @param fileName [in] The file name of the model to load.
        ///
        /// @return True if the model is loaded successfully; false otherwise.
        bool LoadModel(const char* fileName);


        /// Sets the materials of the current model.
        ///
        /// @param index    [in] The index of the material being changed.
        /// @param fileName [in] The file name of the material to load.
        ///
        /// @return True if the material is set successfully; false otherwise.
        ///
        /// @remarks
        ///     This does not update the material of every loaded mesh of the same definition. This will be done once the model is saved and the editor is closed or reopened.
        bool SetMaterial(int index, const char* fileName);


        /// Resets the camera to the default position.
        void ResetCamera();

        /// Sets the rotation of the camera.
        void SetCameraRotation(float xRotation, float yRotation);

        
        ///////////////////////////////////////////////////
        // Events.

        /// Updates the model editor so that the viewport is rendered.
        void Update(double deltaTimeInSeconds);


    private:

        /// Applies the camera rotation to the camera node.
        void ApplyCameraRotation();

        /// Saves the current state to the 'currentState' object.
        void SaveState();

        /// Restores the state defined by 'currentState'.
        void RestoreState();

        /// Retrieves a pointer to the currently loaded model.
        Model* GetCurrentModel();


    private:

        /// The editor that owns this sub-editor.
        Editor &editor;


        /// The scene containing the model.
        Scene scene;

        /// The viewport that the scene will be drawn from.
        SceneViewport viewport;

        /// The scene node acting as the camera for the viewport.
        SceneNode camera;

        /// The renderer to use with the viewport.
        DefaultViewportRenderer renderer;


        /// The scene node containing the model.
        SceneNode modelNode;



        /// Structure containing the relevant GUI elements.
        struct _GUI
        {
            _GUI()
                : Viewport(nullptr)
            {
            }

            /// The main viewport.
            GTGUI::Element* Viewport;

        }GUI;


        /// The event handler for the 3D viewport.
        Editor3DViewportEventHandler viewportEventHandler;


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// Structure containing the editor state for an individual model. State is saved and restored as the current item changes, giving
        /// the illusion that each tab has it's own local copy of every GUI element, even though it doesn't really.
        struct State
        {
            State()
                : cameraPosition(), cameraOrientation(),
                  cameraXRotation(0.0f), cameraYRotation(0.0f),
                  showConvexDecomposition(false), convexDecompositionSettings(),
                  materials()
            {
            }

            glm::vec3 cameraPosition;
            glm::quat cameraOrientation;

            float cameraXRotation;
            float cameraYRotation;

            bool showConvexDecomposition;

            ConvexHullBuildSettings convexDecompositionSettings;

            GTCore::Vector<GTCore::String> materials;
        };

        /// A pointer to the State object for the currently open model.
        State* currentState;

        /// A map of State objects, mapped to the path of the appropriate model.
        GTCore::Dictionary<State*> loadedStates;
    };
}

#endif
