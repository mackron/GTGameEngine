
#ifndef __GTEngine_VertexArrayFactory_hpp_
#define __GTEngine_VertexArrayFactory_hpp_

#include "Rendering/VertexArray.hpp"
#include "Physics/Bullet.hpp"           // For btAABB. Might add a proper AABB header for this...

namespace GTEngine
{
    /**
    *   \brief  Static class for creating vertex arrays.
    */
    class VertexArrayFactory
    {
    // Creating.
    public:

        /**
        *   \brief  Creates a vertex array from an AABB.
        */
        static VertexArray* CreateWireframeFromAABB(const btAABB &aabb);

        /**
        *   \brief              Creates a flat on the XZ axis, facing +Y.
        *   \param  width  [in] The width of the plane.
        *   \param  height [in] The height of the plane.
        *
        *   \remarks
        *       The format of this plane is 3xPosition, 2xTexCoord and 3xNormal (P3T2N3).
        *       \par
        *       This is a single-sided plane.
        */
        static VertexArray* CreatePlaneXZ(float width, float height, VertexFormat &format = VertexFormat::P3T2N3);
        static VertexArray* CreatePlaneXY(float width, float height, VertexFormat &format = VertexFormat::P3T2N3);


    // Deleting.
    public:

        /**
        *   \brief  Deletes a vertex array that was created from this class.
        */
        static void DeleteVertexArray(VertexArray * vaToDelete);

    };
}

#endif