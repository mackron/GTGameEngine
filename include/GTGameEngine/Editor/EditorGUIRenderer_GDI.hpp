// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorGUIRenderer_GDI_hpp_
#define __GT_EditorGUIRenderer_GDI_hpp_

#include "../Config.hpp"

#if defined(GT_BUILD_EDITOR) && defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GTGameEngine/GUI/GUIRenderer.hpp>
#include <GTGameEngine/Editor/EditorGUISurfaceAUXData.hpp>

namespace GT
{
    /// GDI implementation of the GUI renderer for the editor.
    ///
    /// It is assumed that the GUI context's font manager is the default GDI font manager. This means text will be drawn directly
    /// using the GDI API.
    ///
    /// It is also assumed that all painting commands will be executed in response to the WM_PAINT message. BeginPaintSurface()
    /// will call Win32's BeginPaint() function and EndPaintSurface() will call Win32's EndPaint() function.
    class EditorGUIRenderer_GDI : public GUIRenderer
    {
    public:

        /// Constructor.
        EditorGUIRenderer_GDI();

        /// Destructor.
        ~EditorGUIRenderer_GDI();


        /// @copydoc GUIRenderer::BeginPaintSurface()
        void BeginPaintSurface(GUIContext &context, HGUISurface hSurface, void* pInputData);

        /// @copydoc GUIRenderer::EndPaintSurface()
        void EndPaintSurface(GUIContext &context);


        /// @copydoc GUIRenderer::Clear()
        void Clear(GUIContext &context);

        /// @copydoc GUIRenderer::Clear(GUIContext &, const GTLib::Rect<int> &)
        void Clear(GUIContext &context, const GTLib::Rect<int> &rect);

        /// @copydoc GUIRenderer::DrawRectangle()
        void DrawRectangle(GUIContext &context, GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc GUIRenderer::SetClippingRect()
        void SetClippingRect(GUIContext &context, GTLib::Rect<int> rect);


        /// @copydoc GUIRenderer::CanDrawText()
        bool CanDrawText(GUIContext &context, HGUIFont hFont);

        /// @copydoc GUIRenderer::DrawText()
        void DrawText(GUIContext &context, const GUITextRunDesc &textRunDesc);


        /// @copydoc GUIRenderer::DrawRawImage()
        void DrawRawImage(GT::GUIContext &context, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent);



    private:

        /// The PAINTSTRUCT that was created from Win32's BeginPaint() function which is called from BeginPaintSurface().
        PAINTSTRUCT m_ps;

        /// The surface that is currently being painted.
        HWND m_hCurrentWindow;

        /// The DC to draw to.
        HDC m_hDC;

        /// A pointer to the AUX data for the current surface.
        EditorGUISurfaceAUXData* m_pCurrentSurfaceAUXData;
    };
}

#endif

#endif
