
#ifndef __GTEngine_Editor_Sandbox_hpp_
#define __GTEngine_Editor_Sandbox_hpp_

#include "EditorMode.hpp"
#include "Editor3DViewportEventHandler.hpp"
#include "../DefaultScene.hpp"
#include "../DefaultViewportRenderer.hpp"
#include <GTGUI/Server.hpp>

namespace GTEngine
{
    class Editor;

    class EditorMode_Sandbox : public EditorMode
    {
    public:

        /// Constructor.
        EditorMode_Sandbox(Editor &editor);

        /// Destructor.
        ~EditorMode_Sandbox();


        /// Initialises the model editor.
        bool Startup(GTGUI::Server &guiServer);


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

        /// The main editor object that owns this sub editor.
        Editor &editor;


        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : Main(nullptr), MenuBar(nullptr), Viewport(nullptr)
            {
            }

            /// The main container element.
            GTGUI::Element* Main;

            /// The menu bar for the model editor.
            GTGUI::Element* MenuBar;

            /// The model viewport
            GTGUI::Element* Viewport;

        }GUI;


        /// The scene that will contain the model node.
        DefaultScene scene;

        /// The viewport we will be rendering from.
        SceneViewport viewport;

        /// The renderer we'll be using to draw the scene/model.
        DefaultViewportRenderer renderer;

        /// The camera node where rendering will be drawn from.
        SceneNode cameraNode;


        float cameraXRotation;      ///< The camera's current X rotation.
        float cameraYRotation;      ///< The camera's current Y rotation.


        /// The event handler for the viewport.
        Editor3DViewportEventHandler viewportEventHandler;


    private:    // No copying.
        EditorMode_Sandbox(const EditorMode_Sandbox &);
        EditorMode_Sandbox & operator=(const EditorMode_Sandbox &);
    };
}

#endif
