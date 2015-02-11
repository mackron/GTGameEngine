// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/HardwarePlatform.hpp>

namespace GT
{
    namespace GE
    {
        HardwarePlatform::HardwarePlatform()
            : m_cpuHardwarePlatform(),
              m_gpuHardwarePlatform(),
              m_audioHardwarePlatform(),
              m_inputHardwarePlatform()
        {
        }

        HardwarePlatform::~HardwarePlatform()
        {
        }


        ResultCode HardwarePlatform::Startup()
        {
            ResultCode result = 0;

            result = m_cpuHardwarePlatform.Startup();
            if (Failed(result))
            {
                return result;
            }

            result = m_gpuHardwarePlatform.Startup();
            if (Failed(result))
            {
                m_cpuHardwarePlatform.Shutdown();

                return result;
            }

            result = m_audioHardwarePlatform.Startup();
            if (Failed(result))
            {
                m_gpuHardwarePlatform.Shutdown();
                m_cpuHardwarePlatform.Shutdown();

                return result;
            }

            result = m_inputHardwarePlatform.Startup();
            if (Failed(result))
            {
                m_audioHardwarePlatform.Shutdown();
                m_gpuHardwarePlatform.Shutdown();
                m_cpuHardwarePlatform.Shutdown();

                return result;
            }

            return result;
        }

        void HardwarePlatform::Shutdown()
        {
            m_inputHardwarePlatform.Shutdown();
            m_audioHardwarePlatform.Shutdown();
            m_gpuHardwarePlatform.Shutdown();
            m_cpuHardwarePlatform.Shutdown();
        }



        unsigned int HardwarePlatform::GetGPURenderingDeviceCount() const
        {
            return m_gpuHardwarePlatform.GetGPURenderingDeviceCount();
        }

        ResultCode HardwarePlatform::GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const
        {
            return m_gpuHardwarePlatform.GetGPURenderingDeviceInfo(deviceIndex, deviceInfoOut);
        }


        ResultCode HardwarePlatform::CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount, GPURenderingDevice* &deviceOut)
        {
            return m_gpuHardwarePlatform.CreateGPURenderingDevice(deviceIndex, renderingAPIs, renderingAPIsCount, deviceOut);
        }

        void HardwarePlatform::DeleteGPURenderingDevice(GPURenderingDevice* device)
        {
            m_gpuHardwarePlatform.DeleteGPURenderingDevice(device);
        }
    }
}