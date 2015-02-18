// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_OpenGL_hpp_
#define __GT_GE_Rendering_OpenGL_hpp_

#include <GTGameEngine/Config.hpp>
#include <GTLib/ResultCodes.hpp>

#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL45)
#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GL/glcorearb.h>
#include <GL/wglext.h>

typedef HGLRC (* PFNWGLCREATECONTEXTPROC)       (HDC hdc);
typedef BOOL  (* PFNWGLDELETECONTEXTPROC)       (HGLRC hglrc);
typedef BOOL  (* PFNWGLMAKECURRENTPROC)         (HDC hdc, HGLRC hglrc);
typedef HGLRC (* PFNWGLGETCURRENTCONTEXTPROC)   ();
typedef PROC  (* PFNWGLGETPROCADDRESSPROC)      (LPCSTR lpszProc);
typedef BOOL  (* PFNWGLSHARELISTSPROC)          (HGLRC hglrc1, HGLRC hglrc2);
typedef HDC   (* PFNWGLGETCURRENTDCPROC)        ();
#endif

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
            const PIXELFORMATDESCRIPTOR & GetPFD() const;
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
            PIXELFORMATDESCRIPTOR m_pfd;
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
            PFNWGLCREATECONTEXTPROC    CreateContext;
            PFNWGLDELETECONTEXTPROC    DeleteContext;
            PFNWGLMAKECURRENTPROC      MakeCurrent;
            PFNWGLGETPROCADDRESSPROC   GetProcAddress;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif

            PFNGLGETSTRINGPROC         GetString;
            PFNGLGETINTEGERVPROC       GetIntegerv;

            PFNGLCLEARPROC             Clear;
            PFNGLCLEARCOLORPROC        ClearColor;
            PFNGLDRAWELEMENTSPROC      DrawElements;

            PFNGLCREATESHADERPROC      CreateShader;
            PFNGLDELETESHADERPROC      DeleteShader;
            PFNGLSHADERSOURCEPROC      ShaderSource;
            PFNGLSHADERBINARYPROC      ShaderBinary;
            PFNGLCOMPILESHADERPROC     CompileShader;
            PFNGLGETSHADERIVPROC       GetShaderiv;
            PFNGLGETSHADERINFOLOGPROC  GetShaderInfoLog;
            PFNGLCREATEPROGRAMPROC     CreateProgram;
            PFNGLDELETEPROGRAMPROC     DeleteProgram;
            PFNGLATTACHSHADERPROC      AttachShader;
            PFNGLDETACHSHADERPROC      DetachShader;
            PFNGLLINKPROGRAMPROC       LinkProgram;
            PFNGLGETPROGRAMIVPROC      GetProgramiv;
            PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;

            PFNGLGENBUFFERSPROC        GenBuffers;
            PFNGLDELETEBUFFERSPROC     DeleteBuffers;
            PFNGLBINDBUFFERPROC        BindBuffer;
            PFNGLBUFFERDATAPROC        BufferData;
            PFNGLBUFFERSUBDATAPROC     BufferSubData;
            PFNGLMAPBUFFERPROC         MapBuffer;
            PFNGLUNMAPBUFFERPROC       UnmapBuffer;


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
