// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_CollisionShapeTypes_hpp_
#define __GTEngine_CollisionShapeTypes_hpp_

#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

namespace GT
{
    enum CollisionShapeType
    {
        CollisionShapeType_None             = 0,
        CollisionShapeType_Box              = 1,
        CollisionShapeType_Sphere           = 2,
        CollisionShapeType_Ellipsoid        = 3,
        CollisionShapeType_CylinderX        = 4,
        CollisionShapeType_CylinderY        = 5,
        CollisionShapeType_CylinderZ        = 6,
        CollisionShapeType_CapsuleX         = 7,
        CollisionShapeType_CapsuleY         = 8,
        CollisionShapeType_CapsuleZ         = 9,
        CollisionShapeType_ConvexHull       = 10,
        CollisionShapeType_ModelConvexHulls = 11,
    };

    inline CollisionShapeType GetCollisionShapeType(const btCollisionShape* shape)
    {
        if (shape != nullptr)
        {
            switch (shape->getShapeType())
            {
            case BOX_SHAPE_PROXYTYPE:
                {
                    return CollisionShapeType_Box;
                }

            case SPHERE_SHAPE_PROXYTYPE:
                {
                    return CollisionShapeType_Sphere;
                }

            case CUSTOM_CONVEX_SHAPE_TYPE:
                {
                    return CollisionShapeType_Ellipsoid;
                }

            case CYLINDER_SHAPE_PROXYTYPE:
                {
                    int upAxis = static_cast<const btCylinderShape*>(shape)->getUpAxis();

                    if (upAxis == 0)
                    {
                        return CollisionShapeType_CylinderX;
                    }
                    else if (upAxis == 2)
                    {
                        return CollisionShapeType_CylinderZ;
                    }

                    return CollisionShapeType_CylinderY;
                }

            case CAPSULE_SHAPE_PROXYTYPE:
                {
                    int upAxis = static_cast<const btCapsuleShape*>(shape)->getUpAxis();

                    if (upAxis == 0)
                    {
                        return CollisionShapeType_CapsuleX;
                    }
                    else if (upAxis == 2)
                    {
                        return CollisionShapeType_CapsuleZ;
                    }

                    return CollisionShapeType_CapsuleY;
                }

            case CONVEX_HULL_SHAPE_PROXYTYPE:
                {
                    return CollisionShapeType_ConvexHull;
                }

            case COMPOUND_SHAPE_PROXYTYPE:                  // <-- Will be the model convex hulls.
                {
                    return CollisionShapeType_ModelConvexHulls;
                }


            default:
                {
                    break;
                }
            }
        }

        return CollisionShapeType_None;
    }

    inline CollisionShapeType GetCollisionShapeType(const btCollisionShape &shape)
    {
        return GetCollisionShapeType(&shape);
    }
}

#endif