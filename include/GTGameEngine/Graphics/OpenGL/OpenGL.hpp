// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_OpenGL_hpp_
#define __GT_GE_Rendering_OpenGL_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL4)
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Vector.hpp>
#include <cstdint>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GL/glcorearb.h>
#include <GL/wglext.h>

typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTPROC)       (HDC hdc);
typedef BOOL  (WINAPI * PFNWGLDELETECONTEXTPROC)       (HGLRC hglrc);
typedef BOOL  (WINAPI * PFNWGLMAKECURRENTPROC)         (HDC hdc, HGLRC hglrc);
typedef HGLRC (WINAPI * PFNWGLGETCURRENTCONTEXTPROC)   ();
typedef PROC  (WINAPI * PFNWGLGETPROCADDRESSPROC)      (LPCSTR lpszProc);
typedef BOOL  (WINAPI * PFNWGLSHARELISTSPROC)          (HGLRC hglrc1, HGLRC hglrc2);
typedef HDC   (WINAPI * PFNWGLGETCURRENTDCPROC)        ();
#endif

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
    /// Class representing an OpenGL context.
    class OpenGL4Context
    {
    public:

        /// Constructor.
        OpenGL4Context();

        /// Destructor.
        ~OpenGL4Context();



        /// Starts up the context.
        ///
        /// @param flags [in] Flags controlling how the context should be started up.
        ///
        /// @return <0 if an error occurs; >= 0 if there is no error.
        ResultCode Startup(uint32_t flags = 0);

        /// Shuts down the context.
        void Shutdown();


        /// Retrieves the version of the OpenGL API supported by the context.
        ///
        /// @param major [out] A reference to the variable that will receive the major version.
        /// @param minor [out] A reference to the variable that will receive the minor version.
        void GetVersion(unsigned int &major, unsigned int &minor) const;

        /// Retrieves the version of GLSL supported by the context.
        ///
        /// @param major [out] A reference to the variable that will receive the major version.
        /// @param minor [out] A reference to the variable that will receive the minor version.
        void GetGLSLVersion(unsigned int &major, unsigned int &minor) const;

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
        ///     This does not retrieve the function pointers for each extension, but rather extracts the extension strings to that IsExtensionSupported() works correctly.
        ResultCode InitExtensions();

        /// Initializes the OpenGL API.
        ///
        /// @param majorVersion [in] The major part of the OpenGL version whose API is being initialized.
        /// @param minorVersion [in] The minor part of the OpenGL version whose API is being initialized.
        ///
        /// @remarks
        ///     This assumes a context is already current.
        ///     @par
        ///     This does not initialize the API if it is not part of the applicable version.
        ResultCode InitAPI(unsigned int majorVersion, unsigned int minorVersion);

            



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

        /// The major version of GLSL that is supported by this context.
        unsigned int m_majorVersionGLSL;

        /// The minor version of GLSL that is supported by this context.
        unsigned int m_minorVersionGLSL;

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
        PFNWGLGETPROCADDRESSPROC     GetProcAddress;

        PFNWGLSWAPINTERVALEXTPROC    SwapIntervalEXT;
        PFNWGLGETSWAPINTERVALEXTPROC GetSwapIntervalEXT;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif

        PFNGLGETSTRINGPROC              GetString;
        PFNGLGETSTRINGIPROC             GetStringi;
        PFNGLGETINTEGERVPROC            GetIntegerv;
        PFNGLGETFLOATI_VPROC            GetFloati_v;
        PFNGLGETDOUBLEI_VPROC           GetDoublei_v;
        PFNGLVIEWPORTARRAYVPROC         ViewportArrayv;
        PFNGLVIEWPORTINDEXEDFPROC       ViewportIndexedf;
        PFNGLDEPTHRANGEARRAYVPROC       DepthRangeArrayv;
        PFNGLDEPTHRANGEINDEXEDPROC      DepthRangeIndexed;
        PFNGLSCISSORARRAYVPROC          ScissorArrayv;
        PFNGLSCISSORINDEXEDPROC         ScissorIndexed;
        PFNGLDEPTHFUNCPROC              DepthFunc;
        PFNGLDEPTHMASKPROC              DepthMask;
        PFNGLSTENCILOPSEPARATEPROC      StencilOpSeparate;
        PFNGLSTENCILFUNCSEPARATEPROC    StencilFuncSeparate;
        PFNGLSTENCILMASKSEPARATEPROC    StencilMaskSeparate;
        PFNGLENABLEPROC                 Enable;
        PFNGLENABLEIPROC                Enablei;
        PFNGLDISABLEPROC                Disable;
        PFNGLDISABLEIPROC               Disablei;
        PFNGLCLEARCOLORPROC             ClearColor;
        PFNGLCLEARDEPTHPROC             ClearDepth;
        PFNGLCLEARSTENCILPROC           ClearStencil;
        PFNGLFRONTFACEPROC              FrontFace;
        PFNGLCULLFACEPROC               CullFace;
        PFNGLPOLYGONMODEPROC            PolygonMode;
        PFNGLPOLYGONOFFSETPROC          PolygonOffset;
        PFNGLBLENDEQUATIONSEPARATEPROC  BlendEquationSeparate;
        PFNGLBLENDEQUATIONSEPARATEIPROC BlendEquationSeparatei;
        PFNGLBLENDFUNCSEPARATEPROC      BlendFuncSeparate;
        PFNGLBLENDFUNCSEPARATEIPROC     BlendFuncSeparatei;
        PFNGLBLENDCOLORPROC             BlendColor;
        PFNGLSAMPLEMASKIPROC            SampleMaski;
        PFNGLCOLORMASKPROC              ColorMask;
        PFNGLCOLORMASKIPROC             ColorMaski;
        PFNGLCLIPCONTROLPROC            ClipControl;
        PFNGLFLUSHPROC                  Flush;
        PFNGLFINISHPROC                 Finish;


        // Drawing.
        PFNGLDRAWELEMENTSBASEVERTEXPROC                      DrawElementsBaseVertex;
        PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC             DrawElementsInstancedBaseVertex;
        PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC DrawElementsInstancedBaseVertexBaseInstance;

        // Shaders.
        PFNGLCREATEPROGRAMPIPELINESPROC CreateProgramPipelines;
        PFNGLDELETEPROGRAMPIPELINESPROC DeleteProgramPipelines;
        PFNGLBINDPROGRAMPIPELINEPROC    BindProgramPipeline;
        PFNGLUSEPROGRAMSTAGESPROC       UseProgramStages;
        PFNGLCREATESHADERPROGRAMVPROC   CreateShaderProgramv;
        PFNGLCREATESHADERPROC           CreateShader;
        PFNGLDELETESHADERPROC           DeleteShader;
        PFNGLSHADERSOURCEPROC           ShaderSource;
        PFNGLSHADERBINARYPROC           ShaderBinary;
        PFNGLCOMPILESHADERPROC          CompileShader;
        PFNGLGETSHADERIVPROC            GetShaderiv;
        PFNGLGETSHADERINFOLOGPROC       GetShaderInfoLog;
        PFNGLCREATEPROGRAMPROC          CreateProgram;
        PFNGLDELETEPROGRAMPROC          DeleteProgram;
        PFNGLATTACHSHADERPROC           AttachShader;
        PFNGLDETACHSHADERPROC           DetachShader;
        PFNGLLINKPROGRAMPROC            LinkProgram;
        PFNGLGETPROGRAMIVPROC           GetProgramiv;
        PFNGLGETPROGRAMINFOLOGPROC      GetProgramInfoLog;
        PFNGLUSEPROGRAMPROC             UseProgram;
        PFNGLGETATTRIBLOCATIONPROC      GetAttribLocation;

        // Vertex Array Objects.
        PFNGLCREATEVERTEXARRAYSPROC          CreateVertexArrays;
        PFNGLDELETEVERTEXARRAYSPROC          DeleteVertexArrays;
        PFNGLBINDVERTEXARRAYPROC             BindVertexArray;
        PFNGLENABLEVERTEXARRAYATTRIBPROC     EnableVertexArrayAttrib;
        PFNGLDISABLEVERTEXARRAYATTRIBPROC    DisableVertexArrayAttrib;
        PFNGLVERTEXARRAYVERTEXBUFFERPROC     VertexArrayVertexBuffer;
        PFNGLVERTEXARRAYELEMENTBUFFERPROC    VertexArrayElementBuffer;
        PFNGLVERTEXARRAYATTRIBFORMATPROC     VertexArrayAttribFormat;
        PFNGLVERTEXARRAYATTRIBIFORMATPROC    VertexArrayAttribIFormat;
        PFNGLVERTEXARRAYATTRIBLFORMATPROC    VertexArrayAttribLFormat;
        PFNGLVERTEXARRAYATTRIBBINDINGPROC    VertexArrayAttribBinding;
        PFNGLVERTEXARRAYBINDINGDIVISORPROC   VertexArrayBindingDivisor;

        // Buffer Objects.
        PFNGLCREATEBUFFERSPROC               CreateBuffers;
        PFNGLDELETEBUFFERSPROC               DeleteBuffers;
        PFNGLNAMEDBUFFERSTORAGEPROC          NamedBufferStorage;
        PFNGLNAMEDBUFFERSUBDATAPROC          NamedBufferSubData;
        PFNGLCOPYNAMEDBUFFERSUBDATAPROC      CopyNamedBufferSubData;
        PFNGLCLEARNAMEDBUFFERSUBDATAPROC     ClearNamedBufferSubData;
        PFNGLMAPNAMEDBUFFERRANGEPROC         MapNamedBufferRange;
        PFNGLUNMAPNAMEDBUFFERPROC            UnmapNamedBuffer;
        PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC FlushMappedNamedBufferRange;
        PFNGLBINDBUFFERBASEPROC              BindBufferBase;
        PFNGLBINDBUFFERSBASEPROC             BindBuffersBase;
        PFNGLBINDBUFFERRANGEPROC             BindBufferRange;
        PFNGLBINDBUFFERSRANGEPROC            BindBuffersRange;


        // Textures
        PFNGLGENTEXTURESPROC                 GenTextures;       // <-- Needed for texture views.
        PFNGLCREATETEXTURESPROC              CreateTextures;
        PFNGLDELETETEXTURESPROC              DeleteTextures;
        PFNGLBINDTEXTUREUNITPROC             BindTextureUnit;
        PFNGLTEXTURESTORAGE1DPROC            TextureStorage1D;
        PFNGLTEXTURESTORAGE2DPROC            TextureStorage2D;
        PFNGLTEXTURESTORAGE3DPROC            TextureStorage3D;
        PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC TextureStorage2DMultisample;
        PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC TextureStorage3DMultisample;
        PFNGLTEXTURESUBIMAGE1DPROC           TextureSubImage1D;
        PFNGLTEXTURESUBIMAGE2DPROC           TextureSubImage2D;
        PFNGLTEXTURESUBIMAGE3DPROC           TextureSubImage3D;
        PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC CompressedTextureSubImage1D;
        PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC CompressedTextureSubImage2D;
        PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC CompressedTextureSubImage3D;
        PFNGLGENERATETEXTUREMIPMAPPROC       GenerateTextureMipmap;
        PFNGLTEXTUREVIEWPROC                 TextureView;

        // Samplers
        PFNGLCREATESAMPLERSPROC              CreateSamplers;
        PFNGLDELETESAMPLERSPROC              DeleteSamplers;
        PFNGLBINDSAMPLERPROC                 BindSampler;
        PFNGLSAMPLERPARAMETERFPROC           SamplerParameterf;
        PFNGLSAMPLERPARAMETERFVPROC          SamplerParameterfv;
        PFNGLSAMPLERPARAMETERIPROC           SamplerParameteri;
        PFNGLSAMPLERPARAMETERIVPROC          SamplerParameteriv;
        PFNGLSAMPLERPARAMETERIIVPROC         SamplerParameterIiv;
        PFNGLSAMPLERPARAMETERIUIVPROC        SamplerParameterIuiv;

        // Framebuffers.
        PFNGLCREATEFRAMEBUFFERSPROC           CreateFramebuffers;
        PFNGLDELETEFRAMEBUFFERSPROC           DeleteFramebuffers;
        PFNGLBINDFRAMEBUFFERPROC              BindFramebuffer;
        PFNGLNAMEDFRAMEBUFFERTEXTUREPROC      NamedFramebufferTexture;
        PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC NamedFramebufferTextureLayer;
        PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC   NamedFramebufferDrawBuffer;
        PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC  NamedFramebufferDrawBuffers;
        PFNGLCLEARNAMEDFRAMEBUFFERIVPROC      ClearNamedFramebufferiv;
        PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC     ClearNamedFramebufferuiv;
        PFNGLCLEARNAMEDFRAMEBUFFERFVPROC      ClearNamedFramebufferfv;
        PFNGLCLEARNAMEDFRAMEBUFFERFIPROC      ClearNamedFramebufferfi;
        PFNGLBLITNAMEDFRAMEBUFFERPROC         BlitNamedFramebuffer;
        PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC  CheckNamedFramebufferStatus;

        // GL_ARB_debug_output
        PFNGLDEBUGMESSAGECONTROLARBPROC  DebugMessageControlARB;
        PFNGLDEBUGMESSAGEINSERTARBPROC   DebugMessageInsertARB;
        PFNGLDEBUGMESSAGECALLBACKARBPROC DebugMessageCallbackARB;


    public:

        //////////////////////////////////////////////////////
        // Error Codes

        static const ResultCode RenderingAPINotSupported      = (1 << 31) | 0x01;
        static const ResultCode FailedToLoadOpenGLLibrary     = (1 << 31) | 0x02;
        static const ResultCode FailedToCreateContext         = (1 << 31) | 0x03;
        static const ResultCode FailedToMakeContextCurrent    = (1 << 31) | 0x04;
        static const ResultCode FailedToInitAPI               = (1 << 31) | 0x05;

        static const ResultCode FailedToRetrieveGetStringProc = (1 << 31) | 0x10;
        static const ResultCode FailedToRetrieveVersionString = (1 << 31) | 0x11;
        static const ResultCode FailedToRetrievesExtensions   = (1 << 31) | 0x12;

        // Win32 Errors
#if defined(GT_PLATFORM_WINDOWS)
        static const ResultCode FailedToFindPixelFormat       = (1 << 31) | 0xF0;
        static const ResultCode FailedToSetPixelFormat        = (1 << 31) | 0xF1;
#endif


        //////////////////////////////////////////////////////
        // Startup Flags

        static const uint32_t EnableDebugging  = (1 << 0);


    private:    // No copying.
        OpenGL4Context(const OpenGL4Context &);
        OpenGL4Context & operator=(const OpenGL4Context &);
    };
}

#endif

#endif
