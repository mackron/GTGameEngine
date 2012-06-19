
#ifndef __GTEngine_Physics_CollisionWorld_hpp_
#define __GTEngine_Physics_CollisionWorld_hpp_

#include "Bullet.hpp"
#include "CollisionObject.hpp"
#include "../Math.hpp"

// I don't like it, but there are some warnings about overloaded virtual functions that need to be hidden.
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

namespace GTEngine
{
    /**
    *   \brief  An simplified collision world for physics.
    *
    *   \remarks
    *       In order to inherit directly from btCollisionWorld, we need to use multiple inheritance. It doesn't make logical sense to do this,
    *       but I think it's best that our own world uses an 'is-a' relationship instead of a 'has-a' relationship with btCollisionWorld.
    *       \par
    *       Some methods may be ambiguous (rayTest(), for example). Where possible, this class will implement unambiguous versions of these
    *       methods. Sometimes it may not be possible. For example, rayTest() is virtual in btCollisionWorld, and it is also called recursively,
    *       meaning implementing our own version will cause an infinite loop.
    */
    ATTRIBUTE_ALIGNED16(class) CollisionWorld : public btDefaultCollisionConfiguration, public btCollisionDispatcher, public btDbvtBroadphase, public btCollisionWorld
    {
    public:

        /**
        *   \brief  Constructor.
        */
        CollisionWorld();

        /**
        *   \brief  Destructor.
        */
        virtual ~CollisionWorld();


    // Custom functions for simplifying some stuff.
    public:

        /// Wrapper around rayTest() for use with glm::vec3.
        void rayTest(const glm::vec3 &rayFromWorld, const glm::vec3 &rayToWorld, RayResultCallback &resultCallback) const;

	    virtual void addCollisionObject(CollisionObject* collisionObject, short int collisionFilterGroup = btBroadphaseProxy::DefaultFilter, short int collisionFilterMask = btBroadphaseProxy::AllFilter);
        virtual void removeCollisionObject(CollisionObject* collisionObject);

        virtual void addCollisionObject(btCollisionObject* collisionObject, short int collisionFilterGroup = btBroadphaseProxy::DefaultFilter, short int collisionFilterMask = btBroadphaseProxy::AllFilter);
        virtual void removeCollisionObject(btCollisionObject* collisionObject);


    // The functions below are ambiguous from inheritance. We do our own versions here.
    public:
    };
}

#if defined(__GNUC__)
    #pragma GCC diagnostic push
#endif

#endif