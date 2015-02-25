// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_OpenGL_hpp_
#define __GT_GE_Rendering_OpenGL_hpp_

#include <GTGameEngine/Config.hpp>
#include <GTLib/ResultCodes.hpp>

#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL45)
#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GL/GL.h>
#include <GL/glext.h>
#include <GL/wglext.h>

typedef void (APIENTRYP PFNGLCULLFACEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLFRONTFACEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLHINTPROC) (GLenum target, GLenum mode);
typedef void (APIENTRYP PFNGLLINEWIDTHPROC) (GLfloat width);
typedef void (APIENTRYP PFNGLPOINTSIZEPROC) (GLfloat size);
typedef void (APIENTRYP PFNGLPOLYGONMODEPROC) (GLenum face, GLenum mode);
typedef void (APIENTRYP PFNGLSCISSORPROC) (GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC) (GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLDRAWBUFFERPROC) (GLenum buf);
typedef void (APIENTRYP PFNGLCLEARPROC) (GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC) (GLint s);
typedef void (APIENTRYP PFNGLCLEARDEPTHPROC) (GLdouble depth);
typedef void (APIENTRYP PFNGLSTENCILMASKPROC) (GLuint mask);
typedef void (APIENTRYP PFNGLCOLORMASKPROC) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (APIENTRYP PFNGLDEPTHMASKPROC) (GLboolean flag);
typedef void (APIENTRYP PFNGLDISABLEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLENABLEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLFINISHPROC) (void);
typedef void (APIENTRYP PFNGLFLUSHPROC) (void);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC) (GLenum sfactor, GLenum dfactor);
typedef void (APIENTRYP PFNGLLOGICOPPROC) (GLenum opcode);
typedef void (APIENTRYP PFNGLSTENCILFUNCPROC) (GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLSTENCILOPPROC) (GLenum fail, GLenum zfail, GLenum zpass);
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC) (GLenum func);
typedef void (APIENTRYP PFNGLPIXELSTOREFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPIXELSTOREIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLREADBUFFERPROC) (GLenum src);
typedef void (APIENTRYP PFNGLREADPIXELSPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLGETBOOLEANVPROC) (GLenum pname, GLboolean *data);
typedef void (APIENTRYP PFNGLGETDOUBLEVPROC) (GLenum pname, GLdouble *data);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC) (void);
typedef void (APIENTRYP PFNGLGETFLOATVPROC) (GLenum pname, GLfloat *data);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC) (GLenum pname, GLint *data);
typedef const GLubyte *(APIENTRYP PFNGLGETSTRINGPROC) (GLenum name);
typedef void (APIENTRYP PFNGLGETTEXIMAGEPROC) (GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC) (GLenum target, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC) (GLenum target, GLint level, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISENABLEDPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLDEPTHRANGEPROC) (GLdouble near, GLdouble far);
typedef void (APIENTRYP PFNGLVIEWPORTPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

typedef void (APIENTRYP PFNGLDRAWARRAYSPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRYP PFNGLGETPOINTERVPROC) (GLenum pname, void **params);
typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC) (GLfloat factor, GLfloat units);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC) (GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC) (GLsizei n, const GLuint *textures);
typedef void (APIENTRYP PFNGLGENTEXTURESPROC) (GLsizei n, GLuint *textures);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREPROC) (GLuint texture);

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

            /// Helper for determining whether or not the given extension is supported.
            ///
            /// @param extension [in] A reference to the extension string.
            ///
            /// @return True if the extension is supported.
            bool IsExtensionSupported(const char* extension) const;


#if defined(GT_PLATFORM_WINDOWS)
            /// Retrieves a handle to the Windows OpenGL rendering context.
            HGLRC GetRenderingContext() const;

            /// Retrieves the pixel format that must be used for all OpenGL enabled windows using this context.
            int GetPixelFormat() const;

            /// Retrieves a reference to the pixel format descriptor for when an OpenGL enable window is created.
            const PIXELFORMATDESCRIPTOR & GetPFD() const;

            /// Retrieves a handle to the dummy device context.
            ///
            /// @remarks
            ///     When a null window is made current, the dummy window (which is never visible) will be made current. This will allow
            ///     rendering commands to be issued, but not actually displayed on anything.
            HDC GetDummyDC() const;
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
            PFNWGLCREATECONTEXTPROC      CreateContext;
            PFNWGLDELETECONTEXTPROC      DeleteContext;
            PFNWGLMAKECURRENTPROC        MakeCurrent;
            PFNWGLGETCURRENTCONTEXTPROC  GetCurrentContext;
            PFNWGLGETPROCADDRESSPROC     GetProcAddress;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif

            PFNGLGETSTRINGPROC           GetString;
            PFNGLGETINTEGERVPROC         GetIntegerv;
            PFNGLVIEWPORTPROC            Viewport;
            PFNGLDEPTHRANGEPROC          DepthRange;
            PFNGLDEPTHFUNCPROC           DepthFunc;
            PFNGLDEPTHMASKPROC           DepthMask;
            PFNGLSTENCILOPSEPARATEPROC   StencilOpSeparate;
            PFNGLSTENCILFUNCSEPARATEPROC StencilFuncSeparate;
            PFNGLSTENCILMASKSEPARATEPROC StencilMaskSeparate;
            PFNGLENABLEPROC              Enable;
            PFNGLDISABLEPROC             Disable;
            PFNGLCLEARCOLORPROC          ClearColor;
            PFNGLCLEARDEPTHPROC          ClearDepth;
            PFNGLCLEARSTENCILPROC        ClearStencil;
            PFNGLFRONTFACEPROC           FrontFace;
            PFNGLCULLFACEPROC            CullFace;
            PFNGLPOLYGONMODEPROC         PolygonMode;
            PFNGLPOLYGONOFFSETPROC       PolygonOffset;

            PFNGLCLEARPROC               Clear;
            PFNGLDRAWELEMENTSPROC        DrawElements;

            PFNGLCREATESHADERPROC        CreateShader;
            PFNGLDELETESHADERPROC        DeleteShader;
            PFNGLSHADERSOURCEPROC        ShaderSource;
            PFNGLSHADERBINARYPROC        ShaderBinary;
            PFNGLCOMPILESHADERPROC       CompileShader;
            PFNGLGETSHADERIVPROC         GetShaderiv;
            PFNGLGETSHADERINFOLOGPROC    GetShaderInfoLog;
            PFNGLCREATEPROGRAMPROC       CreateProgram;
            PFNGLDELETEPROGRAMPROC       DeleteProgram;
            PFNGLATTACHSHADERPROC        AttachShader;
            PFNGLDETACHSHADERPROC        DetachShader;
            PFNGLLINKPROGRAMPROC         LinkProgram;
            PFNGLGETPROGRAMIVPROC        GetProgramiv;
            PFNGLGETPROGRAMINFOLOGPROC   GetProgramInfoLog;
            PFNGLUSEPROGRAMPROC          UseProgram;
            PFNGLGETATTRIBLOCATIONPROC   GetAttribLocation;
            PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer;
            PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
            PFNGLDISABLEVERTEXATTRIBARRAYPROC DisableVertexAttribArray;

            PFNGLGENBUFFERSPROC          GenBuffers;
            PFNGLDELETEBUFFERSPROC       DeleteBuffers;
            PFNGLBINDBUFFERPROC          BindBuffer;
            PFNGLBUFFERDATAPROC          BufferData;
            PFNGLBUFFERSUBDATAPROC       BufferSubData;
            PFNGLMAPBUFFERPROC           MapBuffer;
            PFNGLUNMAPBUFFERPROC         UnmapBuffer;


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
