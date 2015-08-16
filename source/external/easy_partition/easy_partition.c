// Version 0.1 - Public Domain. See "unlicense" statement at the end of this file.

#include "easy_partition.h"
#include <assert.h>

#if (defined(EASYQUADTREE_USE_DEFAULT_ALLOCATOR) && EASYQUADTREE_USE_DEFAULT_ALLOCATOR==1) || \
    (defined(EASYOCTREE_USE_DEFAULT_ALLOCATOR) && EASYOCTREE_USE_DEFAULT_ALLOCATOR==1)     || \
    (defined(EASYBVH_USE_DEFAULT_ALLOCATOR) && EASYBVH_USE_DEFAULT_ALLOCATOR==1)
#include <stdlib.h>
#endif


/////////////////////////////////////
// Quadtree
#if defined(EASYPARTITION_BUILD_QUADTREE) && EASYPARTITION_BUILD_QUADTREE==1

#if defined(EASYQUADTREE_USE_DEFAULT_ALLOCATOR) && EASYQUADTREE_USE_DEFAULT_ALLOCATOR==1
    easyquadtree* easyquadtree_malloc()
    {
        return malloc(sizeof(easyquadtree));
    }

    void easyquadtree_free(easyquadtree* pQuadtree)
    {
        free(pQuadtree);
    }
#endif


easyquadtree_bool easyquadtree_aabb_contains_point(easyquadtree_aabb aabb, easyquadtree_point point)
{
    if (point.x < aabb.center.x - aabb.halfWidth)  return 0;
    if (point.x > aabb.center.x + aabb.halfWidth)  return 0;
    if (point.y < aabb.center.y - aabb.halfHeight) return 0;
    if (point.y > aabb.center.y + aabb.halfHeight) return 0;

    return 1;
}

easyquadtree_bool easyquadtree_aabb_contains_aabb(easyquadtree_aabb aabb, easyquadtree_aabb other)
{
    const float otherLeft   = other.center.x - other.halfWidth;
    const float otherRight  = other.center.x + other.halfWidth;
    const float otherBottom = other.center.y - other.halfHeight;
    const float otherTop    = other.center.y + other.halfHeight;
    
    const float thisLeft    = aabb.center.x  - aabb.halfWidth;
    const float thisRight   = aabb.center.x  + aabb.halfWidth;
    const float thisBottom  = aabb.center.y  - aabb.halfHeight;
    const float thisTop     = aabb.center.y  + aabb.halfHeight;
    
    return
        otherLeft   >= thisLeft   &&
        otherRight  <= thisRight  &&
        otherBottom >= thisBottom &&
        otherTop    <= thisTop;
}

easyquadtree_bool easyquadtree_aabb_intersects_aabb(easyquadtree_aabb aabb, easyquadtree_aabb other)
{
    if (other.center.x + other.halfWidth  < aabb.center.x - aabb.halfWidth)  return 0;
    if (other.center.x - other.halfWidth  > aabb.center.x + aabb.halfWidth)  return 0;
    if (other.center.y + other.halfHeight < aabb.center.y - aabb.halfHeight) return 0;
    if (other.center.y - other.halfHeight > aabb.center.y + aabb.halfHeight) return 0;

    return 1;
}

easyquadtree* easyquadtree_create(easyquadtree_aabb bounds, void* pUserData)
{
    easyquadtree* pQuadtree = easyquadtree_malloc();
    if (pQuadtree != 0)
    {
        if (easyquadtree_init(pQuadtree, bounds, pUserData))
        {
            return pQuadtree;
        }
        else
        {
            easyquadtree_free(pQuadtree);
            pQuadtree = 0;
        }
    }

    return 0;
}

void easyquadtree_delete(easyquadtree* pQuadtree)
{
    if (pQuadtree != 0)
    {
        easyquadtree_uninit(pQuadtree);
        easyquadtree_free(pQuadtree);
    }
}

easyquadtree_bool easyquadtree_init(easyquadtree* pQuadtree, easyquadtree_aabb bounds, void* pUserData)
{
    if (pQuadtree != 0)
    {
        if (bounds.halfWidth == bounds.halfHeight)      // <-- Must be square.
        {
            pQuadtree->bounds = bounds;
            pQuadtree->children[0] = NULL;
            pQuadtree->children[1] = NULL;
            pQuadtree->children[2] = NULL;
            pQuadtree->children[3] = NULL;
            pQuadtree->pUserData = pUserData;

            return 1;
        }
    }

    return 0;
}

void easyquadtree_uninit(easyquadtree* pQuadtree)
{
    if (pQuadtree != 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            easyquadtree_delete(pQuadtree->children[i]);
            pQuadtree->children[i] = 0;
        }

        pQuadtree->pUserData = 0;
    }
}


