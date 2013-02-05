// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_AABB_hpp_
#define __GTEngine_AABB_hpp_

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

namespace GTEngine
{
    typedef btAABB AABB;
}

#endif
