// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_IO_cstdio_HPP_
#define __GTLib_IO_cstdio_HPP_

#include <GTLib/Config.hpp>
#include <cstdarg>
#include <cstdio>

#if defined(GT_COMPILER_VC)
#include <direct.h>

namespace GTLib
{
    namespace IO
    {
        inline int mkdir(const char *directory)
        {
            return ::_mkdir(directory);
        }

        inline int vsnprintf(char *buffer, size_t sizeOfBuffer, const char *format, va_list argList)
        {
            if (buffer == nullptr)
            {
                return _vscprintf(format, argList);
            }

            return vsnprintf_s(buffer, sizeOfBuffer, _TRUNCATE, format, argList);
        }
    }
}

inline FILE * popen(const char *command, const char *openMode)
{
    return _popen(command, openMode);
}

inline int pclose(FILE *pipe)
{
    return _pclose(pipe);
}
#else
#include <sys/stat.h>
#include <sys/types.h>

namespace GTLib
{
    namespace IO
    {
        inline int mkdir(const char *directory)
        {
#if defined(GT_PLATFORM_WINDOWS)
            return ::mkdir(directory);
#else
            return ::mkdir(directory, 0777);
#endif
        }

        inline int vsnprintf(char *buffer, size_t sizeOfBuffer, const char *format, va_list argList)
        {
            return std::vsnprintf(buffer, sizeOfBuffer, format, argList);
        }
    }
}
#endif

namespace GTLib
{
    namespace IO
    {
        int snprintf(char *buffer, size_t sizeOfBuffer, const char *format, ...);
    }
}

#endif
