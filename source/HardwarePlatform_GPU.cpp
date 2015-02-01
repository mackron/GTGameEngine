// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/HardwarePlatform_GPU.hpp>
#include <GTGameEngine/GPURenderingDevice_D3D11.hpp>

#include <iostream>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/Strings.hpp>
#include <GTLib/windows.hpp>
#include <dxgi.h>
#include <d3d11.h>

typedef HRESULT (WINAPI *pCreateDXGIFactory1) (REFIID riid, _Out_ void **ppFactory);


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

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03


typedef HGLRC (* PFNWGLCREATECONTEXTPROC)       (HDC hdc);
typedef BOOL  (* PFNWGLDELETECONTEXTPROC)       (HGLRC hglrc);
typedef BOOL  (* PFNWGLMAKECURRENTPROC)         (HDC hdc, HGLRC hglrc);

typedef const GLubyte * (* PFNGLGETSTRINGPROC) (GLenum name);

#endif

namespace GT
{
    namespace GE
    {
        /// Helper function for appending an API code to the given GPURenderingDeviceInfo structure.
        inline void AddSupportedRenderingAPI(GPURenderingDeviceInfo &info, RenderingAPI apiCode)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (info.supportedAPIs[i] == 0)
                {
                    info.supportedAPIs[i] = apiCode;
                    break;
                }
            }
        }

        /// Helper function for determining whether or not the given rendering API is supported by the given GPU rendering device.
        inline bool IsRenderingAPISupported(GPURenderingDeviceInfo &info, RenderingAPI apiCode)
        {
            for (int i = 0; i < 16 && info.supportedAPIs[i] != 0; ++i)
            {
                if (info.supportedAPIs[i] == apiCode)
                {
                    return true;
                }
            }

            return false;
        }



        HardwarePlatform_GPU::HardwarePlatform_GPU()
            : 
#if defined(GT_PLATFORM_WINDOWS)
              m_hDXGI(0),
              m_hD3D11(0),
              m_hD3DCompiler(0),
              m_hOpenGL32(0),
              m_hOpenGLDummyWindow(0),
              m_hOpenGLDummyDC(0),
              m_OpenGLPixelFormat(0),
              m_hOpenGLRC(0)
