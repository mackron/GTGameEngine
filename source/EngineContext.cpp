// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/Config.hpp>
#include <GTLib/CommandLine.hpp>

namespace GT
{
    EngineContext::EngineContext()
        : m_commandLine(),
          m_hardwarePlatform(),
          m_fileSystem(),
          m_assetLibrary(m_fileSystem)
    {
    }

    EngineContext::~EngineContext()
    {
    }


    ResultCode EngineContext::Startup(int argc, char** argv)
    {
        ResultCode result = 0;


        // Command line.
        m_commandLine.Parse(argc, argv);



        // Hardware platform.
        result = m_hardwarePlatform.Startup();
        if (GT::Failed(result))
        {
            return result;
        }



        // File system.
        result = m_fileSystem.Startup();
        if (GT::Failed(result))
        {
            return result;
        }

        m_fileSystem.AddLowPriorityBaseDirectory(m_commandLine.GetApplicationDirectory());



        // Asset library.
        result = m_assetLibrary.Startup();
        if (GT::Failed(result))
        {
            return result;
        }



        return result;
    }

    void EngineContext::Shutdown()
    {
        m_assetLibrary.Shutdown();
        m_fileSystem.Shutdown();
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



    GPURenderingDevice* EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount)
    {
        return m_hardwarePlatform.CreateGPURenderingDevice(deviceIndex, renderingAPIs, renderingAPIsCount);
    }

    GPURenderingDevice* EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPI)
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
                    
                return this->CreateGPURenderingDevice(deviceIndex, renderingAPIs, sizeof(renderingAPIs) / sizeof(renderingAPIs[0]));
            }

        default:
            {
                return this->CreateGPURenderingDevice(deviceIndex, &renderingAPI, 1);
            }
        }
    }

    GPURenderingDevice* EngineContext::CreateGPURenderingDevice(unsigned int deviceIndex)
    {
        return this->CreateGPURenderingDevice(deviceIndex, RenderingAPI_Any);
    }

    GPURenderingDevice* EngineContext::CreateGPURenderingDevice()
    {
        return this->CreateGPURenderingDevice(0);
    }

    void EngineContext::DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice)
    {
        m_hardwarePlatform.DeleteGPURenderingDevice(renderingDevice);
    }



    FileSystem & EngineContext::GetFileSystem()
    {
        return m_fileSystem;
    }

    AssetLibrary & EngineContext::GetAssetLibrary()
    {
        return m_assetLibrary;
    }


    //////////////////////////////////////////////////////////
    // Private
}