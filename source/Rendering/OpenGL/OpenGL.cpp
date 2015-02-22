// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL45)
#include <cassert>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
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

#if defined(GT_PLATFORM_WINDOWS)
              CreateContext(nullptr),
              DeleteContext(nullptr),
              MakeCurrent(nullptr),
              GetCurrentContext(nullptr),
              GetProcAddress(nullptr),
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
              UnmapBuffer(nullptr)
        {
        }

        OpenGLContext::~OpenGLContext()
        {
        }


        ResultCode OpenGLContext::Startup()
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
                        this->CreateContext     = reinterpret_cast<PFNWGLCREATECONTEXTPROC    >(::GetProcAddress(m_hOpenGL32, "wglCreateContext"));
                        this->DeleteContext     = reinterpret_cast<PFNWGLDELETECONTEXTPROC    >(::GetProcAddress(m_hOpenGL32, "wglDeleteContext"));
                        this->MakeCurrent       = reinterpret_cast<PFNWGLMAKECURRENTPROC      >(::GetProcAddress(m_hOpenGL32, "wglMakeCurrent"));
                        this->GetCurrentContext = reinterpret_cast<PFNWGLGETCURRENTCONTEXTPROC>(::GetProcAddress(m_hOpenGL32, "wglGetCurrentContext"));
                        this->GetProcAddress    = reinterpret_cast<PFNWGLGETPROCADDRESSPROC   >(::GetProcAddress(m_hOpenGL32, "wglGetProcAddress"));

                        assert(this->CreateContext != NULL && this->DeleteContext != NULL && this->MakeCurrent != NULL && this->GetProcAddress != NULL);
                        {
                            m_hRC = this->CreateContext(m_hDummyDC);
                            if (m_hRC != NULL)
                            {
                                if (this->MakeCurrent(m_hDummyDC, m_hRC))
                                {
                                    ResultCode result = this->InitGLAPI();
                                    if (GT::Succeeded(result))
                                    {
                                        return result;
                                    }
                                    else
                                    {
                                        // Failed to initialize the OpenGL API.
                                        this->DeleteContext(m_hRC);
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
                                    this->DeleteContext(m_hRC);
                                    m_hRC = NULL;

                                    DestroyWindow(m_hDummyHWND);
                                    m_hDummyHWND = NULL;

                                    FreeLibrary(m_hOpenGL32);
                                    m_hOpenGL32 = NULL;

                                    return FailedToMakeContextCurrent;
                                }
                            }
                            else
                            {
                                // Failed to create context.
                                DestroyWindow(m_hDummyHWND);
                                m_hDummyHWND = NULL;

                                FreeLibrary(m_hOpenGL32);
                                m_hOpenGL32 = NULL;

                                return FailedToCreateContext;
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

                        return FailedToSetPixelFormat;
                    }
                }
                else
                {
                    // Failed to find appropriate pixel format.
                    DestroyWindow(m_hDummyHWND);
                    m_hDummyHWND = NULL;

                    FreeLibrary(m_hOpenGL32);
                    m_hOpenGL32 = NULL;

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
            this->DeleteContext(m_hRC);
            m_hRC = NULL;

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

        ResultCode OpenGLContext::InitGLAPI()
        {
            // We first need to check for the version.
            this->GetString = reinterpret_cast<PFNGLGETSTRINGPROC>(this->GetGLProcAddress("glGetString"));
            if (this->GetString != nullptr)
            {
                const char* versionStr = reinterpret_cast<const char*>(this->GetString(GL_VERSION));
                if (versionStr != nullptr)
                {
                    const char* majorStart = versionStr;
                          char* minorStart;

                    m_majorVersion = strtoul(majorStart, &minorStart, 0);
                    m_minorVersion = strtoul(minorStart + 1, NULL, 0);

                    if (m_majorVersion > 2 || (m_majorVersion == 2 && m_minorVersion >= 1))
                    {
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


                        return 0;   // No error.
                    }
                    else
                    {
                        // OpenGL 2.1 is not supported.
                        return RenderingAPINotSupported;
                    }
                }
                else
                {
                    // Failed to retrieve version string.
                    return FailedToRetrieveVersionString;
                }
            }
            else
            {
                // Failed to retrieve glGetString().
                return FailedToRetrieveGetStringProc;
            }
        }
    }
}

#endif
