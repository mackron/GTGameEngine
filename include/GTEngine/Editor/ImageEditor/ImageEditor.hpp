
#ifndef __GTEngine_ImageEditor_hpp_
#define __GTEngine_ImageEditor_hpp_

#include "../SubEditor.hpp"
#include "../../Texture2DLibrary.hpp"

namespace GTEngine
{
    /// Class representing an image editor.
    class ImageEditor : public SubEditor
    {
    public:

        /// Constructor.
        ImageEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~ImageEditor();



        /// Retrieves a pointer to the Texture2D image.
              Texture2D* GetImage()       { return this->image; }
        const Texture2D* GetImage() const { return this->image; }


        /// Retrieves the current zoom.
        float GetZoom() const { return this->zoom; }

        /// Sets the current zoom.
        void SetZoom(float newZoom) { this->zoom = newZoom; }


        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        bool Save();


    private:

        /// The viewport the image will be drawn to.
        GTGUI::Element* viewportElement;

        /// The event handler for the viewport. We need this for the mouse wheel and drawing.
        /// Event handler for the viewport. Mainly needed for drawing the image at draw time.
        struct ViewportEventHandler : public GTGUI::ElementEventHandler
        {
            /// Constructor.
            ViewportEventHandler(ImageEditor &ownerEditorIn)
                : ownerEditor(ownerEditorIn)
            {
            }


            /// OnDraw.
            void OnDraw(GTGUI::Element &element);

            /// OnMouseWheel.
            void OnMouseWheel(GTGUI::Element &element, int delta, int x, int y);


        private:

            /// The editor that owns this viewport.
            ImageEditor &ownerEditor;


        private:    // No copying.
            ViewportEventHandler(const ViewportEventHandler &);
            ViewportEventHandler & operator=(const ViewportEventHandler &);

        }viewportEventHandler;



        /// The texture to draw.
        Texture2D* image;

        /// The zoom level.
        float zoom;
    };
}

#endif