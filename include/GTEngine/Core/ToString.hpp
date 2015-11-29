// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_ToString_hpp_
#define __GTLib_ToString_hpp_

#include <cstdint>
#include "String.hpp"

namespace GT
{
    String ToString(int64_t value);
    inline String ToString(int32_t value) { return ToString(static_cast<int64_t>(value)); }
    inline String ToString(int16_t value) { return ToString(static_cast<int64_t>(value)); }
    inline String ToString(int8_t  value) { return ToString(static_cast<int64_t>(value)); }
    
    String ToString(uint64_t value);
    inline String ToString(uint32_t value) { return ToString(static_cast<uint64_t>(value)); }
    inline String ToString(uint16_t value) { return ToString(static_cast<uint64_t>(value)); }
    inline String ToString(uint8_t  value) { return ToString(static_cast<uint64_t>(value)); }


    /**
    *   \brief              Converts a double to a string.
    *   \param  format [in] The format of the value. The format should be the same as would be given to sprintf(), but without the leading '%' and trailing 'g' or 'f'.
    *
    *   \remarks
    *       The format can not be more than 16 characters. The string will never be converted to scientific notation. It uses the 'f' qualifier.
    */
    String ToString(double value, const char *format = nullptr);
    String ToString(float value,  const char *format = nullptr);
}


#endif
