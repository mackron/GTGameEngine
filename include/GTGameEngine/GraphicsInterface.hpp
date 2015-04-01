// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsInterface_hpp_
#define __GT_GraphicsInterface_hpp_

#include "Config.hpp"
#include "GPURenderingDeviceInfo.hpp"
#include "Rendering/RenderingTypes.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    /// Base class for doing graphics operations.
    class GraphicsInterface
    {
    public:

        /// Constructor.
		GraphicsInterface(GraphicsInterfaceType type);

        /// Destructor.
        virtual ~GraphicsInterface();


		/// Retrieves the class of the graphics type.
		virtual GraphicsInterfaceClass GetClass() const;

		/// Retrieves the type of the graphics interface.
		GraphicsInterfaceType GetType() const;

		


        /// Starts up the graphics interface.
        ///
        /// @return The result code specifying whether or not starting up was successful.
        virtual ResultCode Startup() = 0;

        /// Shuts down the graphics interface.
        virtual void Shutdown() = 0;
    


        //////////////////////////////////////////
        // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
        /// Win32: Creates a framebuffer for the given window so that it can be drawn to.
        ///
        /// @param hWnd                [in] A handle to the window whose framebuffer is being initialized.
        /// @param includeDepthStencil [in] Whether or not a depth/stencil buffer should also be created.
        virtual ResultCode InitWindowBuffers(HWND hWnd, bool includeDepthStencil) = 0;

        /// Win32: Uninitialises the framebuffer of the given window.
        ///
        /// @param hWnd [in] A handle to the window whose framebuffer is being unitialized.
        virtual void UninitWindowBuffers(HWND hWnd) = 0;

        /// Win32: Updates the size of the give window's framebuffer.
        ///
        /// @param hWnd [in] A handle to the window whose framebuffer needs to be resized.
        ///
        /// @remarks
        ///     This should be called in response to the window's size changing.
        virtual void ResizeWindowBuffers(HWND hWnd) = 0;

        /// Win32: Swaps the buffers for the given window.
        ///
        /// @param hWnd     [in] The window whose buffers are being swapped.
        /// @param interval [in] The swap interval which controls v-sync.
        virtual void SwapWindowBuffers(HWND hWnd, int interval) = 0;
#endif



	private:

		/// The graphics interface type.
		GraphicsInterfaceType m_type;
    };
}

#endif // !__GT_GPUGraphicsInterface_hpp_
