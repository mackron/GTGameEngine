
#ifndef __GTEngine_AABB_hpp_
#define __GTEngine_AABB_hpp_

/// The AABB class is actually just the btAABB class. Might as well repurpose it.
#include <BulletCollision/Gimpact/btBoxCollision.h>

namespace GTEngine
{
    typedef btAABB AABB;
}

#endif