easyquadtree_bool easyquadtree_calculate_children_bounds(easyquadtree* pQuadtree, easyquadtree_aabb* pNWOut, easyquadtree_aabb* pNEOut, easyquadtree_aabb* pSWOut, easyquadtree_aabb* pSEOut)
{
    if (pQuadtree != 0)
    {
        const float childrenOffset    = pQuadtree->bounds.halfWidth  * 0.5f;
        const float childrenHalfSize  = pQuadtree->bounds.halfHeight * 0.5f;
        const float childrenWestPosX  = pQuadtree->bounds.center.x - childrenOffset;
        const float childrenEastPosX  = pQuadtree->bounds.center.x + childrenOffset;
        const float childrenSouthPosY = pQuadtree->bounds.center.y - childrenOffset;
        const float childrenNorthPosY = pQuadtree->bounds.center.y + childrenOffset;

        if (pNWOut != 0)
        {
            pNWOut->center.x   = childrenWestPosX;
            pNWOut->center.y   = childrenNorthPosY;
            pNWOut->halfWidth  = childrenHalfSize;
            pNWOut->halfHeight = childrenHalfSize;
        }

        if (pNEOut != 0)
        {
            pNEOut->center.x   = childrenEastPosX;
            pNEOut->center.y   = childrenNorthPosY;
            pNEOut->halfWidth  = childrenHalfSize;
            pNEOut->halfHeight = childrenHalfSize;
        }

        if (pSWOut != 0)
        {
            pSWOut->center.x   = childrenWestPosX;
            pSWOut->center.y   = childrenSouthPosY;
            pSWOut->halfWidth  = childrenHalfSize;
            pSWOut->halfHeight = childrenHalfSize;
        }

        if (pSEOut != 0)
        {
            pSEOut->center.x   = childrenEastPosX;
            pSEOut->center.y   = childrenSouthPosY;
            pSEOut->halfWidth  = childrenHalfSize;
            pSEOut->halfHeight = childrenHalfSize;
        }

        return 1;
    }

    return 0;
}


