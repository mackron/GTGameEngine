// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsAPI_OpenGL_hpp_
#define __GT_GraphicsAPI_OpenGL_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_OPENGL)
#include "GraphicsAPI.hpp"
#include <GTLib/Vector.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTPROC)       (HDC hdc);
typedef BOOL  (WINAPI * PFNWGLDELETECONTEXTPROC)       (HGLRC hglrc);
typedef BOOL  (WINAPI * PFNWGLMAKECURRENTPROC)         (HDC hdc, HGLRC hglrc);
typedef HGLRC (WINAPI * PFNWGLGETCURRENTCONTEXTPROC)   ();
typedef PROC  (WINAPI * PFNWGLGETPROCADDRESSPROC)      (LPCSTR lpszProc);
typedef BOOL  (WINAPI * PFNWGLSHARELISTSPROC)          (HGLRC hglrc1, HGLRC hglrc2);
typedef HDC   (WINAPI * PFNWGLGETCURRENTDCPROC)        ();
#endif

// Version 1.0
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

// Version 1.1
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


#define GL_TEXTURE_MAX_ANISOTROPY_EXT                     0x84FE

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT                   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT                  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT                  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT                  0x83F3

#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT                  0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT            0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT            0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT            0x8C4F

namespace GT
{
    /// Base class representing a null graphics API.
    class GraphicsAPI_OpenGL : public GraphicsAPI
    {
    public:

        /// Constructor.
        GraphicsAPI_OpenGL();

        /// Destructor.
        ~GraphicsAPI_OpenGL();


        /// @copydoc GraphicsAPI::Startup().
        bool Startup();

        /// @copydoc GraphicsAPI::Shutdown()
        void Shutdown();


        /// @copydoc GraphicsAPI::GetType()
        GraphicsAPIType GetType() const;


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

        /// Initializes the OpenGL extensions.
        ///
        /// @remarks
        ///     This does not retrieve the function pointers for each extension, but rather extracts the extension strings so that IsExtensionSupported() works correctly.
        bool InitExtensions();

        /// Initializes the OpenGL API.
        ///
        /// @remarks
        ///     This assumes a context is already current.
        bool InitAPI();


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


        /// The major version of the OpenGL API that is supported by the given rendering context.
        unsigned int m_majorVersion;

        /// The minor version of the OpenGL API that is supported by the given rendering context.
        unsigned int m_minorVersion;

        /// The list of supported extensions.
        GTLib::Vector<const char*> m_extensions;


    public:

        ///////////////////////////////////////////////////////////////
        // API

#if defined(GT_PLATFORM_WINDOWS)
        PFNWGLCREATECONTEXTPROC      CreateContext;
        PFNWGLDELETECONTEXTPROC      DeleteContext;
        PFNWGLMAKECURRENTPROC        MakeCurrent;
        PFNWGLGETCURRENTCONTEXTPROC  GetCurrentContext;
        PFNWGLGETCURRENTDCPROC       GetCurrentDC;
        PFNWGLGETPROCADDRESSPROC     GetProcAddress;

        PFNWGLSWAPINTERVALEXTPROC    SwapIntervalEXT;
        PFNWGLGETSWAPINTERVALEXTPROC GetSwapIntervalEXT;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif

        PFNGLGETSTRINGPROC              GetString;
        PFNGLGETSTRINGIPROC             GetStringi;
        PFNGLGETINTEGERVPROC            GetIntegerv;
        PFNGLDEPTHFUNCPROC              DepthFunc;
        PFNGLDEPTHMASKPROC              DepthMask;
        PFNGLSTENCILOPSEPARATEPROC      StencilOpSeparate;
        PFNGLSTENCILFUNCSEPARATEPROC    StencilFuncSeparate;
        PFNGLSTENCILMASKSEPARATEPROC    StencilMaskSeparate;
        PFNGLENABLEPROC                 Enable;
        PFNGLDISABLEPROC                Disable;
        PFNGLCLEARCOLORPROC             ClearColor;
        PFNGLCLEARDEPTHPROC             ClearDepth;
        PFNGLCLEARSTENCILPROC           ClearStencil;
        PFNGLCLEARPROC                  Clear;
        PFNGLFRONTFACEPROC              FrontFace;
        PFNGLCULLFACEPROC               CullFace;
        PFNGLPOLYGONMODEPROC            PolygonMode;
        PFNGLPOLYGONOFFSETPROC          PolygonOffset;
        PFNGLBLENDEQUATIONSEPARATEPROC  BlendEquationSeparate;
        PFNGLBLENDFUNCSEPARATEPROC      BlendFuncSeparate;
        PFNGLBLENDCOLORPROC             BlendColor;
        PFNGLCOLORMASKPROC              ColorMask;
        PFNGLFLUSHPROC                  Flush;
        PFNGLFINISHPROC                 Finish;
        PFNGLVIEWPORTPROC               Viewport;


