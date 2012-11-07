
#ifndef __GTEngine_Editor_ImageEditor_hpp_
#define __GTEngine_Editor_ImageEditor_hpp_

#include "../Texture2DLibrary.hpp"
#include <GTGUI/Element.hpp>

namespace GTEngine
{
    class Editor;

    /// Class representing the image editor in the main editor.
    class Editor_ImageEditor
    {
    public:

        /// Constructor.
        Editor_ImageEditor(Editor &editor);

        /// Destructor.
        ~Editor_ImageEditor();


        /// Starts up the image editor.
        void Startup();


        /// Loads the given image.
        ///
        /// @param fileName [in] The name of the image file to load.
        bool LoadImage(const char* fileName);


        /// Retrieves a pointer to the currently loaded image, as a Texture2D.
        Texture2D* GetCurrentImage() { return this->currentImage; }


        /// Retrieves the current zoom.
        float GetZoom() const;

        /// Sets the zoom.
        void SetZoom(float newZoom);


        /// Retrieves a reference to the game.
              Game & GetGame();
        const Game & GetGame() const;



    private:

        /// The main editor that owns this sub-editor.
        Editor &editor;

        /// A pointer to the texture currently being displayed.
        Texture2D* currentImage;


        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : Viewport(nullptr)
            {
            }

            /// The main viewport that will contain the image.
            GTGUI::Element* Viewport;

        }GUI;


        /// Event handler for the viewport. Mainly needed for drawing the image at draw time.
        class ViewportEventHandler : public GTGUI::ElementEventHandler
        {
        public:

            /// Constructor.
            ViewportEventHandler(Editor_ImageEditor &editor)
                : editor(editor)
            {
            }


            /// OnDraw.
            void OnDraw(GTGUI::Element &element);

            /// OnMouseWheel.
            void OnMouseWheel(GTGUI::Element &element, int delta, int x, int y);


        private:

            /// The editor that owns this viewport.
            Editor_ImageEditor &editor;


        private:    // No copying.
            ViewportEventHandler(const ViewportEventHandler &);
            ViewportEventHandler & operator=(const ViewportEventHandler &);

        }viewportEventHandler;



        /// Structure containing the editor state for each loaded image.
        struct State
        {
            State()
                : zoom(1.0f)
            {
            }

            float zoom;
        };


        /// A pointer to the State object for the currently open model.
        State* currentState;

        /// A map of State objects, mapped to the path of the appropriate model.
        GTCore::Dictionary<State*> loadedStates;


    private:    // No copying.
        Editor_ImageEditor(const Editor_ImageEditor &);
        Editor_ImageEditor & operator=(const Editor_ImageEditor &);
    };
}

#endif
