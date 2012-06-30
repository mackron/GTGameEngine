
#ifndef __GTEngine_Physics_BulletDynamics_hpp_
#define __GTEngine_Physics_BulletDynamics_hpp_

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4127)   // Conditional expression is constant.
    #pragma warning(disable:4100)   // Unreferenced formal parameter.
    #pragma warning(disable:4324)   // structure was padded due to __declspec(align())
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <BulletCollision/Gimpact/btBoxCollision.h>                  // <-- for btAABB.
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btConeShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>

//#include <btBulletDynamicsCommon.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
#endif


#endif
