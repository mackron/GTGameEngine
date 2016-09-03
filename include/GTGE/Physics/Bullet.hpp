// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Physics_BulletDynamics
#define GT_Physics_BulletDynamics

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4127)   // Conditional expression is constant.
    #pragma warning(disable:4100)   // Unreferenced formal parameter.
    #pragma warning(disable:4324)   // structure was padded due to __declspec(align())
    #pragma warning(disable:4456)   // declaration of '...' hides previous local declaration
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wuninitialized"
    #if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7))
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    #endif
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wcast-align"
        #pragma GCC diagnostic ignored "-Woverloaded-virtual"
        #pragma GCC diagnostic ignored "-Wunreachable-code"
    #endif
#endif
#include "../source/external/bullet3/src/BulletCollision/Gimpact/btBoxCollision.h"                  // <-- for btAABB.
#include "../source/external/bullet3/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionDispatch/btGhostObject.h"

#include "../source/external/bullet3/src/BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.h"

#include "../source/external/bullet3/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "../source/external/bullet3/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btBoxShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btSphereShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btConeShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btCylinderShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btCompoundShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "../source/external/bullet3/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "../source/external/bullet3/src/BulletCollision/Gimpact/btGImpactShape.h"

#include "../source/external/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "../source/external/bullet3/src/BulletDynamics/Dynamics/btRigidBody.h"
#include "../source/external/bullet3/src/BulletDynamics/Character/btKinematicCharacterController.h"

#include "../source/external/bullet3/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "../source/external/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"
#include "../source/external/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h"
#include "../source/external/bullet3/src/BulletDynamics/ConstraintSolver/btHingeConstraint.h"
#include "../source/external/bullet3/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.h"
#include "../source/external/bullet3/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "../source/external/bullet3/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.h"


//#include <btBulletDynamicsCommon.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


#endif
