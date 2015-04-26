// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/OpenGL/OpenGL.hpp>

#if defined(GT_GE_BUILD_OPENGL4)
#include <cassert>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GTLib/Strings/Tokenizer.hpp>      // For extracting extensions.
#include <cstdlib>
#endif

namespace GT
{
    OpenGL4Context::OpenGL4Context()
        :
#if defined(GT_PLATFORM_WINDOWS)
            m_hOpenGL32(NULL),
            m_hDummyHWND(NULL),
            m_hDummyDC(NULL),
            m_hRC(NULL),
            m_pixelFormat(0),
            m_pfd(),
#endif
#if defined(GT_PLATFORM_LINUX)
#endif

            m_majorVersion(0),
            m_minorVersion(0),
            m_extensions(),

#if defined(GT_PLATFORM_WINDOWS)
            CreateContext(nullptr),
            DeleteContext(nullptr),
            MakeCurrent(nullptr),
            GetCurrentContext(nullptr),
            GetProcAddress(nullptr),

            SwapIntervalEXT(nullptr),
            GetSwapIntervalEXT(nullptr),
#endif
#if defined(GT_PLATFORM_LINUX)
#endif
            GetString(nullptr),
            GetIntegerv(nullptr),

            GetFloati_v(nullptr),
            GetDoublei_v(nullptr),
            ViewportArrayv(nullptr),
            ViewportIndexedf(nullptr),
            DepthRangeArrayv(nullptr),
            DepthRangeIndexed(nullptr),
            ScissorArrayv(nullptr),
            ScissorIndexed(nullptr),
            DepthFunc(nullptr),
            DepthMask(nullptr),
            StencilOpSeparate(nullptr),
            StencilFuncSeparate(nullptr),
            StencilMaskSeparate(nullptr),
            Enable(nullptr),
            Enablei(nullptr),
            Disable(nullptr),
            Disablei(nullptr),
            ClearColor(nullptr),
            ClearDepth(nullptr),
            ClearStencil(nullptr),
            FrontFace(nullptr),
            CullFace(nullptr),
            PolygonMode(nullptr),
            PolygonOffset(nullptr),
            BlendEquationSeparate(nullptr),
            BlendEquationSeparatei(nullptr),
            BlendFuncSeparate(nullptr),
            BlendFuncSeparatei(nullptr),
            BlendColor(nullptr),
            SampleMaski(nullptr),
            ColorMask(nullptr),
            ColorMaski(nullptr),
            ClipControl(nullptr),
            Flush(nullptr),
            Finish(nullptr),

            DrawElementsBaseVertex(nullptr),
            DrawElementsInstancedBaseVertex(nullptr),
            DrawElementsInstancedBaseVertexBaseInstance(nullptr),

            CreateProgramPipelines(nullptr),
            DeleteProgramPipelines(nullptr),
            BindProgramPipeline(nullptr),
            UseProgramStages(nullptr),
            CreateShaderProgramv(nullptr),
            CreateShader(nullptr),
            DeleteShader(nullptr),
            ShaderSource(nullptr),
            ShaderBinary(nullptr),
            CompileShader(nullptr),
            GetShaderiv(nullptr),
            GetShaderInfoLog(nullptr),
            CreateProgram(nullptr),
            DeleteProgram(nullptr),
            AttachShader(nullptr),
            DetachShader(nullptr),
            LinkProgram(nullptr),
            GetProgramiv(nullptr),
            GetProgramInfoLog(nullptr),
            UseProgram(nullptr),
            GetAttribLocation(nullptr),

            CreateVertexArrays(nullptr),
            DeleteVertexArrays(nullptr),
            BindVertexArray(nullptr),
            EnableVertexArrayAttrib(nullptr),
            DisableVertexArrayAttrib(nullptr),
            VertexArrayVertexBuffer(nullptr),
            VertexArrayElementBuffer(nullptr),
            VertexArrayAttribFormat(nullptr),
            VertexArrayAttribIFormat(nullptr),
            VertexArrayAttribLFormat(nullptr),
            VertexArrayAttribBinding(nullptr),
            VertexArrayBindingDivisor(nullptr),

            CreateBuffers(nullptr),
            DeleteBuffers(nullptr),
            NamedBufferStorage(nullptr),
            NamedBufferSubData(nullptr),
            CopyNamedBufferSubData(nullptr),
            ClearNamedBufferSubData(nullptr),
            MapNamedBufferRange(nullptr),
            UnmapNamedBuffer(nullptr),
            FlushMappedNamedBufferRange(nullptr),
            BindBufferBase(nullptr),
            BindBuffersBase(nullptr),
            BindBufferRange(nullptr),
            BindBuffersRange(nullptr),


