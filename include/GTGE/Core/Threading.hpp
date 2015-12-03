// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Threading
#define GT_Threading

#include "Threading/Thread.hpp"
#include <cstdint>

namespace GT
{
    /// Puts the calling thread to sleep for a given number of milliseconds.
    ///
    /// @param  milliseconds [in] The number of milliseconds to put the calling thread to sleep.
    void Sleep(unsigned int milliseconds);
}

#endif
