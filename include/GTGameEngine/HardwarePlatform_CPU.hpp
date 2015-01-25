// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_CPU_hpp_
#define __GT_GE_HardwarePlatform_CPU_hpp_

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    namespace GE
    {
        /// Class representing all of the usable CPU and RAM devices on the running system.
        class HardwarePlatform_CPU
        {
        public:

            /// Constructor.
            HardwarePlatform_CPU();

            /// Destructor.
            ~HardwarePlatform_CPU();


            /// Starts up the hardware platform object.
            ///
            /// @return A result code that determines whether or not initialization was successful, and if not, what the error was.
            ResultCode Startup();

            /// Shuts down the hardware platform object.
            void Shutdown();



        private:    // No copying.
            HardwarePlatform_CPU(const HardwarePlatform_CPU &);
            HardwarePlatform_CPU & operator=(const HardwarePlatform_CPU &);
        };
    }
}

#endif