            CreateTextures(nullptr),
            DeleteTextures(nullptr),
            BindTextureUnit(nullptr),
            TextureStorage1D(nullptr),
            TextureStorage2D(nullptr),
            TextureStorage3D(nullptr),
            TextureStorage2DMultisample(nullptr),
            TextureStorage3DMultisample(nullptr),
            TextureSubImage1D(nullptr),
            TextureSubImage2D(nullptr),
            TextureSubImage3D(nullptr),
            CompressedTextureSubImage1D(nullptr),
            CompressedTextureSubImage2D(nullptr),
            CompressedTextureSubImage3D(nullptr),
            GenerateTextureMipmap(nullptr),
            TextureView(nullptr),

            GenTextures(nullptr),
            CreateSamplers(nullptr),
            DeleteSamplers(nullptr),
            BindSampler(nullptr),
            SamplerParameterf(nullptr),
            SamplerParameterfv(nullptr),
            SamplerParameteri(nullptr),
            SamplerParameteriv(nullptr),
            SamplerParameterIiv(nullptr),
            SamplerParameterIuiv(nullptr),

            CreateFramebuffers(nullptr),
            DeleteFramebuffers(nullptr),
            BindFramebuffer(nullptr),
            NamedFramebufferTexture(nullptr),
            NamedFramebufferTextureLayer(nullptr),
            NamedFramebufferDrawBuffer(nullptr),
            NamedFramebufferDrawBuffers(nullptr),
            ClearNamedFramebufferiv(nullptr),
            ClearNamedFramebufferuiv(nullptr),
            ClearNamedFramebufferfv(nullptr),
            ClearNamedFramebufferfi(nullptr),
            BlitNamedFramebuffer(nullptr),
            CheckNamedFramebufferStatus(nullptr),

            DebugMessageControlARB(nullptr),
            DebugMessageInsertARB(nullptr),
            DebugMessageCallbackARB(nullptr)
    {
    }

    OpenGL4Context::~OpenGL4Context()
    {
    }


    ResultCode OpenGL4Context::Startup(uint32_t flags)
    {
        m_hOpenGL32 = LoadLibraryW(L"OpenGL32.dll");
        if (m_hOpenGL32 != NULL)
        {
            m_hDummyHWND = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
            m_hDummyDC   = GetDC(m_hDummyHWND);

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
                    this->CreateContext = reinterpret_cast<PFNWGLCREATECONTEXTPROC>(::GetProcAddress(m_hOpenGL32, "wglCreateContext"));
                    this->DeleteContext = reinterpret_cast<PFNWGLDELETECONTEXTPROC>(::GetProcAddress(m_hOpenGL32, "wglDeleteContext"));

                    if (this->CreateContext != nullptr && this->DeleteContext != nullptr)
                    {
                        m_hRC = this->CreateContext(m_hDummyDC);
                        if (m_hRC != NULL)
                        {
                            this->MakeCurrent = reinterpret_cast<PFNWGLMAKECURRENTPROC>(::GetProcAddress(m_hOpenGL32, "wglMakeCurrent"));
                            if (this->MakeCurrent != nullptr && this->MakeCurrent(m_hDummyDC, m_hRC))
                            {
                                this->GetProcAddress = reinterpret_cast<PFNWGLGETPROCADDRESSPROC>(::GetProcAddress(m_hOpenGL32, "wglGetProcAddress"));
                                if (this->GetProcAddress != nullptr)
                                {
                                    // We now need to get the version and extensions. With newer versions of OpenGL we need to use glGetStringi() in conjuction with glGetIntegerv() to get extensions.
                                    this->GetString   = reinterpret_cast<PFNGLGETSTRINGPROC  >(this->GetGLProcAddress("glGetString"));
                                    this->GetStringi  = reinterpret_cast<PFNGLGETSTRINGIPROC >(this->GetGLProcAddress("glGetStringi"));
                                    this->GetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(this->GetGLProcAddress("glGetIntegerv"));
                                    if (this->GetString != nullptr && this->GetIntegerv != nullptr)
                                    {
                                        const char* versionStr = reinterpret_cast<const char*>(this->GetString(GL_VERSION));
                                        if (versionStr != nullptr)
                                        {
                                            const char* majorStart = versionStr;
                                                  char* minorStart;

                                            m_majorVersion = strtoul(majorStart, &minorStart, 0);
                                            m_minorVersion = strtoul(minorStart + 1, NULL, 0);

                                            // Clamp at OpenGL 4.5 to ensure we don't break in the future since we'll be creating a core context for the highest possible version.
                                            if (m_majorVersion > 4)
                                            {
                                                m_majorVersion = 4;
                                                m_minorVersion = 5;
                                            }

                                            if (m_majorVersion == 4 && m_minorVersion >= 0)
                                            {
                                                // Now we need the GLSL version. This always matches the OpenGL version in the case of OpenGL 4.
                                                m_majorVersionGLSL = m_majorVersion;
                                                m_minorVersionGLSL = m_minorVersion;



                                                // We now need to create an extended context.
                                                auto _wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(this->GetProcAddress("wglCreateContextAttribsARB"));
                                                if (_wglCreateContextAttribsARB != nullptr)
                                                {
                                                    int attribList[] =
                                                    {
                                                        WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
                                                        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
                                                        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                                                        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                                        0, 0
                                                    };

                                                    attribList[1] = static_cast<int>(m_majorVersion);
                                                    attribList[3] = static_cast<int>(m_minorVersion);

                                                    if ((flags & EnableDebugging) != 0)
                                                    {
                                                        attribList[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
                                                    }
                                                    

                                                    HGLRC oldRC = m_hRC;
                                                    HGLRC newRC = _wglCreateContextAttribsARB(m_hDummyDC, 0, attribList);
                                                    if (newRC != NULL)
                                                    {
                                                        if (this->MakeCurrent(m_hDummyDC, newRC))
                                                        {
                                                            m_hRC = newRC;
                                                            this->DeleteContext(oldRC);
                                                        }
                                                        else
                                                        {
                                                            this->Shutdown();
                                                            return FailedToMakeContextCurrent;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        this->Shutdown();
                                                        return FailedToCreateContext;
                                                    }
                                                }
                                                else
                                                {
                                                    this->Shutdown();
                                                    return FailedToCreateContext;
                                                }


                                                ResultCode result = this->InitExtensions();
                                                if (GT::Succeeded(result))
                                                {
                                                    result = this->InitAPI(m_majorVersion, m_minorVersion);
                                                    if (GT::Succeeded(result))
                                                    {
                                                        // If we get here there were no errors. It's been a long journey...
                                                        return 0;
                                                    }
                                                    else
                                                    {
                                                        this->Shutdown();
                                                        return result;
                                                    }
                                                }
                                                else
                                                {
                                                    this->Shutdown();
                                                    return result;
                                                }
                                            }
                                            else
                                            {
                                                this->Shutdown();
                                                return RenderingAPINotSupported;
                                            }
                                        }
                                        else
                                        {
                                            this->Shutdown();
                                            return FailedToRetrieveVersionString;
                                        }
                                    }
                                    else
                                    {
                                        this->Shutdown();
                                        return FailedToRetrieveGetStringProc;
                                    }
                                }
                                else
                                {
                                    this->Shutdown();
                                    return FailedToMakeContextCurrent;
                                }
                            }
                            else
                            {
                                this->Shutdown();
                                return FailedToMakeContextCurrent;
                            }
                        }
                        else
                        {
                            this->Shutdown();
                            return FailedToCreateContext;
                        }
                    }
                    else
                    {
                        this->Shutdown();
                        return FailedToCreateContext;
                    }
                }
                else
                {
                    this->Shutdown();
                    return FailedToSetPixelFormat;
                }
            }
            else
            {
                this->Shutdown();
                return FailedToFindPixelFormat;
            }
        }
        else
        {
            // Failed to load OpenGL32.dll.
            return FailedToLoadOpenGLLibrary;
        }
    }