#endif
        {
        }

        HardwarePlatform_GPU::~HardwarePlatform_GPU()
        {
        }


        ResultCode HardwarePlatform_GPU::Startup()
        {
            // We need to iterate over each display adapter and retrieve information about it. This is very platform-specific.

            // Windows
            //
            // On Windows, we first want to use DXGI. If this is not available we use just the primary adapter that is specified by the OpenGL API.
#if defined(GT_PLATFORM_WINDOWS)
            // D3D Libraries
            m_hDXGI = LoadLibraryW(L"dxgi.dll");
            if (m_hDXGI != 0)
            {
                m_hD3D11 = LoadLibraryW(L"d3d11.dll");
                if (m_hD3D11 != 0)
                {
                    const wchar_t* compilerDLLs[] = {
                        L"d3dcompiler_47.dll",
                        L"d3dcompiler_46.dll",
                        L"d3dcompiler_45.dll",
                        L"d3dcompiler_44.dll",
                        L"d3dcompiler_43.dll",
                        L"d3dcompiler_42.dll",
                        L"d3dcompiler_41.dll",
                        L"d3dcompiler_40.dll",
                        L"d3dcompiler_39.dll",
                        L"d3dcompiler_38.dll",
                        L"d3dcompiler_37.dll",
                        L"d3dcompiler_36.dll",
                        L"d3dcompiler_35.dll",
                        L"d3dcompiler_34.dll",
                        L"d3dcompiler_33.dll",
                    };

                    int compilerDLLCount = sizeof(compilerDLLs) / sizeof(compilerDLLs[0]);
                    for (int i = 0; i < compilerDLLCount; ++i)
                    {
                        m_hD3DCompiler = LoadLibraryW(compilerDLLs[i]);
                        if (m_hD3DCompiler != 0)
                        {
                            break;
                        }
                    }
                }
            }

            // OpenGL Library
            m_hOpenGL32 = LoadLibraryW(L"OpenGL32.dll");



            // We will first try the DXGI API for finding the device information.
            if (m_hDXGI != 0 && m_hD3D11 != 0 && m_hD3DCompiler != 0)
            {
                pCreateDXGIFactory1 _CreateDXGIFactory1 = reinterpret_cast<pCreateDXGIFactory1>(GetProcAddress(m_hDXGI, "CreateDXGIFactory1"));
                if (_CreateDXGIFactory1 != nullptr)
                {
                    IDXGIFactory1* pDXGIFactory1;
                    HRESULT hr = _CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDXGIFactory1));
                    if (SUCCEEDED(hr))
                    {
                        UINT iAdapter = 0;
                        IDXGIAdapter1* adapter;
                        while (SUCCEEDED(pDXGIFactory1->EnumAdapters1(iAdapter, &adapter)))
                        {
                            DXGI_ADAPTER_DESC dxgiDesc;
                            if (SUCCEEDED(adapter->GetDesc(&dxgiDesc)))
                            {
                                GPURenderingDeviceInfo info;
                                ZeroMemory(&info, sizeof(info));
                                GTLib::Strings::Copy(info.description, dxgiDesc.Description);
                                info.identifier_D3D11  = adapter;
                                info.identifier_OpenGL = (iAdapter == 0) ? 1 : 0;



                                // We need to try creating a D3D11 device from this adapter. If this is successful, this is a D3D11 supported device. Note that because we are explicitly specifying
                                // an adapter, we need to also specify D3D_DRIVER_TYPE_UNKNOWN.
                                PFN_D3D11_CREATE_DEVICE _D3D11CreateDevice = reinterpret_cast<PFN_D3D11_CREATE_DEVICE>(GetProcAddress(m_hD3D11, "D3D11CreateDevice"));
                                if (_D3D11CreateDevice != nullptr)
                                {
                                    D3D_FEATURE_LEVEL    featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
                                    D3D_FEATURE_LEVEL    featureLevel    = D3D_FEATURE_LEVEL_11_0;
                                    ID3D11Device*        pD3DDevice      = nullptr;
                                    ID3D11DeviceContext* pD3DContext     = nullptr;
                                    HRESULT hr = _D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, featureLevels, 1, D3D11_SDK_VERSION, &pD3DDevice, &featureLevel, &pD3DContext);
                                    if (SUCCEEDED(hr) && featureLevel == D3D_FEATURE_LEVEL_11_0)
                                    {
                                        AddSupportedRenderingAPI(info, RenderingAPI_D3D11);

                                        pD3DDevice->Release();
                                        pD3DContext->Release();
                                    }
                                }


                                m_renderingDevices.PushBack(info);
                            }

                            ++iAdapter;
                        }

                        pDXGIFactory1->Release();
                    }
                }
            }


            // Now we will check for OpenGL support.
            if (m_hOpenGL32 != 0)
            {
                m_hOpenGLDummyWindow = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
                m_hOpenGLDummyDC     = GetDC(m_hOpenGLDummyWindow);

                PIXELFORMATDESCRIPTOR pfd;
                memset(&pfd, 0, sizeof(pfd));
                pfd.nSize        = sizeof(m_hOpenGLDummyDC);
                pfd.nVersion     = 1;
                pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
                pfd.iPixelType   = PFD_TYPE_RGBA;
                pfd.cStencilBits = 8;
                pfd.cDepthBits   = 24;
                pfd.cColorBits   = 32;
                m_OpenGLPixelFormat = ChoosePixelFormat(m_hOpenGLDummyDC, &pfd);

                if (m_OpenGLPixelFormat != 0)
                {
                    if (SetPixelFormat(m_hOpenGLDummyDC, m_OpenGLPixelFormat,  &pfd))
                    {
                        PFNWGLCREATECONTEXTPROC _wglCreateContext = reinterpret_cast<PFNWGLCREATECONTEXTPROC>(GetProcAddress(m_hOpenGL32, "wglCreateContext"));
                        PFNWGLMAKECURRENTPROC   _wglMakeCurrent   = reinterpret_cast<PFNWGLMAKECURRENTPROC  >(GetProcAddress(m_hOpenGL32, "wglMakeCurrent"));

                        if (_wglCreateContext != nullptr && _wglMakeCurrent != nullptr)
                        {
                            m_hOpenGLRC = _wglCreateContext(m_hOpenGLDummyDC);
                            if (m_hOpenGLRC != 0)
                            {
                                _wglMakeCurrent(m_hOpenGLDummyDC, m_hOpenGLRC);

                                // We need to use glGetString() to determine which version of the OpenGL API is supported.
                                PFNGLGETSTRINGPROC _glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(GetProcAddress(m_hOpenGL32, "glGetString"));
                                if (_glGetString != nullptr)
                                {
                                    const char* versionStr = reinterpret_cast<const char*>(_glGetString(GL_VERSION));
                                    if (versionStr != nullptr)
                                    {
                                        const char* majorStart = versionStr;
                                              char* minorStart;

                                        unsigned int major = strtoul(majorStart, &minorStart, 0);
                                        unsigned int minor = strtoul(minorStart + 1, NULL, 0);

                                        if (major > 2 || (major == 2 && minor >= 1))
                                        {
                                            // If we don't yet have any GPURenderingDeviceInfo objects we'll need to create one.
                                            if (m_renderingDevices.GetCount() == 0)
                                            {
                                                GPURenderingDeviceInfo info;
                                                ZeroMemory(&info, sizeof(info));
                                                GTLib::Strings::Copy(info.description, 256, reinterpret_cast<const char*>(_glGetString(GL_RENDERER)));
                                                info.identifier_OpenGL = 1;

                                                m_renderingDevices.PushBack(info);
                                            }

                                            assert(m_renderingDevices.GetCount() > 0);

                                            GPURenderingDeviceInfo& openGLDeviceInfo = m_renderingDevices[0];       // <-- The OpenGL device should always be the first item since that should correspond to the primary display, which is what OpenGL always uses.
                                            assert(openGLDeviceInfo.identifier_OpenGL == 1);
                                            {
                                                AddSupportedRenderingAPI(openGLDeviceInfo, RenderingAPI_OpenGL21);


                                                // Now we need to check for OpenGL 4.5 support.
                                                // TODO: On NVIDIA we can just use the same version string from glGetString(), however I'm not sure if that's how all vendor's report the maximum version. Test this.
                                                if (major > 4 || (major == 4 && minor >= 5))
                                                {
                                                    AddSupportedRenderingAPI(openGLDeviceInfo, RenderingAPI_OpenGL45);
                                                }
                                                else
                                                {
                                                    // OpenGL 4.5 is not supported.
                                                }
                                            }
                                        }
                                        else
                                        {
                                            // Minimum supported version of OpenGL is not supported.
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
#endif


            return 0;   // No error.
        }

        void HardwarePlatform_GPU::Shutdown()
        {
#if defined(GT_PLATFORM_WINDOWS)
            PFNWGLDELETECONTEXTPROC _wglDeleteContext = reinterpret_cast<PFNWGLDELETECONTEXTPROC>(GetProcAddress(m_hOpenGL32, "wglDeleteContext"));
            if (_wglDeleteContext != nullptr)
            {
                _wglDeleteContext(m_hOpenGLRC);
            }

            DestroyWindow(m_hOpenGLDummyWindow);


            FreeLibrary(m_hOpenGL32);
            FreeLibrary(m_hD3DCompiler);
            FreeLibrary(m_hD3D11);
            FreeLibrary(m_hDXGI);
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
        }


        unsigned int HardwarePlatform_GPU::GetGPURenderingDeviceCount() const
        {
            return static_cast<unsigned int>(m_renderingDevices.GetCount());
        }

        ResultCode HardwarePlatform_GPU::GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const
        {
            if (deviceIndex < m_renderingDevices.GetCount())
            {
                deviceInfoOut = m_renderingDevices[deviceIndex];
                return 0;   // No error.
            }
            else
            {
                // Invalid device index.
                return -1;
            }
        }



        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount, GPURenderingDevice* &deviceOut)
        {
            deviceOut = nullptr;    // <-- Always assign a value to the output argument and default to null.


            if (deviceIndex <= m_renderingDevices.GetCount())
            {
                GPURenderingDeviceInfo &info = m_renderingDevices[deviceIndex];

                RenderingAPI chosenAPI = RenderingAPI_Any;
                for (unsigned int iRenderingAPI = 0; iRenderingAPI < renderingAPIsCount; ++iRenderingAPI)
                {
                    if (IsRenderingAPISupported(info, renderingAPIs[iRenderingAPI]))
                    {
                        chosenAPI = renderingAPIs[iRenderingAPI];
                        break;
                    }
                }


                // If we couldn't find a supported API we'll need to return an error.
                if (chosenAPI != RenderingAPI_Any)
                {
                    GPURenderingDevice* newDevice = nullptr;

                    switch (chosenAPI)
                    {
                    case RenderingAPI_D3D11:
                        {
                            newDevice = new GPURenderingDevice_D3D11(info);
                            break;
                        }

                    case RenderingAPI_OpenGL45:
                        {
                            break;
                        }

                    case RenderingAPI_OpenGL21:
                        {
                            break;
                        }

                    default:
                        {
                            assert(false);
                            break;
                        }
                    }


                    if (newDevice != nullptr)
                    {
                        ResultCode result = newDevice->Startup();
                        if (GT::Succeeded(result))
                        {
                            deviceOut = newDevice;
                            return 0;   // No error.
                        }
                        else
                        {
                            return result;
                        }
                    }
                    else
                    {
                        // Should never actually get here, but in this case the chosen API code is unknown or unimplemented.
                        return -3;
                    }
                }
                else
                {
                    // No supported rendering API.
                    return -2;
                }
            }
            else
            {
                // Invalid device index.
                return -1;
            }
            
        }

        void HardwarePlatform_GPU::DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice)
        {
            delete renderingDevice;
        }



        ResultCode CreateGPUComputeDevice(GPURenderingDevice* &computeDeviceOut)
        {
            computeDeviceOut = nullptr;
            return -1;
        }

        void DeleteGPUComputeDevice(GPURenderingDevice* computeDevice)
        {
            delete computeDevice;
        }




        //////////////////////////////////////////////////////////
        // Private
    }
}
