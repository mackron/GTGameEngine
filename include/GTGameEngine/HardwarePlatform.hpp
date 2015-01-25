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
