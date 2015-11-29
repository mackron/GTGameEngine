// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Limits
#define GT_Limits

#include <cstdint>

namespace GT
{
    template <typename T>
    T MinValue() { return 0; }

    template<> inline int8_t   MinValue() { return (-127 - 1); }
    template<> inline int16_t  MinValue() { return (-32767 - 1); }
    template<> inline int32_t  MinValue() { return (-2147483647 - 1); }
    template<> inline int64_t  MinValue() { return (-9223372036854775807 - 1); }
    template<> inline uint8_t  MinValue() { return 0; }
    template<> inline uint16_t MinValue() { return 0; }
    template<> inline uint32_t MinValue() { return 0; }
    template<> inline uint64_t MinValue() { return 0; }


    template <typename T> 
    T MaxValue() { return 0; }

    template<> inline int8_t   MaxValue() { return 0x7f; }
    template<> inline int16_t  MaxValue() { return 0x7fff; } 
    template<> inline int32_t  MaxValue() { return 0x7fffffff; }
    template<> inline int64_t  MaxValue() { return 0x7fffffffffffffff; }
    template<> inline uint8_t  MaxValue() { return 0xff; }
    template<> inline uint16_t MaxValue() { return 0xffff; } 
    template<> inline uint32_t MaxValue() { return 0xffffffff; }
    template<> inline uint64_t MaxValue() { return 0xffffffffffffffff; }
}

#endif
