// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_GPU_hpp_
#define __GT_GE_HardwarePlatform_GPU_hpp_

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    namespace GE
    {
        /// Class representing all of the usable GPU devices on the running system.
        class HardwarePlatform_GPU
        {
        public:

            /// Constructor.
            HardwarePlatform_GPU();

            /// Destructor.
            ~HardwarePlatform_GPU();


            /// Starts up the hardware platform object.
            ///
            /// @return A result code that determines whether or not initialization was successful, and if not, what the error was.
            ResultCode Startup();

            /// Shuts down the platform object.
            void Shutdown();



        private:    // No copying.
            HardwarePlatform_GPU(const HardwarePlatform_GPU &);
            HardwarePlatform_GPU & operator=(const HardwarePlatform_GPU &);
        };
    }
}

#endif