        // Drawing.
        PFNGLDRAWELEMENTSPROC           DrawElements;

        // Shaders.
        PFNGLCREATESHADERPROC           CreateShader;
        PFNGLDELETESHADERPROC           DeleteShader;
        PFNGLSHADERSOURCEPROC           ShaderSource;
        PFNGLCOMPILESHADERPROC          CompileShader;
        PFNGLGETSHADERIVPROC            GetShaderiv;
        PFNGLGETSHADERINFOLOGPROC       GetShaderInfoLog;
        PFNGLCREATEPROGRAMPROC          CreateProgram;
        PFNGLDELETEPROGRAMPROC          DeleteProgram;
        PFNGLATTACHSHADERPROC           AttachShader;
        PFNGLDETACHSHADERPROC           DetachShader;
        PFNGLBINDATTRIBLOCATIONPROC     BindAttribLocation;
        PFNGLLINKPROGRAMPROC            LinkProgram;
        PFNGLGETPROGRAMIVPROC           GetProgramiv;
        PFNGLGETPROGRAMINFOLOGPROC      GetProgramInfoLog;
        PFNGLUSEPROGRAMPROC             UseProgram;
        PFNGLGETATTRIBLOCATIONPROC      GetAttribLocation;
        PFNGLGETUNIFORMLOCATIONPROC     GetUniformLocation;
        PFNGLUNIFORM1FVPROC             Uniform1fv;
        PFNGLUNIFORM2FVPROC             Uniform2fv;
        PFNGLUNIFORM3FVPROC             Uniform3fv;
        PFNGLUNIFORM4FVPROC             Uniform4fv;
        PFNGLUNIFORM1IPROC              Uniform1i;
        PFNGLUNIFORM1IVPROC             Uniform1iv;
        PFNGLUNIFORM2IVPROC             Uniform2iv;
        PFNGLUNIFORM3IVPROC             Uniform3iv;
        PFNGLUNIFORM4IVPROC             Uniform4iv;
        PFNGLUNIFORMMATRIX2FVPROC       UniformMatrix2fv;
        PFNGLUNIFORMMATRIX3FVPROC       UniformMatrix3fv;
        PFNGLUNIFORMMATRIX4FVPROC       UniformMatrix4fv;

        // Vertex Array Objects.


        // Buffer Objects.
        PFNGLGENBUFFERSPROC               GenBuffers;
        PFNGLDELETEBUFFERSARBPROC         DeleteBuffers;
        PFNGLBINDBUFFERPROC               BindBuffer;
        PFNGLBUFFERDATAPROC               BufferData;
        PFNGLBUFFERSUBDATAPROC            BufferSubData;
        PFNGLVERTEXATTRIBPOINTERPROC      VertexAttribPointer;
        PFNGLENABLEVERTEXATTRIBARRAYPROC  EnableVertexAttribArray;
        PFNGLDISABLEVERTEXATTRIBARRAYPROC DisableVertexAttribArray;


        // Textures
        PFNGLGENTEXTURESPROC                 GenTextures;
        PFNGLDELETETEXTURESPROC              DeleteTextures;
        PFNGLACTIVETEXTUREPROC               ActiveTexture;
        PFNGLBINDTEXTUREPROC                 BindTexture;
        PFNGLTEXIMAGE1DPROC                  TexImage1D;
        PFNGLTEXIMAGE2DPROC                  TexImage2D;
        PFNGLTEXIMAGE3DPROC                  TexImage3D;
        PFNGLTEXPARAMETERIPROC               TexParameteri;


        // Framebuffers.


        // GL_ARB_debug_output
        PFNGLDEBUGMESSAGECONTROLARBPROC  DebugMessageControlARB;
        PFNGLDEBUGMESSAGEINSERTARBPROC   DebugMessageInsertARB;
        PFNGLDEBUGMESSAGECALLBACKARBPROC DebugMessageCallbackARB;
    };
}
#endif

#endif
