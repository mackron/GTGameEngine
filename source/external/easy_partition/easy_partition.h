// Version 0.1 - Public Domain. See "unlicense" statement at the end of this file.

// This library is actually a collection of smaller libraries - one for each type of partitioning data structure. Currently, this
// includes:
//   - easy_quadtree
//
// Plans for the future include
//   - easy_octree
//   - easy_bvh
//
// Also keep in mind that not too much effort has been put towards efficiency.

#ifndef easy_partition
#define easy_partition

#ifdef __cplusplus
extern "C" {
#endif


// Options. These should be self-explanatory.
#define EASYPARTITION_BUILD_QUADTREE    1
#define EASYPARTITION_BUILD_OCTREE      1   // Not yet implemented.
#define EASYPARTITION_BUILD_BVH         1   // Not yet implemented.


/// Specifies whether or not easyquadtree_malloc() and easyquadtree_free() should use the default implementation. Set this to 0 if you
/// would like to implement your own version of these functions.
#define EASYQUADTREE_USE_DEFAULT_ALLOCATOR   1

/// Specifies whether or not easyoctree_malloc() and easyoctree_free() should use their default implementations. If this is set to 0
/// it is up to the application to provide implementations of those functions.
#define EASYOCTREE_USE_DEFAULT_ALLOCATOR     1

/// Specifies whether or not easybvh_malloc() and easybvh_free() should use their default implementations. If this is set to 0
/// it is up to the application to provide implementations of those functions.
#define EASYBVH_USE_DEFAULT_ALLOCATOR        1


/////////////////////////////////////
// Quadtree
//
// Below is an implementation of a quadtree data structure. The smallest allowable size for a node is 1, and they are always square. Each
// quadtree node contains a single data pointer, so you probably want to wrap a higher level structure over this and pass in a pointer to
// a list of items or something.
//
// With the current implementation, if an insertion point falls exactly on the boundary of a node's boundary it will NOT be considered to be
// inside that node. Instead it will be part of the next higher level node.
#if defined(EASYPARTITION_BUILD_QUADTREE) && EASYPARTITION_BUILD_QUADTREE==1

typedef int easyquadtree_bool;

typedef struct easyquadtree       easyquadtree;
typedef struct easyquadtree_point easyquadtree_point;
typedef struct easyquadtree_aabb  easyquadtree_aabb;

typedef easyquadtree_bool (* easyquadtree_query_proc)(easyquadtree* pQuadtree);


/// malloc()
easyquadtree* easyquadtree_malloc(void);

/// free()
void easyquadtree_free(easyquadtree* pQuadtree);


#define EASYQUADTREE_NORTHWEST  0
#define EASYQUADTREE_NORTHEAST  1
#define EASYQUADTREE_SOUTHWEST  2
#define EASYQUADTREE_SOUTHEAST  3


struct easyquadtree_point
{
    /// The x position of the point.
    float x;

    /// The y position of the point.
    float y;

};

struct easyquadtree_aabb
{
    /// The center point of the AABB.
    easyquadtree_point center;

    /// The half width of the AABB.
    float halfWidth;

    /// The half height of the AABB.
    float halfHeight;
};

/// Determines whether or not the given AABB contains the given point.
easyquadtree_bool easyquadtree_aabb_contains_point(easyquadtree_aabb aabb, easyquadtree_point point);

/// Determines whether or not the given AABB wholly contains the given AABB.
easyquadtree_bool easyquadtree_aabb_contains_aabb(easyquadtree_aabb aabb, easyquadtree_aabb other);

/// Determines whether or not the given AABB intersects with another AABB.
easyquadtree_bool easyquadtree_aabb_intersects_aabb(easyquadtree_aabb aabb, easyquadtree_aabb other);


struct easyquadtree
{
    /// The bounds of this quadtree. This is always square (halfWidth == halfHeight).
    easyquadtree_aabb bounds;

    /// The four children. North-West, North-East, South-West, South-East.
    easyquadtree* children[4];

    /// The pointer that was passed into quadtree_insert() or quadtree_init().
    void* pUserData;
};

/// Creates a new quadtree node.
///
/// @param bounds    [in] The quadtree's bounds as a center/half AABB.
/// @param pUserData [in] A pointer to the user data that will be associated with the new node.
///
/// @remarks
///     This will not create child nodes.
easyquadtree* easyquadtree_create(easyquadtree_aabb bounds, void* pUserData);

/// Deletes a quadtree node that was created with easyquadtree_create().
///
/// @remarks
///     This will also delete the child nodes.
void easyquadtree_delete(easyquadtree* pQuadtree);

/// Initializes the given quadtree.
///
/// @remarks
///     This will fail if the bounds are smaller than 1.
easyquadtree_bool easyquadtree_init(easyquadtree* pQuadtree, easyquadtree_aabb bounds, void* pUserData);

/// Uninitializes the given quadtree.
///
/// @remarks
///     This will delete the child nodes.
void easyquadtree_uninit(easyquadtree* pQuadtree);


/// Calculates the bounds of each of the children of the given quadtree.
easyquadtree_bool easyquadtree_calculate_children_bounds(easyquadtree* pQuadtree, easyquadtree_aabb* pNWOut, easyquadtree_aabb* pNEOut, easyquadtree_aabb* pSWOut, easyquadtree_aabb* pSEOut);


/// Determines whether or not the given quadtree is large enough to be sub-divided.
easyquadtree_bool easyquadtree_can_subdivide(easyquadtree* pQuadtree);

/// Sub-divides the quadtree by creating all four of it's children.
///
/// @remarks
///     This will fail if the quadtree is equal to or smaller than 1x1.
///     @par
///     The user data pointer for each child will be set to null.
easyquadtree_bool easyquadtree_subdivide(easyquadtree* pQuadtree);

/// Recursively sub-divides the quadtree until the given bounds is fully contained within either the input quadtree or one of it's children.
///
/// @return A pointer to the quadtree node that full contains the given bounds.
easyquadtree* easyquadtree_subdivide_by_bounds(easyquadtree* pQuadtree, easyquadtree_aabb bounds);
easyquadtree* easyquadtree_subdivide_by_point(easyquadtree* pQuadtree, easyquadtree_point point);

/// Retrieves a pointer to the child quadtree node that fully contains the given bounds.
///
/// @remarks
///     This will not do any sub-divisions which means it will only look at existing nodes and may not return the tightest fitting node. Use
///     subdivide_by_bounds() to do sub-divisions until the tightest fitting node is found.
easyquadtree* easyquadtree_get_node_by_bounds(easyquadtree* pQuadtree, easyquadtree_aabb bounds);
easyquadtree* easyquadtree_get_node_by_point(easyquadtree* pQuadtree, easyquadtree_point point);


/// Iterates over every allocated quadtree node that intersects with the given bounds.
///
/// @param pQuadtree [in] A pointer to the root level quadtree node to query.
/// @param bounds    [in] The bounds to check against.
/// @param callback  [in] The function to call for every quadtree that is contained within the given bounds.
///
/// @remarks
///     When callback returns false, iteration will stop.
///     @par
///     The return value specifies whether or not the iteration completed organically or was forced to terminate with callback(). If a callback
///     returns false (any thus forcefully terminates the iteration), the return value will be false. An organic termination will return true.
easyquadtree_bool easyquadtree_query_range(easyquadtree* pQuadtree, easyquadtree_aabb bounds, easyquadtree_query_proc callback);

/// Iterates over every child of the given quadtree, including itself.
easyquadtree_bool easyquadtree_iterate_children_and_self(easyquadtree* pQuadtree, easyquadtree_query_proc callback);

#endif  // easy_quadtree





#ifdef __cplusplus
}
#endif

#endif

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
