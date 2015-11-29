// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Threading.hpp>

#if defined(_WIN32)
#include <GTEngine/Core/windows.hpp>

namespace GT
{
    void Sleep(unsigned int milliseconds)
    {
        ::Sleep(milliseconds);
    }
}

#else
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>

namespace GT
{
    void Sleep(unsigned int milliseconds)
    {
        usleep(milliseconds * 1000);    // <-- usleep is in microseconds.
    }
}

#endif
