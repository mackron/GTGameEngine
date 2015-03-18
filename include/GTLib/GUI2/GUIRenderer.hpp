// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_Renderer_hpp_
#define __GT_GUI_Renderer_hpp_

#include <GTLib/Rect.hpp>
#include <GTLib/Colour.hpp>

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


        /// Creates a render buffer of the given size.
        ///
        /// @param width  [in] The width of the render buffer.
        /// @param height [in] The height of the render buffer.
        ///
        /// @remarks
        ///     The size of the render buffer is immutable, which means the buffer must be deleted and re-created when a resize is needed. Render buffers
        ///     are always in RGBA8 format.
        virtual HGUIRenderBuffer CreateRenderBuffer(unsigned int width, unsigned int height) = 0;

        /// Deletes the given render buffer.
        ///
        /// @param hBuffer [in] A handle to the render buffer to delete.
        virtual void DeleteRenderBuffer(HGUIRenderBuffer hBuffer) = 0;

        /// Retrieves the size of the given render buffer.
        ///
        /// @param hBuffer   [in]  The render buffer whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to teh variable that will receive the height.
        ///
        /// @remarks
        ///     If the buffer is invalid, widthOut and heightOut will be set to 0.
        virtual void GetRenderBufferSize(HGUIRenderBuffer hBuffer, unsigned int &widthOut, unsigned int &heightOut) = 0;

        /// Creates a mapping to the data of the given render buffer.
        ///
        /// @param hBuffer [in] The render buffer whose data is being mapped.
        ///
        /// @remarks
        ///     The returned pointer supports both reading and writing, and the size will be at least width*height*4 in size.
        ///     @par
        ///     If the render buffer is already mapped, this will return null.
        ///     @par
        ///     The buffer must be unmapped with UnmapRenderBuffer() when the mapping is no longer needed.
        virtual void* MapRenderBuffer(HGUIRenderBuffer hBuffer) = 0;

        /// Unmaps the data of the given render buffer.
        ///
        /// @param hBuffer [in] The render buffer whose data is being unmapped.
        /// @param data    [in] The pointer that was returned by the corresponding call to MapRenderBuffer(). This is only used for validation.
        ///
        /// @remarks
        ///     The 'data' parameter is only used for validation. If this is not equal to the pointer that was returned by MapRenderBuffer(), UnmapRenderBuffer()
        ///     will return false.
        virtual bool UnmapRenderBuffer(HGUIRenderBuffer hBuffer, const void* data) = 0;


        /// Sets the render buffer that all future drawing commands will be drawn to.
        ///
        /// @param hBuffer [in] The render buffer to all future drawing commands should be drawn to.
        virtual void SetCurrentRenderBuffer(HGUIRenderBuffer hBuffer) = 0;



        ///////////////////////////////////////
        // Drawing

        /// Clears the current render buffer.
        virtual void Clear() = 0;

        /// Clears the current render  buffer, but only withing the given rectangle region.
        ///
        /// @param rect [in] The rectangle to clear.
        ///
        /// @remarks
        ///     The clipping rectangle will be set to the given rectangle.
        virtual void Clear(const GTLib::Rect<int> &rect) = 0;

        /// Draws a rectangle of the given colour.
        ///
        /// @param rect   [in] The rectangle to draw.
        /// @param colour [in] The colour of the rectangle.
        virtual void DrawRectangle(GTLib::Rect<int> rect, GTLib::Colour colour) = 0;

        /// Sets the clipping rectangle.
        ///
        /// @param rect [in] The new clipping rectangle.
        ///
        /// @remarks
        ///     Anything outside the given rectangle will be clipped.
        virtual void SetClippingRect(GTLib::Rect<int> rect) = 0;
    };
}

#endif
