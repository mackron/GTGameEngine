// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/Parse.hpp>
#include <GTGE/Core/BasicBuffer.hpp>
#include <GTGE/Core/stdlib.hpp>

#include <cstring>

namespace GT
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
#if defined(_WIN32)
        return static_cast<int64_t>(_strtoi64(value, nullptr, base));
#else
        return static_cast<int64_t>(std::strtoll(value, nullptr, base));
#endif
    }

    template <> uint64_t Parse<uint64_t>(const char *value, int base)
    {
#if defined(_WIN32)
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