    void OpenGL4Context::Shutdown()
    {
        for (size_t iExtension = 0; iExtension < m_extensions.GetCount(); ++iExtension)
        {
            free(const_cast<void*>(reinterpret_cast<const void*>(m_extensions[iExtension])));
        }
        m_extensions.Clear();



        if (this->DeleteContext != nullptr)
        {
            this->DeleteContext(m_hRC);
            m_hRC = NULL;
        }

        DestroyWindow(m_hDummyHWND);
        m_hDummyHWND = NULL;

        FreeLibrary(m_hOpenGL32);
        m_hOpenGL32 = NULL;
    }


    void OpenGL4Context::GetVersion(unsigned int &majorVersionOut, unsigned int &minorVersionOut) const
    {
        majorVersionOut = m_majorVersion;
        minorVersionOut = m_minorVersion;
    }

    void OpenGL4Context::GetGLSLVersion(unsigned int &majorVersionOut, unsigned int &minorVersionOut) const
    {
        majorVersionOut = m_majorVersionGLSL;
        minorVersionOut = m_minorVersionGLSL;
    }

    bool OpenGL4Context::IsExtensionSupported(const char* extension) const
    {
        for (size_t iExtension = 0; iExtension < m_extensions.GetCount(); ++iExtension)
        {
            if (strcmp(m_extensions[iExtension], extension) == 0)
            {
                return true;
            }
        }

        return false;
    }



#if defined(GT_PLATFORM_WINDOWS)
    HGLRC OpenGL4Context::GetRenderingContext() const
    {
        return m_hRC;
    }

    int OpenGL4Context::GetPixelFormat() const
    {
        return m_pixelFormat;
    }

    const PIXELFORMATDESCRIPTOR & OpenGL4Context::GetPFD() const
    {
        return m_pfd;
    }

    HDC OpenGL4Context::GetDummyDC() const
    {
        return m_hDummyDC;
    }
#endif




    //////////////////////////////////////////
    // Private

