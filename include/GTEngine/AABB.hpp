// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_AABB
#define GT_AABB

/// The AABB class is actually just the btAABB class. Might as well repurpose it.
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
    #if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7))
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    #endif
#endif

#include <BulletCollision/Gimpact/btBoxCollision.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

namespace GT
{
    typedef btAABB AABB;
}

#endif
