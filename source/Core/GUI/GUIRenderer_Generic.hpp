// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_Renderer_Generic_h_
#define __GT_GUI_Renderer_Generic_h_

#include <GTGameEngine/Core/GUI/GUIRenderer.hpp>
#include <GTLib/HandleManager.hpp>

namespace GT
{
    /// Generic implementation of the GUI render buffer for use by the generic renderer.
    struct GUIRenderBuffer_Generic
    {
        /// Constructor.
        GUIRenderBuffer_Generic(HGUIRenderBuffer handleIn, unsigned int widthIn, unsigned int heightIn);

        /// Destructor.
        ~GUIRenderBuffer_Generic();



        /// The render buffer handle for validation.
        HGUIRenderBuffer handle;


        /// The width of the buffer.
        unsigned int width;

        /// The height of the buffer.
        unsigned int height;

        /// A pointer to the raw data of the buffer.
        uint32_t* data;


        /// Keeps track of whether or not the data is mapped.
        bool isDataMapped;
    };



    /// Generic implementation of the GUI renderer.
    ///
    /// This writes directly to an RGBA8 buffer.
    class GUIRenderer_Generic : public GUIRenderer
    {
    public:

        /// Constructor.
        GUIRenderer_Generic();

        /// Destructor.
        virtual ~GUIRenderer_Generic();



        /// @copydoc GUIRenderer::BeginPaintSurface()
        void BeginPaintSurface(GT::HGUISurface hSurface);

        /// @copydoc GUIRenderer::EndPaintSurface()
        void EndPaintSurface();

        /// @copydoc GUIRenderer::Clear()
        void Clear();

        /// @copydoc GUIRenderer::Clear(const GTLib::Rect<int> &)
        void Clear(const GTLib::Rect<int> &rect);

        /// @copydoc GUIRenderer::DrawRectangle()
        void DrawRectangle(GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc GUIRenderer::SetClippingRect()
        void SetClippingRect(GTLib::Rect<int> rect);


    private:

        /// Retrieves a pointer to the render buffer object associated with the given handle.
        ///
        /// @param hBuffer [in] A handle to the buffer whose associated object is being retrieved.
        ///
        /// @return A pointer to the associated object, or null if it is an invalid handle.
        GUIRenderBuffer_Generic* GetRenderBufferObject(HGUIRenderBuffer hBuffer);



    private:

        /// The handle manager for render buffers.
        GT::HandleManager<HGUIRenderBuffer, GUIRenderBuffer_Generic> m_renderBuffers;

        /// A pointer to the buffer all future rendering operations should draw to.
        GUIRenderBuffer_Generic* m_currentRenderBuffer;

        /// The current clipping rectangle.
        GTLib::Rect<unsigned int> m_currentClippingRect;


    private:    // No copying.
        GUIRenderer_Generic(const GUIRenderer_Generic &);
        GUIRenderer_Generic & operator=(const GUIRenderer_Generic&);
    };
}

#endif // !__GT_GUI_Renderer_Generic_h_
