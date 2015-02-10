// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_OpenGL_hpp_
#define __GT_GE_Rendering_OpenGL_hpp_

#include <GTGameEngine/Config.hpp>
#include <GTLib/ResultCodes.hpp>

#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL45)
#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>

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

#if defined(GT_PLATFORM_LINUX)
#endif

//#include <gl/GL.h>


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


typedef const GLubyte * (* PFNGLGETSTRINGPROC)  (GLenum name);
typedef void            (* PFNGLCLEARPROC)      (GLbitfield mask);
typedef void            (* PFNGLCLEARCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);


namespace GT
{
    namespace GE
    {
        /// Class representing an OpenGL context.
        class OpenGLContext
        {
        public:

            /// Constructor.
            OpenGLContext();

            /// Destructor.
            ~OpenGLContext();



            /// Starts up the context.
            ///
            /// @return <0 if an error occurs; >= 0 if there is no error.
            ResultCode Startup();

            /// Shuts down the context.
            void Shutdown();


            /// Retrieves the version of the OpenGL API supported by the context.
            ///
            /// @param major [out] A reference to the variable that will receive the major version.
            /// @param minor [out] A reference to the variable that will receive the minor version.
            void GetVersion(unsigned int &major, unsigned int &minor) const;



#if defined(GT_PLATFORM_WINDOWS)
            /// Retrieves a handle to the Windows OpenGL rendering context.
            HGLRC GetRenderingContext() const;

            /// Retrieves the pixel format that must be used for all OpenGL enabled windows using this context.
            int GetPixelFormat() const;

            /// Retrieves a reference to the pixel format descriptor for when an OpenGL enable window is created.
            const PIXELFORMATDESCRIPTOR_GTGE & GetPFD() const;
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
#endif

#if defined(GT_PLATFORM_LINUX)
#endif

            /// The major version of the OpenGL API that is supported by the given rendering context.
            unsigned int m_majorVersion;

            /// The minor version of the OpenGL API that is supported by the given rendering context.
            unsigned int m_minorVersion;


        public:

            ///////////////////////////////////////////////////////////////
            // API

#if defined(GT_PLATFORM_WINDOWS)
            PFNWGLCREATECONTEXTPROC  CreateContext;
            PFNWGLDELETECONTEXTPROC  DeleteContext;
            PFNWGLMAKECURRENTPROC    MakeCurrent;
            PFNWGLGETPROCADDRESSPROC GetProcAddress;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif

            PFNGLGETSTRINGPROC  GetString;
            PFNGLCLEARPROC      Clear;
            PFNGLCLEARCOLORPROC ClearColor;


        public:

            //////////////////////////////////////////////////////
            // Error Codes

            static const ResultCode RenderingAPINotSupported      = (1 << 31) | 0x01;
            static const ResultCode FailedToLoadOpenGLLibrary     = (1 << 31) | 0x02;
            static const ResultCode FailedToCreateContext         = (1 << 31) | 0x03;
            static const ResultCode FailedToMakeContextCurrent    = (1 << 31) | 0x04;

            static const ResultCode FailedToRetrieveGetStringProc = (1 << 31) | 0x10;
            static const ResultCode FailedToRetrieveVersionString = (1 << 31) | 0x11;

            // Win32 Errors
#if defined(GT_PLATFORM_WINDOWS)
            static const ResultCode FailedToFindPixelFormat       = (1 << 31) | 0xF0;
            static const ResultCode FailedToSetPixelFormat        = (1 << 31) | 0xF1;
#endif


        private:    // No copying.
            OpenGLContext(const OpenGLContext &);
            OpenGLContext & operator=(const OpenGLContext &);
        };
    }
}

#endif

#endif
