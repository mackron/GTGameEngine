// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_Renderer
#define GT_GUI_Renderer

#include <GTLib/Rect.hpp>
#include <GTLib/Colour.hpp>
#include "GUISurface.hpp"

namespace GT
{
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
        /// @param context [in] A reference to the GUI context.
        /// @param hFont   [in] A handle to the font to check.
        virtual bool CanDrawText(GT::GUIContext &context, GT::HGUIFont hFont) = 0;

        /// Draws the given text using the given font.
        ///
        /// @param context     [in] A reference to the context.
        /// @param textRunDesc [in] Information about the text to draw.
        virtual void DrawText(GT::GUIContext &context, const GUITextRunDesc &textRunDesc) = 0;


        /// Generic function for drawing a buffer containing image data.
        ///
        /// @remarks
        ///     The data is always 32-bits per pixel. Use isTransparent to determine if alpha transparency should be used.
        virtual void DrawRawImage(GT::GUIContext &context, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent) = 0;


        /// Allows the renderer to create and initialize internal objects for the given image.
        ///
        /// @remarks
        ///     A renderering backend such as OpenGL will want to create image objects in response to this function.
        ///     @par
        ///     UninitializeImage() will be called to allow the renderer to delete an image.
        ///     @par
        ///     When a shape needs to be drawn with a texture, handle to the image that was passed to this method will used specified.
        virtual void InitializeImage(GT::GUIContext &context, HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData) = 0;

        /// Allows the renderer to uninitialize and delete internal objects for the given image.
        virtual void UninitializeImage(GT::GUIContext &context, HGUIImage hImage) = 0;
    };
}

#endif
