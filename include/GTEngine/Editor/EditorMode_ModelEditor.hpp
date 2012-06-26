
#ifndef __GTEngine_Editor_ModelEditor_hpp_
#define __GTEngine_Editor_ModelEditor_hpp_

#include "EditorMode.hpp"
#include "Editor3DViewportEventHandler.hpp"
#include "../DefaultScene.hpp"
#include "../DefaultViewportRenderer.hpp"
#include <GTGUI/Server.hpp>

namespace GTEngine
{
    class Editor;

    class EditorMode_ModelEditor : public EditorMode
    {
    public:

        /// Constructor.
        EditorMode_ModelEditor(Editor &editor);

        /// Destructor.
        ~EditorMode_ModelEditor();


        /// Initialises the model editor.
        bool Startup(GTGUI::Server &guiServer);

        /// Loads a model.
        ///
        /// @param fileName [in] The file name of the model to load.
        ///
        /// @return True if the model is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     Meshes will have a default material applied by default.
        bool LoadModel(const char* fileName);

        /// Saves the current model as a .gtmodel.
        ///
        /// @param fileName [in] the file name of the model.
        ///
        /// @return True if the model is saved successfully; false otherwise.
        ///
        /// @remarks
        ///     This will overwrite any existing file without warning.
        bool SaveModel(const char* fileName);


        /// Sets the material of the model.
        ///
        /// @param index    [in] The index of the material to set.
        /// @param fileName [in] The file name of the new material.
        ///
        /// @return True if the material is set correctly; false otherwise.
        bool SetMaterial(int index, const char* fileName);


        /// Plays the entire animation track of the model.
        void PlayAnimation();

        /// Pauses the animation.
        void StopAnimation();



        /// Resets the camera to the default position.
        void ResetCamera();

        /// Sets the rotation of the camera.
        void SetCameraRotation(float xRotation, float yRotation);


        //////////////////////////////////////////////////////////////
        // Events

        /// EditorMode::OnActivate().
        void OnActivate();

        /// EditorMode::OnDeactivate().
        void OnDeactivate();

        /// EditorMode::OnUpdate().
        void OnUpdate(double deltaTimeInSeconds);

        /// EditorMode::OnSwapRCQueues().
        void OnSwapRCQueues();


    private:

        /// Applies the camera rotation.
        void ApplyCameraRotation();


    private:

        /// The main editor object that owns this sub editor.
        Editor &editor;


        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : Main(nullptr), MenuBar(nullptr), ModelViewport(nullptr)
            {
            }

            /// The main container element.
            GTGUI::Element* Main;

            /// The menu bar for the model editor.
            GTGUI::Element* MenuBar;

            /// The model viewport
            GTGUI::Element* ModelViewport;

        }GUI;


        /// The scene that will contain the model node.
        DefaultScene scene;

        /// The viewport we will be rendering from.
        SceneViewport viewport;

        /// The renderer we'll be using to draw the scene/model.
        DefaultViewportRenderer renderer;

        /// The camera node where rendering will be drawn from.
        SceneNode cameraNode;

        /// The model node containing the model that is being drawn.
        SceneNode modelNode;


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The event handler for the viewport.
        Editor3DViewportEventHandler viewportEventHandler;


    private:    // No copying.
        EditorMode_ModelEditor(const EditorMode_ModelEditor &);
        EditorMode_ModelEditor & operator=(const EditorMode_ModelEditor &);
    };
}

#endif
