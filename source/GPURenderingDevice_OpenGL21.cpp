// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GPURenderingDevice_OpenGL21.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

//#include <gl/GL.h>

/* AttribMask */
#define GL_CURRENT_BIT                    0x00000001
#define GL_POINT_BIT                      0x00000002
#define GL_LINE_BIT                       0x00000004
#define GL_POLYGON_BIT                    0x00000008
#define GL_POLYGON_STIPPLE_BIT            0x00000010
#define GL_PIXEL_MODE_BIT                 0x00000020
#define GL_LIGHTING_BIT                   0x00000040
#define GL_FOG_BIT                        0x00000080
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_ACCUM_BUFFER_BIT               0x00000200
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_VIEWPORT_BIT                   0x00000800
#define GL_TRANSFORM_BIT                  0x00001000
#define GL_ENABLE_BIT                     0x00002000
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_HINT_BIT                       0x00008000
#define GL_EVAL_BIT                       0x00010000
#define GL_LIST_BIT                       0x00020000
#define GL_TEXTURE_BIT                    0x00040000
#define GL_SCISSOR_BIT                    0x00080000
#define GL_ALL_ATTRIB_BITS                0x000fffff

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

namespace GT
{
    namespace GE
    {
        GPURenderingDevice_OpenGL21::GPURenderingDevice_OpenGL21(const GPURenderingDeviceInfo &info)
            : GPURenderingDevice_Gen1(info)
#if defined(GT_PLATFORM_WINDOWS)
            , m_hOpenGL32(NULL),
              m_hDummyHWND(NULL),
              m_hDummyDC(NULL),
              m_hRC(NULL),
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
                m_hOpenGL32 = LoadLibraryW(L"OpenGL32.dll");
                if (m_hOpenGL32 != NULL)
                {
                    m_hDummyHWND = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
                    m_hDummyDC   = GetDC(m_hDummyHWND);

                    PIXELFORMATDESCRIPTOR m_pfd;
                    memset(&m_pfd, 0, sizeof(m_pfd));
                    m_pfd.nSize        = sizeof(m_pfd);
                    m_pfd.nVersion     = 1;
                    m_pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
                    m_pfd.iPixelType   = PFD_TYPE_RGBA;
                    m_pfd.cStencilBits = 8;
                    m_pfd.cDepthBits   = 24;
                    m_pfd.cColorBits   = 32;
                    m_pixelFormat = ChoosePixelFormat(m_hDummyDC, &m_pfd);

                    if (m_pixelFormat != 0)
                    {
                        if (SetPixelFormat(m_hDummyDC, m_pixelFormat,  &m_pfd))
                        {
                            m_wglCreateContext  = reinterpret_cast<PFNWGLCREATECONTEXTPROC >(GetProcAddress(m_hOpenGL32, "wglCreateContext"));
                            m_wglDeleteContext  = reinterpret_cast<PFNWGLDELETECONTEXTPROC >(GetProcAddress(m_hOpenGL32, "wglDeleteContext"));
                            m_wglMakeCurrent    = reinterpret_cast<PFNWGLMAKECURRENTPROC   >(GetProcAddress(m_hOpenGL32, "wglMakeCurrent"));
                            m_wglGetProcAddress = reinterpret_cast<PFNWGLGETPROCADDRESSPROC>(GetProcAddress(m_hOpenGL32, "wglGetProcAddress"));

                            assert(m_wglCreateContext != NULL && m_wglDeleteContext != NULL && m_wglMakeCurrent != NULL && m_wglGetProcAddress != NULL);
                            {
                                m_hRC = m_wglCreateContext(m_hDummyDC);
                                if (m_hRC != NULL)
                                {
                                    if (m_wglMakeCurrent(m_hDummyDC, m_hRC))
                                    {
                                        ResultCode result = this->InitGLAPI();
                                        if (GT::Succeeded(result))
                                        {
                                            return result;
                                        }
                                        else
                                        {
                                            // Failed to initialize the OpenGL API.
                                            m_wglDeleteContext(m_hRC);
                                            m_hRC = NULL;

                                            DestroyWindow(m_hDummyHWND);
                                            m_hDummyHWND = NULL;

                                            FreeLibrary(m_hOpenGL32);
                                            m_hOpenGL32 = NULL;

                                            return result;
                                        }
                                    }
                                    else
                                    {
                                        // Failed to make the new context current.
                                        m_wglDeleteContext(m_hRC);
                                        m_hRC = NULL;

                                        DestroyWindow(m_hDummyHWND);
                                        m_hDummyHWND = NULL;

                                        FreeLibrary(m_hOpenGL32);
                                        m_hOpenGL32 = NULL;

                                        return -5;
                                    }
                                }
                                else
                                {
                                    // Failed to create context.
                                    DestroyWindow(m_hDummyHWND);
                                    m_hDummyHWND = NULL;

                                    FreeLibrary(m_hOpenGL32);
                                    m_hOpenGL32 = NULL;

                                    return -5;
                                }
                            }
                        }
                        else
                        {
                            // Failed to set pixel format.
                            DestroyWindow(m_hDummyHWND);
                            m_hDummyHWND = NULL;

                            FreeLibrary(m_hOpenGL32);
                            m_hOpenGL32 = NULL;

                            return -4;
                        }
                    }
                    else
                    {
                        // Failed to find appropriate pixel format.
                        DestroyWindow(m_hDummyHWND);
                        m_hDummyHWND = NULL;

                        FreeLibrary(m_hOpenGL32);
                        m_hOpenGL32 = NULL;

                        return -3;
                    }
                }
                else
                {
                    // Failed to load OpenGL32.dll.
                    return -2;
                }
            }
            else
            {
                // The device is not the primary adapter or OpenGL 2.1 is not supported.
                return -1;
            }
        }

        void GPURenderingDevice_OpenGL21::Shutdown()
        {
            m_wglDeleteContext(m_hRC);
            m_hRC = NULL;

            DestroyWindow(m_hDummyHWND);
            m_hDummyHWND = NULL;

            FreeLibrary(m_hOpenGL32);
            m_hOpenGL32 = NULL;
        }


        void GPURenderingDevice_OpenGL21::SetSwapInterval(int swapInterval)
        {
            (void)swapInterval;
        }


        void GPURenderingDevice_OpenGL21::ClearColor(float r, float g, float b, float a)
        {
            m_glClearColor(r, g, b, a);
            m_glClear(GL_COLOR_BUFFER_BIT);
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

                if (SetPixelFormat(hDC, m_pixelFormat, reinterpret_cast<PIXELFORMATDESCRIPTOR*>(&m_pfd)))
                {
                    HWNDFramebuffer framebuffer;
                    framebuffer.m_hDC = hDC;

                    m_windowFramebuffers.Add(hWnd, framebuffer);

                    return 0;   // No error.
                }
                else
                {
                    // Failed to set pixel format.
                    return -1;
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
            if (m_currentHWND != hWnd)
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
                            m_wglMakeCurrent(framebuffer.m_hDC, m_hRC);
                        }

                        m_currentHWND = hWnd;
                        m_currentDC   = framebuffer.m_hDC;
                    }
                    else
                    {
                        // The window can not be used as a render target.
                        return -1;
                    }
                }
                else
                {
                    // No window was specified.
                    if ((m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
                    {
                        m_wglMakeCurrent(NULL, NULL);
                    }

                    m_currentHWND = NULL;
                    m_currentDC   = NULL;
                }
            }

            return 0;   // No error.
        }
