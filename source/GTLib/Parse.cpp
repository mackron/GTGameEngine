// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Parse.hpp>
#include <GTLib/BasicBuffer.hpp>
#include <GTLib/Config.hpp>
#include <GTLib/stdlib.hpp>

#include <cstring>

namespace GTLib
{
    template <> int Parse<int>(const char *value, int base)
    {
        return static_cast<int>(std::strtol(value, nullptr, base));
    }

    template <> unsigned int Parse<unsigned int>(const char *value, int base)
    {
        return static_cast<unsigned int>(std::strtoul(value, nullptr, base));
    }

    template <> int64_t Parse<int64_t>(const char *value, int base)
    {
#if defined(GT_PLATFORM_WINDOWS)
        return static_cast<int64_t>(_strtoi64(value, nullptr, base));
#else
        return static_cast<int64_t>(std::strtoll(value, nullptr, base));
#endif
    }

    template <> uint64_t Parse<uint64_t>(const char *value, int base)
    {
#if defined(GT_PLATFORM_WINDOWS)
        return static_cast<uint64_t>(_strtoui64(value, nullptr, base));
#else
        return static_cast<uint64_t>(std::strtoull(value, nullptr, base));
#endif
    }

    template <> double Parse<double>(const char *value)
    {
        return std::strtod(value, nullptr);
    }
}
