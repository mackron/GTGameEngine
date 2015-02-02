// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_hpp_
#define __GT_GE_GPURenderingDevice_hpp_

#include "Config.hpp"
#include "GPURenderingDeviceInfo.hpp"

#include <GTLib/ResultCodes.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

namespace GT
{
    namespace GE
    {
        class Framebuffer;


        /// Class representing a rendering GPU device.
        class GPURenderingDevice
        {
        public:

            /// Constructor.
            GPURenderingDevice(const GPURenderingDeviceInfo &info);

            /// Destructor.
            virtual ~GPURenderingDevice();


            /// Retrieves the device information.
            ///
            /// @param infoOut [out] A reference to the object that will receive the info.
            void GetInfo(GPURenderingDeviceInfo &infoOut) const;



            /// Initializes the rendering device.
            ///
            /// @return <0 if an error occurs, >=0 if successful.
            virtual ResultCode Startup() = 0;

            /// Uninitializes the rendering device.
            virtual void Shutdown() = 0;


            /// Retrieves the generation number for this renderer.
            ///
            /// @return An integer representing the generation of rendering technologies this rendering device supports. See remarks.
            ///
            /// @remarks
            ///     This is used in determining whether or not this device can be cased to GPURenderingDevice_Gen1 or GPURenderingDevice_Gen2. 
            ///     @par
            ///     If this returns 1, the rendering device supports Direct3D 9 / OpenGL 2.1 class of functionality.
            ///     @par
            ///     If this returns 2, the rendering device supports Direct3D 11 / OpenGL 4.5 class of functionality.
            virtual unsigned int GetGeneration() const = 0;



            /// Sets the swap interval.
            ///
            /// @param swapInterval [in] The new swap interval. See remarks.
            ///
            /// @remarks
            ///     A value of 0 will disable v-sync. A value >0 will enable v-sync.
            virtual void SetSwapInterval(int swapInterval) = 0;


            /// Clears the current framebuffer with the given color.
            ///
            /// @param r [in] The red component.
            /// @param g [in] The green component.
            /// @param b [in] The blue component.
            /// @param a [in] The alpha component.
            virtual void ClearColor(float r, float g, float b, float a) = 0;



            ///////////////////////////////////////////
            // Framebuffers

            /// Creates a framebuffer object.
            ///
            /// @param framebufferOut [in] A reference to the variable that will receive a pointer to the new framebuffer, if successful.
            virtual ResultCode CreateFramebuffer(Framebuffer* &framebuffer) = 0;



            //////////////////////////////////////////
            // Platform-Specific Methods

            /// Swaps the buffers for the current window.
            virtual void SwapBuffers() = 0;

#if defined(GT_PLATFORM_WINDOWS)
            /// Win32: Creates a framebuffer for the given window so that it can be drawn to.
            ///
            /// @param hWnd                [in] A handle to the window whose framebuffer is being initialized.
            /// @param includeDepthStencil [in] Whether or not a depth/stencil buffer should also be created.
            virtual ResultCode InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil) = 0;

            /// Win32: Uninitialises the framebuffer of the given window.
            ///
            /// @param hWnd [in] A handle to the window whose framebuffer is being unitialized.
            virtual void UninitWindowFramebuffer(HWND hWnd) = 0;

            /// Win32: Sets the current window where rendering will be output to.
            ///
            /// @param hWnd [in] A handle to the window to make current.
            ///
            /// @return <0 if there was an error making the window current. >=0 if the window was set successfully.
            ///
            /// @remarks
            ///     Removing the current binding by pass NULL.
            virtual ResultCode SetCurrentWindow(HWND hWnd) = 0;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif



        protected:

            /// The device info structure.
            GPURenderingDeviceInfo m_info;


        private:    // No copying.
            GPURenderingDevice(const GPURenderingDevice &);
            GPURenderingDevice & operator=(const GPURenderingDevice &);
        };
    }
}

#endif