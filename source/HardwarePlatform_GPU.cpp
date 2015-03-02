// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/HardwarePlatform_GPU.hpp>
#include "Rendering/D3D/D3D11/GPURenderingDevice_D3D11.hpp"
#include "Rendering/OpenGL/OpenGL4/GPURenderingDevice_OpenGL4.hpp"
#include "Rendering/OpenGL/OpenGL21/GPURenderingDevice_OpenGL21.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/Strings.hpp>
#include <GTLib/windows.hpp>


#if defined(GT_GE_BUILD_D3D11)
#include <dxgi.h>
#include <d3d11.h>

typedef HRESULT (WINAPI *pCreateDXGIFactory1) (REFIID riid, _Out_ void **ppFactory);
#endif
#endif

namespace GT
{
    HardwarePlatform_GPU::HardwarePlatform_GPU()
        : 
#if defined(GT_GE_BUILD_D3D11)
            m_hDXGI(0),
            m_hD3D11(0),
            m_hD3DCompiler(0),
#endif
#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL4)
            m_gl()
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
#if defined(GT_GE_BUILD_D3D11)
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
#endif

#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL4)
        static const unsigned int minOpenGLVersionMajor = 2;
        static const unsigned int minOpenGLVersionMinor = 1;
        ResultCode resultGL = m_gl.Startup(minOpenGLVersionMajor, minOpenGLVersionMinor, OpenGLContext::NoInitAPI | OpenGLContext::NoInitExtensions | OpenGLContext::NoCoreContext);
        if (GT::Succeeded(resultGL))
        {
            // If we don't yet have any GPURenderingDeviceInfo objects we'll need to create one.
            if (m_renderingDevices.GetCount() == 0)
            {
                GPURenderingDeviceInfo info;
                ZeroMemory(&info, sizeof(info));
                GTLib::Strings::Copy(info.description, 256, reinterpret_cast<const char*>(m_gl.GetString(GL_RENDERER)));
                info.identifier_OpenGL = 1;

                m_renderingDevices.PushBack(info);
            }

            assert(m_renderingDevices.GetCount() > 0);

            GPURenderingDeviceInfo& openGLDeviceInfo = m_renderingDevices[0];       // <-- The OpenGL device should always be the first item since that should correspond to the primary display, which is what OpenGL always uses.
            assert(openGLDeviceInfo.identifier_OpenGL == 1);
            {
#if defined(GT_GE_BUILD_OPENGL21)
                AddSupportedRenderingAPI(openGLDeviceInfo, RenderingAPI_OpenGL21);
#endif

#if defined(GT_GE_BUILD_OPENGL4)
                // Now we need to check for OpenGL 4.5 support.
                unsigned int majorVersion;
                unsigned int minorVersion;
                m_gl.GetVersion(majorVersion, minorVersion);

                if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 5))
                {

                    AddSupportedRenderingAPI(openGLDeviceInfo, RenderingAPI_OpenGL4);
                }
                else
                {
                    // OpenGL 4.5 is not supported.
                }
#endif
            }
        }
        else
        {
            return resultGL;
        }
#endif


        return 0;   // No error.
    }

    void HardwarePlatform_GPU::Shutdown()
    {
#if defined(GT_GE_BUILD_OPENGL21) || defined(GT_GE_BUILD_OPENGL4)
        m_gl.Shutdown();
#endif

#if defined(GT_GE_BUILD_D3D11)
        // We need to release the D3D11 adapters that are stored in the rendering device info structures.
        for (size_t iRenderingDeviceInfo = 0; iRenderingDeviceInfo < m_renderingDevices.GetCount(); ++iRenderingDeviceInfo)
        {
            auto &info = m_renderingDevices.Get(iRenderingDeviceInfo);
            if (info.identifier_D3D11 != nullptr)
            {
                reinterpret_cast<IDXGIAdapter1*>(info.identifier_D3D11)->Release();
            }
        }

        FreeLibrary(m_hD3DCompiler);
        m_hD3DCompiler = NULL;

        FreeLibrary(m_hD3D11);
        m_hD3D11 = NULL;

        FreeLibrary(m_hDXGI);
        m_hDXGI = NULL;
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
            return InvalidDeviceIndex;
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
#if defined(GT_GE_BUILD_D3D11)
                case RenderingAPI_D3D11:
                    {
                        newDevice = new GPURenderingDevice_D3D11(info);
                        break;
                    }
#endif

#if defined(GT_GE_BUILD_OPENGL4)
                case RenderingAPI_OpenGL4:
                    {
                        newDevice = new GPURenderingDevice_OpenGL4(info);
                        break;
                    }
#endif

#if defined(GT_GE_BUILD_OPENGL21)
                case RenderingAPI_OpenGL21:
                    {
                        newDevice = new GPURenderingDevice_OpenGL21(info);
                        break;
                    }
#endif

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
                    return UnknownRenderingAPI;
                }
            }
            else
            {
                // No supported rendering API.
                return RenderingAPINotSupported;
            }
        }
        else
        {
            // Invalid device index.
            return InvalidDeviceIndex;
        }
            
    }

    void HardwarePlatform_GPU::DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice)
    {
        if (renderingDevice != nullptr)
        {
            renderingDevice->Shutdown();
            delete renderingDevice;
        }
    }



    ResultCode CreateGPUComputeDevice(GPURenderingDevice* &computeDeviceOut)
    {
        computeDeviceOut = nullptr;
        return -1;
    }

    void DeleteGPUComputeDevice(GPURenderingDevice* computeDevice)
    {
        if (computeDevice != nullptr)
        {
            computeDevice->Shutdown();
            delete computeDevice;
        }
    }




    //////////////////////////////////////////////////////////
    // Private
}
