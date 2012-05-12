
#ifndef __GTEngine_Editor_ModelEditor_hpp_
#define __GTEngine_Editor_ModelEditor_hpp_

#include "../DefaultScene.hpp"
#include "../DefaultViewportRenderer.hpp"
#include <GTGUI/Server.hpp>

namespace GTEngine
{
    class ModelEditor;

    /// Event handler for the model viewport event handler.
    class ModelViewportEventHandler : public GTGUI::ElementEventHandler
    {
    public:

        /// Constructor.
        ModelViewportEventHandler(ModelEditor &modelEditor);


    public:

        /// OnSize.
        void OnSize(GTGUI::Element &element);

        /// OnDraw.
        void OnDraw(GTGUI::Element &element);

        /// OnLMBDown.
        void OnLMBDown(GTGUI::Element &element, int x, int y);

        /// OnLMBUp.
        void OnLMBUp(GTGUI::Element &element, int x, int y);

        /// OnRMBDown.
        void OnRMBDown(GTGUI::Element &element, int x, int y);

        /// OnRMBUp.
        void OnRMBUp(GTGUI::Element &element, int x, int y);

        /// OnMouseMove
        void OnMouseMove(GTGUI::Element &element, int x, int y);

        /// OnMouseWhee
        void OnMouseWheel(GTGUI::Element &element, int delta, int x, int y);


    private:

        /// The model editor.
        ModelEditor &modelEditor;


    private:    // No copying.
        ModelViewportEventHandler(const ModelViewportEventHandler &);
        ModelViewportEventHandler & operator=(const ModelViewportEventHandler &);
    };
}

namespace GTEngine
{
    class Game;
    class Editor;

    class ModelEditor
    {
    public:

        /// Constructor.
        ModelEditor(Game &game, Editor &editor);

        /// Destructor.
        ~ModelEditor();

        /// Initialises the model editor.
        bool Startup(GTGUI::Server &guiServer);

        /// Updates the model editor. This is mainly required for drawing the scene.
        void Update(double deltaTimeInSeconds);

        /// Swaps the RC queues. This is needed in order to swap the queues for the viewport renderer.
        void SwapRCQueues();


    private:

        /// The game that owns this editor.
        Game &game;

        /// The main editor object that owns this sub editor.
        Editor &editor;


        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : ModelViewport(nullptr)
            {
            }

            /// The model viewport
            GTGUI::Element* ModelViewport;

        }GUI;


        /// Structure containing the event handlers for GUI elements.
        struct _GUIEventHandlers
        {
            _GUIEventHandlers(ModelEditor &modelEditor)
                : ModelViewport(modelEditor)
            {
            }

            /// Event handler for the model viewport.
            ModelViewportEventHandler ModelViewport;

        }GUIEventHandlers;


        


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



        // TESTING
        //
        // The map of scene nodes representing the bones of the model.
        GTCore::Dictionary<SceneNode*> boneSceneNodes;


    private:

        /// Foreign Function Interface for the editor GUI.
        struct FFI
        {
        };


    private:    // No copying.
        ModelEditor(const ModelEditor &);
        ModelEditor & operator=(const ModelEditor &);


    friend class ModelViewportEventHandler;
    };
}

#endif
