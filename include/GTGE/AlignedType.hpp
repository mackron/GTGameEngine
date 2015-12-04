// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_AlignedType
#define GT_AlignedType

#include <stdlib.h>
#include <new>

#include <easy_util/easy_util.h>


/// Base class for correctly aligning new operations.
template <int Alignment>
class AlignedType
{
public:

    void* operator new(size_t size)
    {
        return aligned_malloc(Alignment, size);
    }
    void* operator new(size_t, void* ptr)
    {
        return ptr;
    }
    void* operator new(size_t size, const std::nothrow_t &)
    {
        return aligned_malloc(Alignment, size);
    }

    void* operator new[](size_t size)
    {
        return aligned_malloc(Alignment, size);
    }
    void* operator new[](size_t, void* ptr)
    {
        return ptr;
    }
    void* operator new[](size_t size, const std::nothrow_t &)
    {
        return aligned_malloc(Alignment, size);
    }


    void operator delete(void* ptr)
    {
        aligned_free(ptr);
    }
    void operator delete(void* ptr, const std::nothrow_t &)
    {
        aligned_free(ptr);
    }
    void operator delete(void* ptr, void*)
    {
        aligned_free(ptr);
    }

    void operator delete[](void* ptr)
    {
        aligned_free(ptr);
    }
    void operator delete[](void* ptr, const std::nothrow_t &)
    {
        aligned_free(ptr);
    }
    void operator delete[](void* ptr, void*)
    {
        aligned_free(ptr);
    }
};


/// Base class for SIMD alignment.
class SIMDAlignedType : public AlignedType<16>
{
};


#endif // !__GT_AlignedType_hpp_