    void* OpenGL4Context::GetGLProcAddress(const char* procName) const
    {
#if defined(GT_PLATFORM_WINDOWS)
        assert(this->GetProcAddress != nullptr);

        void* proc = ::GetProcAddress(m_hOpenGL32, procName);
        if (proc == nullptr)
        {
            proc = this->GetProcAddress(procName);
        }
            
        return proc;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
    }

    ResultCode OpenGL4Context::InitExtensions()
    {
        if (this->GetStringi != nullptr)
        {
            // Use the new way.
            GLuint extensionCount = 0;
            this->GetIntegerv(GL_NUM_EXTENSIONS, reinterpret_cast<GLint*>(&extensionCount));

            for (GLuint iExtension = 0; iExtension < extensionCount; ++iExtension)
            {
                const char* extensionStrSource = reinterpret_cast<const char*>(this->GetStringi(GL_EXTENSIONS, iExtension));

                size_t extensionStrLength = strlen(extensionStrSource);
                char* extensionStr = reinterpret_cast<char*>(malloc(extensionStrLength + 1));  // +1 for null terminator.
                memcpy(extensionStr, extensionStrSource, extensionStrLength);
                extensionStr[extensionStrLength] = '\0';


                m_extensions.PushBack(extensionStr);
            }
        }
        else
        {
            // Use the old way.
            const char* extensions = reinterpret_cast<const char*>(this->GetString(GL_EXTENSIONS));
            if (extensions != nullptr)
            {
                GTLib::Strings::WhitespaceTokenizerUTF8 extension(extensions);
                while (extension)
                {
                    char* extensionStr = reinterpret_cast<char*>(malloc(extension.GetSizeInBytes() + 1));  // +1 for null terminator.
                    extension.Copy(extensionStr);

                    m_extensions.PushBack(extensionStr);

                    ++extension;
                }
            }
        }


#if defined(GT_PLATFORM_WINDOWS)
        // WGL extensions.
        const char* extensionsWGL = nullptr;

        auto _wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(this->GetProcAddress("wglGetExtensionsStringARB"));
        if (_wglGetExtensionsStringARB != nullptr)
        {
            extensionsWGL = reinterpret_cast<const char*>(_wglGetExtensionsStringARB(m_hDummyDC));
        }
        else
        {
            auto _wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(this->GetProcAddress("wglGetExtensionsStringARB"));
            if (_wglGetExtensionsStringEXT != nullptr)
            {
                extensionsWGL = reinterpret_cast<const char*>(_wglGetExtensionsStringEXT());
            }
        }


        if (extensionsWGL != nullptr)
        {
            GTLib::Strings::WhitespaceTokenizerUTF8 extension(extensionsWGL);
            while (extension)
            {
                char* extensionStr = reinterpret_cast<char*>(malloc(extension.GetSizeInBytes() + 1));  // +1 for null terminator.
                extension.Copy(extensionStr);

                m_extensions.PushBack(extensionStr);

                ++extension;
            }
        }
#endif


        return 0;
    }

