// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Physics
#define GT_Physics

#include "Physics/CollisionWorld.hpp"
#include "Physics/DynamicsWorld.hpp"
#include "Physics/SceneNodeMotionState.hpp"
#include "Physics/btEllipsoidShape.hpp"
#include "Physics/StaticMeshCollisionShape.hpp"
#include "Physics/MeshCollisionShape.hpp"
#include "Physics/CollisionShapeTypes.hpp"
#include "Math.hpp"

namespace GT
{
    inline btVector3 ToBulletVector3(const glm::vec3 &v)
    {
        return btVector3(v.x, v.y, v.z);
    }

    inline btTransform ToBulletTransform(const glm::mat4 &m)
    {
        btTransform result;
        result.setFromOpenGLMatrix(&m[0][0]);

        return result;
    }


    inline glm::vec3 ToGLMVector3(const btVector3 &v)
    {
        return glm::vec3(v.x(), v.y(), v.z());
    }

    inline glm::quat ToGLMQuaternion(const btQuaternion &v)
    {
        return glm::quat(v.w(), v.x(), v.y(), v.z());
    }

    inline glm::mat4 ToGLMMatrix4(const btTransform &t)
    {
        glm::mat4 result;
        t.getOpenGLMatrix(&result[0][0]);

        return result;
    }


    namespace BulletUtils
    {
        /**
        *   \brief  Helper for calculating the local inertia for a shape.
        */
        inline btVector3 CalculateLocalInertia(btScalar mass, const btCollisionShape &collisionShape)
        {
            // No mass = no inertia. Looking through the Bullet code shows that this check is not performed in the different
            // implementations of calculatelocalInertia(). I like it, so I'll do it here.
            if (mass > 0.0f)
            {
                btVector3 localInertia;
                collisionShape.calculateLocalInertia(mass, localInertia);

                return localInertia;
            }

            return btVector3(0.0f, 0.0f, 0.0f);
        }

        /**
        *   \brief  Creates a btTransform object from a glm::mat4.
        */
        inline btTransform CreateTransform(const glm::mat4 &transform)
        {
            btTransform result;
            result.setFromOpenGLMatrix(&transform[0][0]);

            return result;
        }
    }
}


#endif