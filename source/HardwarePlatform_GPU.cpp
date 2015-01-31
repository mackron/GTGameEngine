// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/HardwarePlatform_GPU.hpp>
#include <GTGameEngine/GPURenderingDevice_D3D11.hpp>
#include <GTLib/Config.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/Strings.hpp>
#include <GTLib/windows.hpp>
#include <dxgi.h>

typedef HRESULT (WINAPI *pCreateDXGIFactory) (REFIID riid, _Out_ void **ppFactory);

#endif

namespace GT
{
    namespace GE
    {
        HardwarePlatform_GPU::HardwarePlatform_GPU()
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
            // On Windows, we first want to use DXGI. If this is not available, we can use the Direct3D9 API. If this is also not available
            // we use just the primary adapter that is specified by the OpenGL API.
#if defined(GT_PLATFORM_WINDOWS)
            // DXGI
            HMODULE hDXGI = LoadLibraryA("dxgi.dll");
            if (hDXGI != 0)
            {
                pCreateDXGIFactory _CreateDXGIFactory = reinterpret_cast<pCreateDXGIFactory>(GetProcAddress(hDXGI, "CreateDXGIFactor"));
                if (_CreateDXGIFactory != nullptr)
                {
                    IDXGIFactory* factory;
                    HRESULT hr = _CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
                    if (SUCCEEDED(hr))
                    {
                        UINT iAdapter = 0;
                        IDXGIAdapter* adapter;
                        while (SUCCEEDED(factory->EnumAdapters(iAdapter, &adapter)))
                        {
                            DXGI_ADAPTER_DESC dxgiDesc;
                            if (SUCCEEDED(adapter->GetDesc(&dxgiDesc)))
                            {
                                GPURenderingDeviceInfo info;
                                GTLib::Strings::Copy(info.description, dxgiDesc.Description);
                                info.identifier_D3D11  = iAdapter;
                                info.identifier_OpenGL = (iAdapter == 0) ? 1 : 0;
                            }

                            ++iAdapter;
                        }

                        factory->Release();
                    }
                }
            }
            FreeLibrary(hDXGI);



            // Direct3D9
            if (m_renderingDevices.GetCount() == 0)
            {
            }



            // OpenGL
            if (m_renderingDevices.GetCount() == 0)
            {
            }
#endif


            return 0;   // No error.
        }

        void HardwarePlatform_GPU::Shutdown()
        {
        }


        unsigned int HardwarePlatform_GPU::GetGPURenderingDeviceCount() const
        {



            // D3D11.
        }

        ResultCode HardwarePlatform_GPU::GetGPURenderingDeviceInfo(GPURenderingDeviceInfo &deviceInfoOut)
        {
        }



        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice(int apiCode, GPURenderingDevice* &renderingDeviceOut)
        {
            renderingDeviceOut = nullptr;

            switch (apiCode)
            {
            case RenderingAPI_D3D11:    return this->CreateGPURenderingDevice_D3D11(renderingDeviceOut);
            case RenderingAPI_D3D9:     return this->CreateGPURenderingDevice_D3D9(renderingDeviceOut);
            case RenderingAPI_OpenGL45: return this->CreateGPURenderingDevice_OpenGL45(renderingDeviceOut);
            case RenderingAPI_OpenGL21: return this->CreateGPURenderingDevice_OpenGL21(renderingDeviceOut);
            case RenderingAPI_Any_Gen1: return this->CreateGPURenderingDevice_Gen1(renderingDeviceOut);
            case RenderingAPI_Any_Gen2: return this->CreateGPURenderingDevice_Gen2(renderingDeviceOut);
            case RenderingAPI_Any: 
            default:
                {
                    return this->CreateGPURenderingDevice_Any(renderingDeviceOut);
                }
            }
        }

        void HardwarePlatform_GPU::DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice)
        {
            delete renderingDevice;
        }


        ResultCode CreateGPUComputeDevice(GPURenderingDevice* &computeDeviceOut)
        {
            computeDeviceOut = nullptr;
        }

        void DeleteGPUComputeDevice(GPURenderingDevice* computeDevice)
        {
            delete computeDevice;
        }




        //////////////////////////////////////////////////////////
        // Private

        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice_Any(GPURenderingDevice* &renderingDeviceOut)
        {
            ResultCode result = this->CreateGPURenderingDevice_Gen2(renderingDeviceOut);
            if (GT::Failed(result))
            {
                // Failed to create a Generation 2 rendering device, so try a Generation 1.
                result = this->CreateGPURenderingDevice_Gen1(renderingDeviceOut);
            }

            return result;
        }

        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice_Gen1(GPURenderingDevice* &renderingDeviceOut)
        {
            ResultCode result = this->CreateGPURenderingDevice_D3D9(renderingDeviceOut);
            if (GT::Failed(result))
            {
                // Failed to create the D3D9 rendering device, so now try the OpenGL 2.1 device.
                result = this->CreateGPURenderingDevice_OpenGL21(renderingDeviceOut);
            }

            return result;
        }

        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice_Gen2(GPURenderingDevice* &renderingDeviceOut)
        {
            ResultCode result = this->CreateGPURenderingDevice_D3D11(renderingDeviceOut);
            if (GT::Failed(result))
            {
                // Failed to create the D3D11 rendering device, so now try the OpenGL 4.5 device.
                result = this->CreateGPURenderingDevice_OpenGL45(renderingDeviceOut);
            }

            return result;
        }

        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice_OpenGL21(GPURenderingDevice* &renderingDeviceOut)
        {
            // Not currently supported.
            renderingDeviceOut = nullptr;
            return -1;
        }

        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice_OpenGL45(GPURenderingDevice* &renderingDeviceOut)
        {
            // Not currently supported.
            renderingDeviceOut = nullptr;
            return -1;
        }

        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice_D3D9(GPURenderingDevice* &renderingDeviceOut)
        {
#if defined(GT_PLATFORM_WINDOWS)
            // Not currently supported.
            renderingDeviceOut = nullptr;
            return -1;
#else
            // Not supported on this platform.
            renderingDeviceOut = nullptr;
            return -1;
#endif
        }

        ResultCode HardwarePlatform_GPU::CreateGPURenderingDevice_D3D11(GPURenderingDevice* &renderingDeviceOut)
        {
#if defined(GT_PLATFORM_WINDOWS)
            // Not currently supported.
            renderingDeviceOut = nullptr;
            return -1;
#else
            // Not supported on this platform.
            renderingDeviceOut = nullptr;
            return -1;
#endif
        }
    }
}