    ResultCode OpenGL4Context::InitAPI(unsigned int majorVersion, unsigned int minorVersion)
    {
        if (majorVersion > 2 || (majorVersion == 2 && minorVersion >= 1))
        {
#if defined(GT_PLATFORM_WINDOWS)
            this->GetCurrentContext = reinterpret_cast<PFNWGLGETCURRENTCONTEXTPROC>(::GetProcAddress(m_hOpenGL32, "wglGetCurrentContext"));
                
            if (this->IsExtensionSupported("WGL_EXT_swap_control"))
            {
                this->SwapIntervalEXT    = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC   >(this->GetGLProcAddress("wglSwapIntervalEXT"));
                this->GetSwapIntervalEXT = reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(this->GetGLProcAddress("wglGetSwapIntervalEXT"));
            }
#endif

            // We support OpenGL 2.1 so we will now initialize the rest of the API.
            this->GetIntegerv              = reinterpret_cast<PFNGLGETINTEGERVPROC             >(this->GetGLProcAddress("glGetIntegerv"));
            this->GetFloati_v              = reinterpret_cast<PFNGLGETFLOATI_VPROC             >(this->GetGLProcAddress("glGetFloati_v"));
            this->GetDoublei_v             = reinterpret_cast<PFNGLGETDOUBLEI_VPROC            >(this->GetGLProcAddress("glGetDoublei_v"));
            this->ViewportArrayv           = reinterpret_cast<PFNGLVIEWPORTARRAYVPROC          >(this->GetGLProcAddress("glViewportArrayv"));
            this->ViewportIndexedf         = reinterpret_cast<PFNGLVIEWPORTINDEXEDFPROC        >(this->GetGLProcAddress("glViewportIndexedf"));
            this->DepthRangeArrayv         = reinterpret_cast<PFNGLDEPTHRANGEARRAYVPROC        >(this->GetGLProcAddress("glDepthRangeArrayv"));
            this->DepthRangeIndexed        = reinterpret_cast<PFNGLDEPTHRANGEINDEXEDPROC       >(this->GetGLProcAddress("glDepthRangeIndexed"));
            this->ScissorArrayv            = reinterpret_cast<PFNGLSCISSORARRAYVPROC           >(this->GetGLProcAddress("glScissorArrayv"));
            this->ScissorIndexed           = reinterpret_cast<PFNGLSCISSORINDEXEDPROC          >(this->GetGLProcAddress("glScissorIndexed"));
            this->DepthFunc                = reinterpret_cast<PFNGLDEPTHFUNCPROC               >(this->GetGLProcAddress("glDepthFunc"));
            this->DepthMask                = reinterpret_cast<PFNGLDEPTHMASKPROC               >(this->GetGLProcAddress("glDepthMask"));
            this->StencilOpSeparate        = reinterpret_cast<PFNGLSTENCILOPSEPARATEPROC       >(this->GetGLProcAddress("glStencilOpSeparate"));
            this->StencilFuncSeparate      = reinterpret_cast<PFNGLSTENCILFUNCSEPARATEPROC     >(this->GetGLProcAddress("glStencilFuncSeparate"));
            this->StencilMaskSeparate      = reinterpret_cast<PFNGLSTENCILMASKSEPARATEPROC     >(this->GetGLProcAddress("glStencilMaskSeparate"));
            this->Enable                   = reinterpret_cast<PFNGLENABLEPROC                  >(this->GetGLProcAddress("glEnable"));
            this->Enablei                  = reinterpret_cast<PFNGLENABLEIPROC                 >(this->GetGLProcAddress("glEnablei"));
            this->Disable                  = reinterpret_cast<PFNGLDISABLEPROC                 >(this->GetGLProcAddress("glDisable"));
            this->Disablei                 = reinterpret_cast<PFNGLDISABLEIPROC                >(this->GetGLProcAddress("glDisablei"));
            this->ClearColor               = reinterpret_cast<PFNGLCLEARCOLORPROC              >(this->GetGLProcAddress("glClearColor"));
            this->ClearDepth               = reinterpret_cast<PFNGLCLEARDEPTHPROC              >(this->GetGLProcAddress("glClearDepth"));
            this->ClearStencil             = reinterpret_cast<PFNGLCLEARSTENCILPROC            >(this->GetGLProcAddress("glClearStencil"));
            this->FrontFace                = reinterpret_cast<PFNGLFRONTFACEPROC               >(this->GetGLProcAddress("glFrontFace"));
            this->CullFace                 = reinterpret_cast<PFNGLCULLFACEPROC                >(this->GetGLProcAddress("glCullFace"));
            this->PolygonMode              = reinterpret_cast<PFNGLPOLYGONMODEPROC             >(this->GetGLProcAddress("glPolygonMode"));
            this->PolygonOffset            = reinterpret_cast<PFNGLPOLYGONOFFSETPROC           >(this->GetGLProcAddress("glPolygonOffset"));
            this->BlendEquationSeparate    = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEPROC   >(this->GetGLProcAddress("glBlendEquationSeparate"));
            this->BlendEquationSeparatei   = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEIPROC  >(this->GetGLProcAddress("glBlendEquationSeparatei"));
            this->BlendFuncSeparate        = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC       >(this->GetGLProcAddress("glBlendFuncSeparate"));
            this->BlendFuncSeparatei       = reinterpret_cast<PFNGLBLENDFUNCSEPARATEIPROC      >(this->GetGLProcAddress("glBlendFuncSeparatei"));
            this->BlendColor               = reinterpret_cast<PFNGLBLENDCOLORPROC              >(this->GetGLProcAddress("glBlendColor"));
            this->SampleMaski              = reinterpret_cast<PFNGLSAMPLEMASKIPROC             >(this->GetGLProcAddress("glSampleMaski"));
            this->ColorMask                = reinterpret_cast<PFNGLCOLORMASKPROC               >(this->GetGLProcAddress("glColorMask"));
            this->ColorMaski               = reinterpret_cast<PFNGLCOLORMASKIPROC              >(this->GetGLProcAddress("glColorMaski"));
            this->ClipControl              = reinterpret_cast<PFNGLCLIPCONTROLPROC             >(this->GetGLProcAddress("glClipControl"));
            this->Flush                    = reinterpret_cast<PFNGLFLUSHPROC                   >(this->GetGLProcAddress("glFlush"));
            this->Finish                   = reinterpret_cast<PFNGLFINISHPROC                  >(this->GetGLProcAddress("glFinish"));

            this->DrawElementsBaseVertex                      = reinterpret_cast<PFNGLDRAWELEMENTSBASEVERTEXPROC                     >(this->GetGLProcAddress("glDrawElementsBaseVertex"));
            this->DrawElementsInstancedBaseVertex             = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC            >(this->GetGLProcAddress("glDrawElementsInstancedBaseVertex"));
            this->DrawElementsInstancedBaseVertexBaseInstance = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC>(this->GetGLProcAddress("glDrawElementsInstancedBaseVertexBaseInstance"));

            this->CreateProgramPipelines   = reinterpret_cast<PFNGLCREATEPROGRAMPIPELINESPROC  >(this->GetGLProcAddress("glCreateProgramPipelines"));
            this->DeleteProgramPipelines   = reinterpret_cast<PFNGLDELETEPROGRAMPIPELINESPROC  >(this->GetGLProcAddress("glDeleteProgramPipelines"));
            this->BindProgramPipeline      = reinterpret_cast<PFNGLBINDPROGRAMPIPELINEPROC     >(this->GetGLProcAddress("glBindProgramPipeline"));
            this->UseProgramStages         = reinterpret_cast<PFNGLUSEPROGRAMSTAGESPROC        >(this->GetGLProcAddress("glUseProgramStages"));
            this->CreateShaderProgramv     = reinterpret_cast<PFNGLCREATESHADERPROGRAMVPROC    >(this->GetGLProcAddress("glCreateShaderProgramv"));
            this->CreateShader             = reinterpret_cast<PFNGLCREATESHADERPROC            >(this->GetGLProcAddress("glCreateShader"));
            this->DeleteShader             = reinterpret_cast<PFNGLDELETESHADERPROC            >(this->GetGLProcAddress("glDeleteShader"));
            this->ShaderSource             = reinterpret_cast<PFNGLSHADERSOURCEPROC            >(this->GetGLProcAddress("glShaderSource"));
            this->ShaderBinary             = reinterpret_cast<PFNGLSHADERBINARYPROC            >(this->GetGLProcAddress("glShaderBinary"));
            this->CompileShader            = reinterpret_cast<PFNGLCOMPILESHADERPROC           >(this->GetGLProcAddress("glCompileShader"));
            this->GetShaderiv              = reinterpret_cast<PFNGLGETSHADERIVPROC             >(this->GetGLProcAddress("glGetShaderiv"));
            this->GetShaderInfoLog         = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC        >(this->GetGLProcAddress("glGetShaderInfoLog"));
            this->CreateProgram            = reinterpret_cast<PFNGLCREATEPROGRAMPROC           >(this->GetGLProcAddress("glCreateProgram"));
            this->DeleteProgram            = reinterpret_cast<PFNGLDELETEPROGRAMPROC           >(this->GetGLProcAddress("glDeleteProgram"));
            this->AttachShader             = reinterpret_cast<PFNGLATTACHSHADERPROC            >(this->GetGLProcAddress("glAttachShader"));
            this->DetachShader             = reinterpret_cast<PFNGLDETACHSHADERPROC            >(this->GetGLProcAddress("glDetachShader"));
            this->LinkProgram              = reinterpret_cast<PFNGLLINKPROGRAMPROC             >(this->GetGLProcAddress("glLinkProgram"));
            this->GetProgramiv             = reinterpret_cast<PFNGLGETPROGRAMIVPROC            >(this->GetGLProcAddress("glGetProgramiv"));
            this->GetProgramInfoLog        = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC       >(this->GetGLProcAddress("glGetProgramInfoLog"));
            this->UseProgram               = reinterpret_cast<PFNGLUSEPROGRAMPROC              >(this->GetGLProcAddress("glUseProgram"));
            this->GetAttribLocation        = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC       >(this->GetGLProcAddress("glGetAttribLocation"));


            this->CreateVertexArrays          = reinterpret_cast<PFNGLCREATEVERTEXARRAYSPROC         >(this->GetGLProcAddress("glCreateVertexArrays"));
            this->DeleteVertexArrays          = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC         >(this->GetGLProcAddress("glDeleteVertexArrays"));
            this->BindVertexArray             = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC            >(this->GetGLProcAddress("glBindVertexArray"));
            this->EnableVertexArrayAttrib     = reinterpret_cast<PFNGLENABLEVERTEXARRAYATTRIBPROC    >(this->GetGLProcAddress("glEnableVertexArrayAttrib"));
            this->DisableVertexArrayAttrib    = reinterpret_cast<PFNGLDISABLEVERTEXARRAYATTRIBPROC   >(this->GetGLProcAddress("glDisableVertexArrayAttrib"));
            this->VertexArrayVertexBuffer     = reinterpret_cast<PFNGLVERTEXARRAYVERTEXBUFFERPROC    >(this->GetGLProcAddress("glVertexArrayVertexBuffer"));
            this->VertexArrayElementBuffer    = reinterpret_cast<PFNGLVERTEXARRAYELEMENTBUFFERPROC   >(this->GetGLProcAddress("glVertexArrayElementBuffer"));
            this->VertexArrayAttribFormat     = reinterpret_cast<PFNGLVERTEXARRAYATTRIBFORMATPROC    >(this->GetGLProcAddress("glVertexArrayAttribFormat"));
            this->VertexArrayAttribIFormat    = reinterpret_cast<PFNGLVERTEXARRAYATTRIBIFORMATPROC   >(this->GetGLProcAddress("glVertexArrayAttribIFormat"));
            this->VertexArrayAttribLFormat    = reinterpret_cast<PFNGLVERTEXARRAYATTRIBLFORMATPROC   >(this->GetGLProcAddress("glVertexArrayAttribLFormat"));
            this->VertexArrayAttribBinding    = reinterpret_cast<PFNGLVERTEXARRAYATTRIBBINDINGPROC   >(this->GetGLProcAddress("glVertexArrayAttribBinding"));
            this->VertexArrayBindingDivisor   = reinterpret_cast<PFNGLVERTEXARRAYBINDINGDIVISORPROC  >(this->GetGLProcAddress("glVertexArrayBindingDivisor"));

            this->CreateBuffers               = reinterpret_cast<PFNGLCREATEBUFFERSPROC              >(this->GetGLProcAddress("glCreateBuffers"));
            this->DeleteBuffers               = reinterpret_cast<PFNGLDELETEBUFFERSPROC              >(this->GetGLProcAddress("glDeleteBuffers"));
            this->NamedBufferStorage          = reinterpret_cast<PFNGLNAMEDBUFFERSTORAGEPROC         >(this->GetGLProcAddress("glNamedBufferStorage"));
            this->NamedBufferSubData          = reinterpret_cast<PFNGLNAMEDBUFFERSUBDATAPROC         >(this->GetGLProcAddress("glNamedBufferSubData"));
            this->CopyNamedBufferSubData      = reinterpret_cast<PFNGLCOPYNAMEDBUFFERSUBDATAPROC     >(this->GetGLProcAddress("glCopyNamedBufferSubData"));
            this->ClearNamedBufferSubData     = reinterpret_cast<PFNGLCLEARNAMEDBUFFERSUBDATAPROC    >(this->GetGLProcAddress("glClearNamedBufferSubData"));
            this->MapNamedBufferRange         = reinterpret_cast<PFNGLMAPNAMEDBUFFERRANGEPROC        >(this->GetGLProcAddress("glMapNamedBufferRange"));
            this->UnmapNamedBuffer            = reinterpret_cast<PFNGLUNMAPNAMEDBUFFERPROC           >(this->GetGLProcAddress("glUnmapNamedBuffer"));
            this->FlushMappedNamedBufferRange = reinterpret_cast<PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC>(this->GetGLProcAddress("glFlushMappedNamedBufferRange"));
            this->BindBufferBase              = reinterpret_cast<PFNGLBINDBUFFERBASEPROC             >(this->GetGLProcAddress("glBindBufferBase"));
            this->BindBuffersBase             = reinterpret_cast<PFNGLBINDBUFFERSBASEPROC            >(this->GetGLProcAddress("glBindBuffersBase"));
            this->BindBufferRange             = reinterpret_cast<PFNGLBINDBUFFERRANGEPROC            >(this->GetGLProcAddress("glBindBufferRange"));
            this->BindBuffersRange            = reinterpret_cast<PFNGLBINDBUFFERSRANGEPROC           >(this->GetGLProcAddress("glBindBuffersRange"));

            this->GenTextures                 = reinterpret_cast<PFNGLGENTEXTURESPROC                >(this->GetGLProcAddress("glGenTextures"));
            this->CreateTextures              = reinterpret_cast<PFNGLCREATETEXTURESPROC             >(this->GetGLProcAddress("glCreateTextures"));
            this->DeleteTextures              = reinterpret_cast<PFNGLDELETETEXTURESPROC             >(this->GetGLProcAddress("glDeleteTextures"));
            this->BindTextureUnit             = reinterpret_cast<PFNGLBINDTEXTUREUNITPROC            >(this->GetGLProcAddress("glBindTextureUnit"));
            this->TextureStorage1D            = reinterpret_cast<PFNGLTEXTURESTORAGE1DPROC           >(this->GetGLProcAddress("glTextureStorage1D"));
            this->TextureStorage2D            = reinterpret_cast<PFNGLTEXTURESTORAGE2DPROC           >(this->GetGLProcAddress("glTextureStorage2D"));
            this->TextureStorage3D            = reinterpret_cast<PFNGLTEXTURESTORAGE3DPROC           >(this->GetGLProcAddress("glTextureStorage3D"));
            this->TextureStorage2DMultisample = reinterpret_cast<PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC>(this->GetGLProcAddress("glTextureStorage2DMultisample"));
            this->TextureStorage3DMultisample = reinterpret_cast<PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC>(this->GetGLProcAddress("glTextureStorage3DMultisample"));
            this->TextureSubImage1D           = reinterpret_cast<PFNGLTEXTURESUBIMAGE1DPROC          >(this->GetGLProcAddress("glTextureSubImage1D"));
            this->TextureSubImage2D           = reinterpret_cast<PFNGLTEXTURESUBIMAGE2DPROC          >(this->GetGLProcAddress("glTextureSubImage2D"));
            this->TextureSubImage3D           = reinterpret_cast<PFNGLTEXTURESUBIMAGE3DPROC          >(this->GetGLProcAddress("glTextureSubImage3D"));
            this->CompressedTextureSubImage1D = reinterpret_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC>(this->GetGLProcAddress("glCompressedTextureSubImage1D"));
            this->CompressedTextureSubImage2D = reinterpret_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC>(this->GetGLProcAddress("glCompressedTextureSubImage2D"));
            this->CompressedTextureSubImage3D = reinterpret_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC>(this->GetGLProcAddress("glCompressedTextureSubImage3D"));
            this->GenerateTextureMipmap       = reinterpret_cast<PFNGLGENERATETEXTUREMIPMAPPROC      >(this->GetGLProcAddress("glGenerateTextureMipmap"));
            this->TextureView                 = reinterpret_cast<PFNGLTEXTUREVIEWPROC                >(this->GetGLProcAddress("glTextureView"));

            this->CreateSamplers              = reinterpret_cast<PFNGLCREATESAMPLERSPROC             >(this->GetGLProcAddress("glCreateSamplers"));
            this->DeleteSamplers              = reinterpret_cast<PFNGLDELETESAMPLERSPROC             >(this->GetGLProcAddress("glDeleteSamplers"));
            this->BindSampler                 = reinterpret_cast<PFNGLBINDSAMPLERPROC                >(this->GetGLProcAddress("glBindSampler"));
            this->SamplerParameterf           = reinterpret_cast<PFNGLSAMPLERPARAMETERFPROC          >(this->GetGLProcAddress("glSamplerParameterf"));
            this->SamplerParameterfv          = reinterpret_cast<PFNGLSAMPLERPARAMETERFVPROC         >(this->GetGLProcAddress("glSamplerParameterfv"));
            this->SamplerParameteri           = reinterpret_cast<PFNGLSAMPLERPARAMETERIPROC          >(this->GetGLProcAddress("glSamplerParameteri"));
            this->SamplerParameteriv          = reinterpret_cast<PFNGLSAMPLERPARAMETERIVPROC         >(this->GetGLProcAddress("glSamplerParameteriv"));
            this->SamplerParameterIiv         = reinterpret_cast<PFNGLSAMPLERPARAMETERIIVPROC        >(this->GetGLProcAddress("glSamplerParameterIiv"));
            this->SamplerParameterIuiv        = reinterpret_cast<PFNGLSAMPLERPARAMETERIUIVPROC       >(this->GetGLProcAddress("glSamplerParameterIuiv"));

            this->CreateFramebuffers           = reinterpret_cast<PFNGLCREATEFRAMEBUFFERSPROC          >(this->GetGLProcAddress("glCreateFramebuffers"));
            this->DeleteFramebuffers           = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC          >(this->GetGLProcAddress("glDeleteFramebuffers"));
            this->BindFramebuffer              = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC             >(this->GetGLProcAddress("glBindFramebuffer"));
            this->NamedFramebufferTexture      = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERTEXTUREPROC     >(this->GetGLProcAddress("glNamedFramebufferTexture"));
            this->NamedFramebufferTextureLayer = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC>(this->GetGLProcAddress("glNamedFramebufferTextureLayer"));
            this->NamedFramebufferDrawBuffer   = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC  >(this->GetGLProcAddress("glNamedFramebufferDrawBuffer"));
            this->NamedFramebufferDrawBuffers  = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC >(this->GetGLProcAddress("glNamedFramebufferDrawBuffers"));
            this->ClearNamedFramebufferiv      = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERIVPROC     >(this->GetGLProcAddress("glClearNamedFramebufferiv"));
            this->ClearNamedFramebufferuiv     = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC    >(this->GetGLProcAddress("glClearNamedFramebufferuiv"));
            this->ClearNamedFramebufferfv      = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERFVPROC     >(this->GetGLProcAddress("glClearNamedFramebufferfv"));
            this->ClearNamedFramebufferfi      = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERFIPROC     >(this->GetGLProcAddress("glClearNamedFramebufferfi"));
            this->BlitNamedFramebuffer         = reinterpret_cast<PFNGLBLITNAMEDFRAMEBUFFERPROC        >(this->GetGLProcAddress("glBlitNamedFramebuffer"));
            this->CheckNamedFramebufferStatus  = reinterpret_cast<PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC >(this->GetGLProcAddress("glCheckNamedFramebufferStatus"));


            if (this->IsExtensionSupported("GL_ARB_debug_output"))
            {
                this->DebugMessageControlARB  = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLARBPROC >(this->GetGLProcAddress("glDebugMessageControlARB"));
                this->DebugMessageInsertARB   = reinterpret_cast<PFNGLDEBUGMESSAGEINSERTARBPROC  >(this->GetGLProcAddress("glDebugMessageInsertARB"));
                this->DebugMessageCallbackARB = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKARBPROC>(this->GetGLProcAddress("glDebugMessageCallbackARB"));
            }
        }

        return 0;   // No error.
    }
}

#endif
