// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_OpenGL21_hpp_
#define __GT_GE_GPURenderingDevice_OpenGL21_hpp_

#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTGameEngine/GPURenderingDevice_Gen1.hpp>
#include <GTLib/Map.hpp>

namespace GT
{
    namespace GE
    {
        /// OpenGL 2.1 implementation of the rendering device.
        class GPURenderingDevice_OpenGL21 : public GPURenderingDevice_Gen1
        {
        public:

            /// Constructor.
            GPURenderingDevice_OpenGL21(const GPURenderingDeviceInfo &info);

            /// Destructor.
            virtual ~GPURenderingDevice_OpenGL21();


            /// GPURenderingDevice::Startup().
            ResultCode Startup();

            /// GPURenderingDevice::Shutdown().
            void Shutdown();


            /// GPURenderingDevice::GetRenderingAPI().
            RenderingAPI GetRenderingAPI() const;



            /// GPURenderingDevice::SetSwapInterval().
            void SetSwapInterval(int swapInterval);


            /// GPURenderingDevice::ClearColor().
            void ClearColor(float r, float g, float b, float a);



            ///////////////////////////////////////////
            // Framebuffers

            /// GPURenderingDevice::CreateFramebuffer().
            ResultCode CreateFramebuffer(Framebuffer* &framebuffer);



            //////////////////////////////////////////
            // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
            /// GPURenderingDevice::SwapBuffers().
            void SwapBuffers();

            /// GPURenderingDevice::InitWindowFramebuffer().
            ResultCode InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil);

            /// GPURenderingDevice::UninitWindowFramebuffer().
            void UninitWindowFramebuffer(HWND hWnd);

            /// GPURenderingDevice::SetCurrentWindow().
            ResultCode SetCurrentWindow(HWND hWnd);
#endif

#if defined(GT_PLATFORM_LINUX)
#endif


        private:



        private:

            /// The OpenGL context object.
            OpenGLContext m_gl;


#if defined(GT_PLATFORM_WINDOWS)
            /// Structure containing the elements that make up the framebuffer for a given window.
            struct HWNDFramebuffer
            {
                /// A pointer to the swap-chain object for the framebuffer.
                HDC m_hDC;
            };

            /// A list of HWNDs mapped to their framebuffer object.
            GTLib::Map<HWND, HWNDFramebuffer> m_windowFramebuffers;


            /// A handle to the current window where rendering operations will be output to.
            HWND m_currentHWND;

            /// A handle to the DC of the current window.
            HDC m_currentDC;
#endif


            ////////////////////////////////////////////////////
            // State

            /// Boolean state flags.
            uint32_t m_stateFlags;


            
            ///////////////////////////////////////////
            // State Flags

            static const uint32_t StageFlag_IsWindowFramebufferCurrent = (1 << 0);     //< Is the current window's framebuffer the current render target?

        public:

            //////////////////////////////////////////////////////
            // Error Codes

            // Win32 Errors
#if defined(GT_PLATFORM_WINDOWS)
#endif

            // Linux/X11 Errors
#if defined(GT_PLATFORM_LINUX)
#endif


        private:    // No copying.
            GPURenderingDevice_OpenGL21(const GPURenderingDevice_OpenGL21 &);
            GPURenderingDevice_OpenGL21 & operator=(const GPURenderingDevice_OpenGL21 &);
        };
    }
}

#endif

#endif
