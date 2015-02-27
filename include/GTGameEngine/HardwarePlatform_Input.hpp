// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_HardwarePlatform_Input_hpp_
#define __GT_GE_HardwarePlatform_Input_hpp_

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    /// Class representing all of the usable input devices (mouse, keyboard, controller, etc) on the running system.
    class HardwarePlatform_Input
    {
    public:

        /// Constructor.
        HardwarePlatform_Input();

        /// Destructor.
        ~HardwarePlatform_Input();


        /// Starts up the hardware platform object.
        ///
        /// @return A result code that determines whether or not initialization was successful, and if not, what the error was.
        ResultCode Startup();

        /// Shuts down the platform object.
        void Shutdown();



    private:    // No copying.
        HardwarePlatform_Input(const HardwarePlatform_Input &);
        HardwarePlatform_Input & operator=(const HardwarePlatform_Input &);
    };
}

#endif