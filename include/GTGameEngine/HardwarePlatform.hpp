// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_hpp_
#define __GT_GE_HardwarePlatform_hpp_

#include "HardwarePlatform_CPU.hpp"
#include "HardwarePlatform_GPU.hpp"
#include "HardwarePlatform_Audio.hpp"
#include "HardwarePlatform_Input.hpp"

namespace GT
{
    /// Class used for detecting hardware and their capabilities on the running system.
    ///
    /// The hardware platform is actually made up of sub-platforms:
    ///     - CPU Platform (CPU, RAM)
    ///     - GPU Platform (GPUs)
    ///     - Audio Hardware Platform (Speakers, Microphones)
    ///     - Input Hardware Platform (Mouse, Keyboard, Controllers)
    class HardwarePlatform
    {
    public:

        /// Constructor.
        HardwarePlatform();

        /// Destructor.
        ~HardwarePlatform();


        /// Starts up the hardware platform.
        ResultCode Startup();

        /// Shuts down the hardware platform.
        void Shutdown();


        /// Retrieves the number of GPU rendering devices available to the application.
        ///
        /// @return The number of GPU devices that are available to the application for rendering.
        unsigned int GetGPURenderingDeviceCount() const;

        /// Retrieves the GPU rendering device info for the device at the given index.
        ///
        /// @param deviceIndex   [in]  The index of the device whose information is being retrieved.
        /// @param deviceInfoOut [out] A reference to the object that will receive the device info.
        ///
        /// @return If an error occurs, the return value will be <0. Otherwise, the return value will be >=0.
        ResultCode GetGPURenderingDeviceInfo(unsigned int deviceIndex, GPURenderingDeviceInfo &deviceInfoOut) const;


        /// @copydoc GT::GE::HardwarePlatform_GPU::CreateGPURenderingDevice()
        GPURenderingDevice* CreateGPURenderingDevice(unsigned int deviceIndex, RenderingAPI renderingAPIs[], unsigned int renderingAPIsCount);

        /// @copydoc GT::GE::HardwarePlatform_GPU::DeleteGPURenderingDevice()
        void DeleteGPURenderingDevice(GPURenderingDevice* device);


    private:

        /// The CPU hardware platform.
        HardwarePlatform_CPU m_cpuHardwarePlatform;

        /// The GPU hardware platform.
        HardwarePlatform_GPU m_gpuHardwarePlatform;

        /// The audio hardware platform.
        HardwarePlatform_Audio m_audioHardwarePlatform;

        /// The input hardware platform.
        HardwarePlatform_Input m_inputHardwarePlatform;



    private:    // No copying.
        HardwarePlatform(const HardwarePlatform &);
        HardwarePlatform & operator=(const HardwarePlatform &);
    };
}

#endif
