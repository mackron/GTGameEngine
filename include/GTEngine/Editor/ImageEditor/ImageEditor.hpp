// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ImageEditor
#define GT_ImageEditor

#include "../SubEditor.hpp"
#include "../../Texture2DLibrary.hpp"
#include "../../Rendering/VertexArray.hpp"

namespace GT
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
              Texture2D* GetImage()       { return m_image; }
        const Texture2D* GetImage() const { return m_image; }


        /// Retrieves the current zoom.
        float GetZoom() const { return m_zoom; }

        /// Sets the current zoom.
        void SetZoom(float newZoom) { m_zoom = newZoom; }


        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::GetMainElement()
              GUIElement* GetMainElement()        { return m_viewportElement; }
        const GUIElement* GetMainElement( ) const { return m_viewportElement; }

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        bool Save();


    private:

        /// The viewport the image will be drawn to.
        GUIElement* m_viewportElement;

        /// The event handler for the viewport. We need this for the mouse wheel and drawing.
        /// Event handler for the viewport. Mainly needed for drawing the image at draw time.
        struct ViewportEventHandler : public GUIElementEventHandler
        {
            /// Constructor.
            ViewportEventHandler(ImageEditor &ownerEditorIn)
                : ownerEditor(ownerEditorIn), vertexArray(nullptr)
            {
            }


            /// GUIElementEventHandler::OnDraw()
            void OnDraw(GUIElement &element);

            /// GUIElementEventHandler::ImplementsOnDraw()
            bool ImplementsOnDraw() const { return true; }

            /// GUIElementEventHandler::OnMouseWheel()
            void OnMouseWheel(GUIElement &element, int delta, int x, int y);


        private:

            /// The editor that owns this viewport.
            ImageEditor &ownerEditor;

            /// The vertex array for drawing the quad.
            VertexArray* vertexArray;


        private:    // No copying.
            ViewportEventHandler(const ViewportEventHandler &);
            ViewportEventHandler & operator=(const ViewportEventHandler &);

        friend class ImageEditor;
        } m_viewportEventHandler;



        /// The texture to draw.
        Texture2D* m_image;

        /// The zoom level.
        float m_zoom;


    private:    // No copying.
        ImageEditor(const ImageEditor &);
        ImageEditor & operator=(const ImageEditor &);
    };
}

#endif
