// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_AlignedType_hpp_
#define __GT_AlignedType_hpp_

#include <stdlib.h>

/// Base class for correctly aligning new operations.
template <int Alignment>
class AlignedType
{
public:

    void* operator new(size_t size)
    {
        return _aligned_malloc(size, Alignment);
    }
    void* operator new[](size_t size)
    {
        return _aligned_malloc(size, Alignment);
    }


    void operator delete(void* ptr, size_t)
    {
        _aligned_free(ptr);
    }
    void operator delete[](void* ptr, size_t)
    {
        _aligned_free(ptr);
    }
};


/// Base class for SIMD alignment.
class SIMDAlignedType : public AlignedType<16>
{
};


#endif // !__GT_AlignedType_hpp_
