// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_Audio_hpp_
#define __GT_GE_HardwarePlatform_Audio_hpp_

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    /// Class representing all of the usable input devices (mouse, keyboard, controller, etc) on the running system.
    class HardwarePlatform_Audio
    {
    public:

        /// Constructor.
        HardwarePlatform_Audio();

        /// Destructor.
        ~HardwarePlatform_Audio();


        /// Starts up the hardware platform object.
        ///
        /// @return A result code that determines whether or not initialization was successful, and if not, what the error was.
        ResultCode Startup();

        /// Shuts down the platform object.
        void Shutdown();



    private:    // No copying.
        HardwarePlatform_Audio(const HardwarePlatform_Audio &);
        HardwarePlatform_Audio & operator=(const HardwarePlatform_Audio &);
    };
}

#endif