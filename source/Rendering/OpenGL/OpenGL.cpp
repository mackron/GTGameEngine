// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL45)
#include <cassert>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GTLib/Strings/Tokenizer.hpp>      // For extracting extensions.
#include <cstdlib>
#endif

namespace GT
{
    namespace GE
    {
        OpenGLContext::OpenGLContext()
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
              Viewport(nullptr),
              DepthRange(nullptr),
              DepthFunc(nullptr),
              DepthMask(nullptr),
              StencilOpSeparate(nullptr),
              StencilFuncSeparate(nullptr),
              StencilMaskSeparate(nullptr),
              Enable(nullptr),
              Disable(nullptr),
              ClearColor(nullptr),
              ClearDepth(nullptr),
              ClearStencil(nullptr),
              FrontFace(nullptr),
              CullFace(nullptr),
              PolygonMode(nullptr),
              PolygonOffset(nullptr),

              Clear(nullptr),
              DrawElements(nullptr),

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
              VertexAttribPointer(nullptr),
              EnableVertexAttribArray(nullptr),
              DisableVertexAttribArray(nullptr),

              GenBuffers(nullptr),
              DeleteBuffers(nullptr),
              BindBuffer(nullptr),
              BufferData(nullptr),
              MapBuffer(nullptr),
              UnmapBuffer(nullptr),

              DebugMessageControlARB(nullptr),
              DebugMessageInsertARB(nullptr),
              DebugMessageCallbackARB(nullptr)
        {
        }

        OpenGLContext::~OpenGLContext()
        {
        }


        ResultCode OpenGLContext::Startup(unsigned int majorVersion, unsigned int minorVersion, uint32_t flags)
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

                                                if (m_majorVersion > majorVersion || (m_majorVersion == majorVersion && m_minorVersion >= minorVersion))
                                                {
                                                    // We now need to create an extended context, if applicable.
                                                    if (majorVersion > 2 || (flags & NoCoreContext) == 0)
                                                    {
                                                        auto _wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(this->GetProcAddress("wglCreateContextAttribsARB"));
                                                        if (_wglCreateContextAttribsARB != nullptr)
                                                        {
                                                            int attribList[] =
                                                            {
                                                                WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
                                                                WGL_CONTEXT_MINOR_VERSION_ARB, 1,
                                                                WGL_CONTEXT_FLAGS_ARB,         0,
                                                                0, 0,                               // WGL_CONTEXT_PROFILE_MASK_ARB
                                                                0, 0
                                                            };

                                                            attribList[1] = static_cast<int>(majorVersion);
                                                            attribList[3] = static_cast<int>(minorVersion);

                                                            if ((flags & EnableDebugging) != 0)
                                                            {
                                                                attribList[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
                                                            }

                                                            if ((flags & NoCoreContext) != 0)
                                                            {
                                                                // We're creating a core context. We want this to be 
                                                                if (majorVersion > 2)
                                                                {
                                                                    attribList[5] |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

                                                                    // Setting WGL_CONTEXT_CORE_PROFILE_BIT_ARB wants to fail on nVidia with anything below OpenGL 3.2...
                                                                    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 2))
                                                                    {
                                                                        attribList[6] = WGL_CONTEXT_PROFILE_MASK_ARB; attribList[7] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
                                                                    }
                                                                }
                                                            }
                                                            else
                                                            {
                                                                // If our OpenGL version is 3.2 or above, we need to use a compatibility profile via WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB. If we don't,
                                                                // we'll still get a valid context, but it won't be fully backwards compatible.
                                                                if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 2))
                                                                {
                                                                    attribList[6] = WGL_CONTEXT_PROFILE_MASK_ARB; attribList[7] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                                                                }
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
                                                                    if (majorVersion > 2)
                                                                    {
                                                                        this->Shutdown();
                                                                        return FailedToMakeContextCurrent;
                                                                    }
                                                                }
                                                            }
                                                            else
                                                            {
                                                                if (majorVersion > 2)
                                                                {
                                                                    this->Shutdown();
                                                                    return FailedToCreateContext;
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            if (majorVersion > 2)
                                                            {
                                                                this->Shutdown();
                                                                return FailedToCreateContext;
                                                            }
                                                        }
                                                    }


                                                    if ((flags & NoInitExtensions) == 0)
                                                    {
                                                        ResultCode result = this->InitExtensions();
                                                        if (GT::Failed(result))
                                                        {
                                                            return FailedToRetrievesExtensions;
                                                        }
                                                    }

                                                    if ((flags & NoInitAPI) == 0)
                                                    {
                                                        ResultCode result = this->InitAPI(majorVersion, minorVersion);
                                                        if (GT::Failed(result))
                                                        {
                                                            return FailedToInitAPI;
                                                        }
                                                    }


                                                    // If we get here there were no errors.
                                                    return 0;
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

        void OpenGLContext::Shutdown()
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


        void OpenGLContext::GetVersion(unsigned int &majorVersionOut, unsigned int &minorVersionOut) const
        {
            majorVersionOut = m_majorVersion;
            minorVersionOut = m_minorVersion;
        }

