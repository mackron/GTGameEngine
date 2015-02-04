// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_OpenGL21_hpp_
#define __GT_GE_GPURenderingDevice_OpenGL21_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTGameEngine/GPURenderingDevice_Gen1.hpp>
#include <GTLib/Map.hpp>


#if defined(GT_PLATFORM_WINDOWS)
// An exact copy of PIXELFORMATDESCRIPTOR so that we don't have to include windows.h in a header file.
struct PIXELFORMATDESCRIPTOR_GTGE
{
    WORD  nSize;
    WORD  nVersion;
    DWORD dwFlags;
    BYTE  iPixelType;
    BYTE  cColorBits;
    BYTE  cRedBits;
    BYTE  cRedShift;
    BYTE  cGreenBits;
    BYTE  cGreenShift;
    BYTE  cBlueBits;
    BYTE  cBlueShift;
    BYTE  cAlphaBits;
    BYTE  cAlphaShift;
    BYTE  cAccumBits;
    BYTE  cAccumRedBits;
    BYTE  cAccumGreenBits;
    BYTE  cAccumBlueBits;
    BYTE  cAccumAlphaBits;
    BYTE  cDepthBits;
    BYTE  cStencilBits;
    BYTE  cAuxBuffers;
    BYTE  iLayerType;
    BYTE  bReserved;
    DWORD dwLayerMask;
    DWORD dwVisibleMask;
    DWORD dwDamageMask;
} ;

typedef HGLRC (* PFNWGLCREATECONTEXTPROC)       (HDC hdc);
typedef BOOL  (* PFNWGLDELETECONTEXTPROC)       (HGLRC hglrc);
typedef BOOL  (* PFNWGLMAKECURRENTPROC)         (HDC hdc, HGLRC hglrc);
typedef HGLRC (* PFNWGLGETCURRENTCONTEXTPROC)   ();
typedef PROC  (* PFNWGLGETPROCADDRESSPROC)      (LPCSTR lpszProc);
typedef BOOL  (* PFNWGLSHARELISTSPROC)          (HGLRC hglrc1, HGLRC hglrc2);
typedef HDC   (* PFNWGLGETCURRENTDCPROC)        ();
#endif

// Base GL types.
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef unsigned short GLhalf;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;


typedef const GLubyte * (* PFNGLGETSTRINGPROC)  (GLenum name);
typedef void            (* PFNGLCLEARPROC)      (GLbitfield mask);
typedef void            (* PFNGLCLEARCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);




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

            /// Retrieves the procedure address of an OpenGL function using wglGetProcAddress(), glXGetProcAddress(), etc.
            ///
            /// @param procName [in] The name of the function to retrieve.
            ///
            /// @remarks
            ///     This does not use the OS-level equivalent (GetProcAddress on Win32 and dlsym on *nix systems).
            void* GetGLProcAddress(const char* procName) const;

            /// Initializes the OpenGL API.
            ///
            /// @remarks
            ///     This assumes a context is already current.
            ///     @par
            ///     This is where version support is checked.
            ResultCode InitGLAPI();


        private:

#if defined(GT_PLATFORM_WINDOWS)
            /// A handle representing the OpenGL32 DLL.
            HMODULE m_hOpenGL32;

            /// The dummy HWND so we can have access to a DC for wglMakeCurrent().
            HWND m_hDummyHWND;

            /// The DC of the dummy window.
            HDC m_hDummyDC;

            /// The OpenGL rendering context.
            HGLRC m_hRC;

            /// The pixel format to use with every window.
            int m_pixelFormat;

            /// The PFD that was used to find the pixel format.
            PIXELFORMATDESCRIPTOR_GTGE m_pfd;


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


            /////////////////////////////////////////////////////
            // WGL API

            PFNWGLCREATECONTEXTPROC  m_wglCreateContext;
            PFNWGLDELETECONTEXTPROC  m_wglDeleteContext;
            PFNWGLMAKECURRENTPROC    m_wglMakeCurrent;
            PFNWGLGETPROCADDRESSPROC m_wglGetProcAddress;
#endif


            ////////////////////////////////////////////////////
            // State

            /// Boolean state flags.
            uint32_t m_stateFlags;


            
            ///////////////////////////////////////////
            // State Flags

            static const uint32_t StageFlag_IsWindowFramebufferCurrent = (1 << 0);     //< Is the current window's framebuffer the current render target?


            /////////////////////////////////////////////////////
            // API

            PFNGLGETSTRINGPROC  m_glGetString;
            PFNGLCLEARPROC      m_glClear;
            PFNGLCLEARCOLORPROC m_glClearColor;



        private:    // No copying.
            GPURenderingDevice_OpenGL21(const GPURenderingDevice_OpenGL21 &);
            GPURenderingDevice_OpenGL21 & operator=(const GPURenderingDevice_OpenGL21 &);
        };
    }
}

#endif

#endif
