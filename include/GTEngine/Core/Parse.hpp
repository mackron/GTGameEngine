// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Parse_hpp_
#define __GTLib_Parse_hpp_

#include <cassert>
#include <cstdint>

namespace GT
{
    template <typename T>
    T Parse(const char *, int)
    {
        assert(false);
        return T();
    }
    
    template <> int          Parse<int>(const char *value, int base);
    template <> unsigned int Parse<unsigned int>(const char *value, int base);
    template <> int64_t      Parse<int64_t>(const char *value, int base);
    template <> uint64_t     Parse<uint64_t>(const char *value, int base);
    
    /**
    *   \brief  Converts a string to another type.
    *
    *   \remarks
    *       For numerical types (int, double, etc), the parser will first trim the ends of whitespace. If any non-numerical
    *       characters exist after the initial number, they will be ignored, and that initial number will be returned. This
    *       is really useful if you have something like "100%" that needs to be converted.
    */
    template <typename T>
    T Parse(const char *)
    {
        assert(false);
        return T();
    }
    
    template <> double       Parse<double>(const char *value);
    template <> inline float Parse<float>(const char *value)
    {
        return (float)Parse<double>(value);
    }
    
    template <> inline int          Parse<int>(const char *value)              { return Parse<int>(value, 0); }
    template <> inline unsigned int Parse<unsigned int>(const char *value)     { return Parse<unsigned int>(value, 0); }
    template <> inline int64_t      Parse<int64_t>(const char *value)          { return Parse<int64_t>(value, 0); }
    template <> inline uint64_t     Parse<uint64_t>(const char *value)         { return Parse<uint64_t>(value, 0); }
}


#endif
