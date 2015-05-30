// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_Renderer_hpp_
#define __GT_GUI_Renderer_hpp_

#include <GTLib/Rect.hpp>
#include <GTLib/Colour.hpp>
#include "GUISurface.hpp"

namespace GT
{
    /// Handle for GUI render buffers.
    typedef uint32_t HGUIRenderBuffer;


    /// Base class for doing GUI rendering.
    class GUIRenderer
    {
    public:

        /// Constructor.
        GUIRenderer();

        /// Destructor.
        virtual ~GUIRenderer();


        /// Begins rendering the given surface.
        ///
        /// @param hSurface [in] A handle to the surface to start rendering.
        virtual void BeginPaintSurface(GT::GUIContext &context, GT::HGUISurface hSurface, void* pInputData) = 0;

        /// Ends rendering of the current surface.
        virtual void EndPaintSurface(GT::GUIContext &context) = 0;


        /// Clears the current surface to black.
        virtual void Clear(GT::GUIContext &context) = 0;

        /// Clears the current render surface, but only withing the given rectangle region.
        ///
        /// @param rect [in] The rectangle to clear.
        ///
        /// @remarks
        ///     The clipping rectangle will be set to the given rectangle.
        virtual void Clear(GT::GUIContext &context, const GTLib::Rect<int> &rect) = 0;

        /// Draws a rectangle of the given colour.
        ///
        /// @param rect   [in] The rectangle to draw.
        /// @param colour [in] The colour of the rectangle.
        virtual void DrawRectangle(GT::GUIContext &context, GTLib::Rect<int> rect, GTLib::Colour colour) = 0;

        /// Sets the clipping rectangle.
        ///
        /// @param rect [in] The new clipping rectangle.
        ///
        /// @remarks
        ///     Anything outside the given rectangle will be clipped.
        virtual void SetClippingRect(GT::GUIContext &context, GTLib::Rect<int> rect) = 0;


        /// Determines whether or not the renderer can natively render text using the given font.
        ///
        /// @param hFont [in] A handle to the font to check.
        //virtual bool CanDrawText(GT::HGUIFont hFont) = 0;

        /// Draws the given text using the given font.
        ///
        /// @param hFont   [in] A handle to the font to use when drawing the text.
        /// @param text    [in] The text run to draw.
        /// @param options [in] The settings to use when drawing the text.
        //virtual void DrawText(GT::HGUIFont hFont, const char* text, const GT::GUITextRenderingOptions &options) = 0;
    };
}

#endif