easyquadtree_bool easyquadtree_can_subdivide(easyquadtree* pQuadtree)
{
    if (pQuadtree != 0)
    {
        // Minimum size is 1.
        if (pQuadtree->bounds.halfWidth > 0.5f)
        {
            // Cannot sub-divide if it is already sub-divided.
            if (pQuadtree->children[0] == 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

easyquadtree_bool easyquadtree_subdivide(easyquadtree* pQuadtree)
{
    if (easyquadtree_can_subdivide(pQuadtree))
    {
        easyquadtree_aabb boundsNW;
        easyquadtree_aabb boundsNE;
        easyquadtree_aabb boundsSW;
        easyquadtree_aabb boundsSE;
        if (easyquadtree_calculate_children_bounds(pQuadtree, &boundsNW, &boundsNE, &boundsSW, &boundsSE))
        {
            pQuadtree->children[EASYQUADTREE_NORTHWEST] = easyquadtree_create(boundsNW, 0);
            pQuadtree->children[EASYQUADTREE_NORTHEAST] = easyquadtree_create(boundsNE, 0);
            pQuadtree->children[EASYQUADTREE_SOUTHWEST] = easyquadtree_create(boundsSW, 0);
            pQuadtree->children[EASYQUADTREE_SOUTHEAST] = easyquadtree_create(boundsSE, 0);

            return 1;
        }
    }

    return 0;
}

easyquadtree* easyquadtree_subdivide_by_bounds(easyquadtree* pQuadtree, easyquadtree_aabb bounds)
{
    if (pQuadtree != 0)
    {
        if (easyquadtree_aabb_contains_aabb(pQuadtree->bounds, bounds))
        {
            // It's fully contained within this quadtree, so check it's children.

            easyquadtree_aabb boundsNW;
            easyquadtree_aabb boundsNE;
            easyquadtree_aabb boundsSW;
            easyquadtree_aabb boundsSE;
            if (easyquadtree_calculate_children_bounds(pQuadtree, &boundsNW, &boundsNE, &boundsSW, &boundsSE))
            {
                int childIndex = -1;

                // North-West
                if (easyquadtree_aabb_contains_aabb(boundsNW, bounds))
                {
                    childIndex = EASYQUADTREE_NORTHWEST;
                }

                // North-East
                if (easyquadtree_aabb_contains_aabb(boundsNE, bounds))
                {
                    childIndex = EASYQUADTREE_NORTHEAST;
                }

                // South-West
                if (easyquadtree_aabb_contains_aabb(boundsSW, bounds))
                {
                    childIndex = EASYQUADTREE_SOUTHWEST;
                }

                // South-East
                if (easyquadtree_aabb_contains_aabb(boundsSE, bounds))
                {
                    childIndex = EASYQUADTREE_SOUTHEAST;
                }


                if (childIndex != -1)
                {
                    // The quadtree may need to be sub-divided.
                    if (pQuadtree->children[childIndex] == 0)
                    {
                        if (!easyquadtree_subdivide(pQuadtree))
                        {
                            // Failed to sub-divide. It could be that the quadtree is already at the minimum size.
                            return pQuadtree;
                        }
                    }


                    assert(pQuadtree->children[childIndex] != 0);

                    easyquadtree* pSubQuadtree = easyquadtree_subdivide_by_bounds(pQuadtree->children[childIndex], bounds);
                    if (pSubQuadtree != 0)
                    {
                        return pSubQuadtree;
                    }
                }
            }

            return pQuadtree;
        }
    }

    return 0;
}

easyquadtree* easyquadtree_subdivide_by_point(easyquadtree* pQuadtree, easyquadtree_point point)
{
    easyquadtree_aabb bounds;
    bounds.center     = point;
    bounds.halfWidth  = 0;
    bounds.halfHeight = 0;
    return easyquadtree_subdivide_by_bounds(pQuadtree, bounds);
}

easyquadtree* easyquadtree_get_node_by_bounds(easyquadtree* pQuadtree, easyquadtree_aabb bounds)
{
    if (pQuadtree != 0)
    {
        if (easyquadtree_aabb_contains_aabb(pQuadtree->bounds, bounds))
        {
            // It's fully contained within this quadtree, so check it's children.
            easyquadtree* pSubQuadtree = 0;

            pSubQuadtree = easyquadtree_get_node_by_bounds(pQuadtree->children[EASYQUADTREE_NORTHWEST], bounds);
            if (pSubQuadtree != 0) {
                return pSubQuadtree;
            }

            pSubQuadtree = easyquadtree_get_node_by_bounds(pQuadtree->children[EASYQUADTREE_NORTHEAST], bounds);
            if (pSubQuadtree != 0) {
                return pSubQuadtree;
            }

            pSubQuadtree = easyquadtree_get_node_by_bounds(pQuadtree->children[EASYQUADTREE_SOUTHWEST], bounds);
            if (pSubQuadtree != 0) {
                return pSubQuadtree;
            }

            pSubQuadtree = easyquadtree_get_node_by_bounds(pQuadtree->children[EASYQUADTREE_SOUTHEAST], bounds);
            if (pSubQuadtree != 0) {
                return pSubQuadtree;
            }


            // If we get here it means none of the children contains the bounds.
            return pQuadtree;
        }
    }

    return 0;
}

easyquadtree* easyquadtree_get_node_by_point(easyquadtree* pQuadtree, easyquadtree_point point)
{
    easyquadtree_aabb bounds;
    bounds.center     = point;
    bounds.halfWidth  = 0;
    bounds.halfHeight = 0;
    return easyquadtree_get_node_by_bounds(pQuadtree, bounds);
}


easyquadtree_bool easyquadtree_query_range(easyquadtree* pQuadtree, easyquadtree_aabb bounds, easyquadtree_query_proc callback)
{
    if (pQuadtree != 0 && callback != 0)
    {
        if (easyquadtree_aabb_contains_aabb(pQuadtree->bounds, bounds))
        {
            if (callback(pQuadtree))
            {
                return
                    easyquadtree_query_range(pQuadtree->children[EASYQUADTREE_NORTHWEST], bounds, callback) &&
                    easyquadtree_query_range(pQuadtree->children[EASYQUADTREE_NORTHEAST], bounds, callback) &&
                    easyquadtree_query_range(pQuadtree->children[EASYQUADTREE_SOUTHWEST], bounds, callback) &&
                    easyquadtree_query_range(pQuadtree->children[EASYQUADTREE_SOUTHEAST], bounds, callback);
            }
            else
            {
                // Forcefully terminated with callback()
                return 0;
            }
        }
    }

    // Terminated organically.
    return 1;
}

easyquadtree_bool easyquadtree_iterate_children_and_self(easyquadtree* pQuadtree, easyquadtree_query_proc callback)
{
    if (pQuadtree != 0 && callback != 0)
    {
        if (callback(pQuadtree))
        {
            return
                easyquadtree_iterate_children_and_self(pQuadtree->children[EASYQUADTREE_NORTHWEST], callback) &&
                easyquadtree_iterate_children_and_self(pQuadtree->children[EASYQUADTREE_NORTHEAST], callback) &&
                easyquadtree_iterate_children_and_self(pQuadtree->children[EASYQUADTREE_SOUTHWEST], callback) &&
                easyquadtree_iterate_children_and_self(pQuadtree->children[EASYQUADTREE_SOUTHEAST], callback);
        }
        else
        {
            // Forcefully terminated with callback()
            return 0;
        }
    }

    // Terminated organically.
    return 1;
}

#endif  // easy_quadtree




/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/