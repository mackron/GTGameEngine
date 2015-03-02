// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/Config.hpp>

namespace GT
{
    EngineContext::EngineContext()
        : m_hardwarePlatform()
    {
    }

    EngineContext::~EngineContext()
    {
    }


    ResultCode EngineContext::Startup(int argc, char** argv)
    {
        (void)argc;
        (void)argv;


        ResultCode result = 0;

            
        // Hardware platform.
        result = m_hardwarePlatform.Startup();
        if (Failed(result))
        {
            return result;
        }



        return result;
    }

    void EngineContext::Shutdown()
    {
        m_hardwarePlatform.Shutdown();
    }



    unsigned int EngineContext::GetGPURenderingDeviceCount() const
    {
        return m_hardwarePlatform.GetGPURenderingDeviceCount();
    }

    ResultCode EngineContext::GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const
    {
        return m_hardwarePlatform.GetGPURenderingDeviceInfo(deviceIndex, deviceInfoOut);
    }



    ResultCode EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount, GPURenderingDevice* &deviceOut)
    {
        return m_hardwarePlatform.CreateGPURenderingDevice(deviceIndex, renderingAPIs, renderingAPIsCount, deviceOut);
    }

    ResultCode EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPI, GPURenderingDevice* &deviceOut)
    {
        switch (renderingAPI)
        {
        case RenderingAPI_Any:
            {
                RenderingAPI renderingAPIs[] = 
                {
                    RenderingAPI_D3D11,
                    RenderingAPI_OpenGL4
                };
                    
                return this->CreateGPURenderingDevice(deviceIndex, renderingAPIs, sizeof(renderingAPIs) / sizeof(renderingAPIs[0]), deviceOut);
            }

        default:
            {
                return this->CreateGPURenderingDevice(deviceIndex, &renderingAPI, 1, deviceOut);
            }
        }
    }

    ResultCode EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex, GPURenderingDevice* &deviceOut)
    {
        return this->CreateGPURenderingDevice(deviceIndex, RenderingAPI_Any, deviceOut);
    }

    GPURenderingDevice* EngineContext::CreateGPURenderingDevice()
    {
        GPURenderingDevice* device;
        if (GT::Succeeded(this->CreateGPURenderingDevice(0, device)))
        {
            return device;
        }

        return nullptr;
    }

    void EngineContext::DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice)
    {
        m_hardwarePlatform.DeleteGPURenderingDevice(renderingDevice);
    }




    //////////////////////////////////////////////////////////
    // Private
}