#endif

#if defined(GT_PLATFORM_LINUX)
#endif



        //////////////////////////////////////////
        // Private

        void* GPURenderingDevice_OpenGL21::GetGLProcAddress(const char* procName) const
        {
#if defined(GT_PLATFORM_WINDOWS)
            assert(m_wglGetProcAddress != nullptr);

            void* proc = GetProcAddress(m_hOpenGL32, procName);
            if (proc == nullptr)
            {
                proc = m_wglGetProcAddress(procName);
            }
            
            return proc;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
        }

        ResultCode GPURenderingDevice_OpenGL21::InitGLAPI()
        {
            // We first need to check for the version.
            m_glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(this->GetGLProcAddress("glGetString"));
            if (m_glGetString != nullptr)
            {
                const char* versionStr = reinterpret_cast<const char*>(m_glGetString(GL_VERSION));
                if (versionStr != nullptr)
                {
                    const char* majorStart = versionStr;
                          char* minorStart;

                    unsigned int major = strtoul(majorStart, &minorStart, 0);
                    unsigned int minor = strtoul(minorStart + 1, NULL, 0);

                    if (major > 2 || (major == 2 && minor >= 1))
                    {
                        // We support OpenGL 2.1 so we will now initialize the rest of the API.
                        m_glClear      = reinterpret_cast<PFNGLCLEARPROC     >(this->GetGLProcAddress("glClear"));
                        m_glClearColor = reinterpret_cast<PFNGLCLEARCOLORPROC>(this->GetGLProcAddress("glClearColor"));


                        return 0;   // No error.
                    }
                    else
                    {
                        // OpenGL 2.1 is not supported.
                        return -3;
                    }
                }
                else
                {
                    // Failed to retrieve version string.
                    return -2;
                }
            }
            else
            {
                // Failed to retrieve glGetString().
                return -1;
            }
        }
    }
}
