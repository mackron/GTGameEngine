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
#include "BulletCollision/Gimpact/btBoxCollision.h"                  // <-- for btAABB.
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"

#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btConeShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"
#include "BulletDynamics/ConstraintSolver/btHinge2Constraint.h"
#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "BulletDynamics/ConstraintSolver/btConeTwistConstraint.h"


//#include <btBulletDynamicsCommon.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


#endif
