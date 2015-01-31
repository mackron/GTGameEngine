// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTLib/Config.hpp>

namespace GT
{
    namespace GE
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



        ResultCode EngineContext::CreateGPURenderingDevice(int apiCode, GPURenderingDevice* &renderingDeviceOut)
        {
            return m_hardwarePlatform.CreateGPURenderingDevice(apiCode, renderingDeviceOut);
        }

        void EngineContext::DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice)
        {
            m_hardwarePlatform.DeleteGPURenderingDevice(renderingDevice);
        }




        //////////////////////////////////////////////////////////
        // Private

    }
}