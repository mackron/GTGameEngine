// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_hpp_
#define __GT_GE_HardwarePlatform_hpp_

#include "HardwarePlatform_CPU.hpp"
#include "HardwarePlatform_GPU.hpp"
#include "HardwarePlatform_Audio.hpp"
#include "HardwarePlatform_Input.hpp"

namespace GT
{
    namespace GE
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


            /// Creates a rendering device for the given API code.
            ///
            /// @param apiCode            [in]  A code identifying the rendering API to use as the back-end.
            /// @param renderingDeviceOut [out] A reference to the variable that will receive a pointer to the new rendering device.
            ///
            /// @return A result code specifying whether or not the device was created successfully. The return value will be >=0 if there was no error.
            ///
            /// @remarks
            ///     Specify a value of 0 for apiCode to automatically choose the best API back-end. On Windows this will try the highest available version of Direct3D. On Linux
            ///     this will pick the highest available version of OpenGL.
            ResultCode CreateGPURenderingDevice(int apiCode, GPURenderingDevice* &renderingDeviceOut);

            /// Deletes a GPU rendering device object that was created with DeleteGPURenderingDevice.
            ///
            /// @param renderingDevice [in] A pointer to the GPU device to delete.
            void DeleteGPURenderingDevice(GPURenderingDevice* renderingDevice);



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
}

#endif