        bool OpenGLContext::IsExtensionSupported(const char* extension) const
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
        HGLRC OpenGLContext::GetRenderingContext() const
        {
            return m_hRC;
        }

        int OpenGLContext::GetPixelFormat() const
        {
            return m_pixelFormat;
        }

        const PIXELFORMATDESCRIPTOR & OpenGLContext::GetPFD() const
        {
            return m_pfd;
        }

        HDC OpenGLContext::GetDummyDC() const
        {
            return m_hDummyDC;
        }
#endif




        //////////////////////////////////////////
        // Private

        void* OpenGLContext::GetGLProcAddress(const char* procName) const
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

        ResultCode OpenGLContext::InitExtensions()
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

        ResultCode OpenGLContext::InitAPI(unsigned int majorVersion, unsigned int minorVersion)
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
                this->Viewport                 = reinterpret_cast<PFNGLVIEWPORTPROC                >(this->GetGLProcAddress("glViewport"));
                this->DepthRange               = reinterpret_cast<PFNGLDEPTHRANGEPROC              >(this->GetGLProcAddress("glDepthRange"));
                this->DepthFunc                = reinterpret_cast<PFNGLDEPTHFUNCPROC               >(this->GetGLProcAddress("glDepthFunc"));
                this->DepthMask                = reinterpret_cast<PFNGLDEPTHMASKPROC               >(this->GetGLProcAddress("glDepthMask"));
                this->StencilOpSeparate        = reinterpret_cast<PFNGLSTENCILOPSEPARATEPROC       >(this->GetGLProcAddress("glStencilOpSeparate"));
                this->StencilFuncSeparate      = reinterpret_cast<PFNGLSTENCILFUNCSEPARATEPROC     >(this->GetGLProcAddress("glStencilFuncSeparate"));
                this->StencilMaskSeparate      = reinterpret_cast<PFNGLSTENCILMASKSEPARATEPROC     >(this->GetGLProcAddress("glStencilMaskSeparate"));
                this->Enable                   = reinterpret_cast<PFNGLENABLEPROC                  >(this->GetGLProcAddress("glEnable"));
                this->Disable                  = reinterpret_cast<PFNGLDISABLEPROC                 >(this->GetGLProcAddress("glDisable"));
                this->ClearColor               = reinterpret_cast<PFNGLCLEARCOLORPROC              >(this->GetGLProcAddress("glClearColor"));
                this->ClearDepth               = reinterpret_cast<PFNGLCLEARDEPTHPROC              >(this->GetGLProcAddress("glClearDepth"));
                this->ClearStencil             = reinterpret_cast<PFNGLCLEARSTENCILPROC            >(this->GetGLProcAddress("glClearStencil"));
                this->FrontFace                = reinterpret_cast<PFNGLFRONTFACEPROC               >(this->GetGLProcAddress("glFrontFace"));
                this->CullFace                 = reinterpret_cast<PFNGLCULLFACEPROC                >(this->GetGLProcAddress("glCullFace"));
                this->PolygonMode              = reinterpret_cast<PFNGLPOLYGONMODEPROC             >(this->GetGLProcAddress("glPolygonMode"));
                this->PolygonOffset            = reinterpret_cast<PFNGLPOLYGONOFFSETPROC           >(this->GetGLProcAddress("glPolygonOffset"));

                this->Clear                    = reinterpret_cast<PFNGLCLEARPROC                   >(this->GetGLProcAddress("glClear"));
                this->DrawElements             = reinterpret_cast<PFNGLDRAWELEMENTSPROC            >(this->GetGLProcAddress("glDrawElements"));

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
                this->VertexAttribPointer      = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC     >(this->GetGLProcAddress("glVertexAttribPointer"));
                this->EnableVertexAttribArray  = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC >(this->GetGLProcAddress("glEnableVertexAttribArray"));
                this->DisableVertexAttribArray = reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(this->GetGLProcAddress("glDisableVertexAttribArray"));

                this->GenBuffers               = reinterpret_cast<PFNGLGENBUFFERSPROC              >(this->GetGLProcAddress("glGenBuffers"));
                this->DeleteBuffers            = reinterpret_cast<PFNGLDELETEBUFFERSPROC           >(this->GetGLProcAddress("glDeleteBuffers"));
                this->BindBuffer               = reinterpret_cast<PFNGLBINDBUFFERPROC              >(this->GetGLProcAddress("glBindBuffer"));
                this->BufferData               = reinterpret_cast<PFNGLBUFFERDATAPROC              >(this->GetGLProcAddress("glBufferData"));
                this->BufferSubData            = reinterpret_cast<PFNGLBUFFERSUBDATAPROC           >(this->GetGLProcAddress("glBufferSubData"));
                this->MapBuffer                = reinterpret_cast<PFNGLMAPBUFFERPROC               >(this->GetGLProcAddress("glMapBuffer"));
                this->UnmapBuffer              = reinterpret_cast<PFNGLUNMAPBUFFERPROC             >(this->GetGLProcAddress("glUnmapBuffer"));


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
}

#endif
