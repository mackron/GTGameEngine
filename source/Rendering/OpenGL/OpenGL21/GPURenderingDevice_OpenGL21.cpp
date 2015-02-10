// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

namespace GT
{
    namespace GE
    {
        GPURenderingDevice_OpenGL21::GPURenderingDevice_OpenGL21(const GPURenderingDeviceInfo &info)
            : GPURenderingDevice_Gen1(info),
              m_gl(),
#if defined(GT_PLATFORM_WINDOWS)
              m_currentHWND(NULL),
              m_currentDC(NULL),
#endif
              m_stateFlags(0)
        {
            m_stateFlags |= StageFlag_IsWindowFramebufferCurrent;       // TODO: Remove this from the constructor once we get the framebuffer system working properly.
        }

        GPURenderingDevice_OpenGL21::~GPURenderingDevice_OpenGL21()
        {
        }


        ResultCode GPURenderingDevice_OpenGL21::Startup()
        {
            if (m_info.identifier_OpenGL == 1 && IsRenderingAPISupported(m_info, RenderingAPI_OpenGL21))
            {
                return m_gl.Startup();
            }
            else
            {
                // The device is not the primary adapter or OpenGL 2.1 is not supported.
                return RenderingAPINotSupported;
            }
        }

        void GPURenderingDevice_OpenGL21::Shutdown()
        {
            m_gl.Shutdown();
        }


        RenderingAPI GPURenderingDevice_OpenGL21::GetRenderingAPI() const
        {
            return RenderingAPI_OpenGL21;
        }


        void GPURenderingDevice_OpenGL21::SetSwapInterval(int swapInterval)
        {
            (void)swapInterval;
        }


        void GPURenderingDevice_OpenGL21::ClearColor(float r, float g, float b, float a)
        {
            m_gl.ClearColor(r, g, b, a);
            m_gl.Clear(GL_COLOR_BUFFER_BIT);
        }



        ///////////////////////////////////////////
        // Framebuffers

        ResultCode GPURenderingDevice_OpenGL21::CreateFramebuffer(Framebuffer* &framebufferOut)
        {
            framebufferOut = nullptr;
            return -1;
        }


        //////////////////////////////////////////
        // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
        void GPURenderingDevice_OpenGL21::SwapBuffers()
        {
            if (m_currentDC != NULL)
            {
                ::SwapBuffers(m_currentDC);
            }
        }

        ResultCode GPURenderingDevice_OpenGL21::InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil)
        {
            (void)includeDepthStencil;      // <-- This is always included with OpenGL contexts because all windows must use the same pixel format.


            if (!m_windowFramebuffers.Exists(hWnd))
            {
                HDC hDC = GetDC(hWnd);

                if (SetPixelFormat(hDC, m_gl.GetPixelFormat(), reinterpret_cast<const PIXELFORMATDESCRIPTOR*>(&m_gl.GetPFD())))
                {
                    HWNDFramebuffer framebuffer;
                    framebuffer.m_hDC = hDC;

                    m_windowFramebuffers.Add(hWnd, framebuffer);

                    return 0;   // No error.
                }
                else
                {
                    // Failed to set pixel format.
                    return OpenGLContext::FailedToSetPixelFormat;
                }
            }
            else
            {
                // Window is already registered. This is not considered an error.
                return 0;
            }
        }

        void GPURenderingDevice_OpenGL21::UninitWindowFramebuffer(HWND hWnd)
        {
            auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
            if (iFramebuffer != nullptr)
            {
                m_windowFramebuffers.RemoveByIndex(iFramebuffer->index);
            }
        }

        ResultCode GPURenderingDevice_OpenGL21::SetCurrentWindow(HWND hWnd)
        {
            if (hWnd != NULL)
            {
                auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
                if (iFramebuffer != nullptr)
                {
                    auto &framebuffer = iFramebuffer->value;

                    // If the current render target is another window, we want that to be unbound, and then have the new window bound.
                    if ((m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
                    {
                    }

                    m_gl.MakeCurrent(framebuffer.m_hDC, m_gl.GetRenderingContext());

                    m_currentHWND = hWnd;
                    m_currentDC   = framebuffer.m_hDC;
                }
                else
                {
                    // The window can not be used as a render target.
                    return InvalidWindowRenderTarget;
                }
            }
            else
            {
                // No window was specified.
                if ((m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
                {
                        
                }

                m_gl.MakeCurrent(NULL, NULL);

                m_currentHWND = NULL;
                m_currentDC   = NULL;
            }

            return 0;   // No error.
        }
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
    }
}

#endif
