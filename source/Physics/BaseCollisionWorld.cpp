// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Physics.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialiser list.
#endif

namespace GTEngine
{
    BaseCollisionWorld::BaseCollisionWorld()
        : configuration(),
          dispatcher(&configuration),
          broadphase()
    {
    }

    BaseCollisionWorld::~BaseCollisionWorld()
    {
    }


    void BaseCollisionWorld::AddCollisionObject(btCollisionObject &objIn, short int collisionFilterGroup, short int collisionFilterMask)
    {
        auto obj = dynamic_cast<CollisionObject*>(&objIn);
        if (obj != nullptr)
        {
            this->AddCollisionObject(*obj, collisionFilterGroup, collisionFilterMask);
        }
        else
        {
            this->GetInternalWorld().addCollisionObject(&objIn, collisionFilterGroup, collisionFilterMask);
        }
    }

    void BaseCollisionWorld::AddCollisionObject(CollisionObject &collisionObject, short int collisionFilterGroup, short int collisionFilterMask)
    {
        auto prevWorld = collisionObject.GetWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveCollisionObject(collisionObject);
        }

        collisionObject.SetWorld(this);
        this->GetInternalWorld().addCollisionObject(&collisionObject, collisionFilterGroup, collisionFilterMask);
    }


    void BaseCollisionWorld::RemoveCollisionObject(btCollisionObject &objIn)
    {
        auto obj = dynamic_cast<CollisionObject*>(&objIn);
        if (obj != nullptr)
        {
            this->RemoveCollisionObject(*obj);
        }
        else
        {
            this->GetInternalWorld().removeCollisionObject(&objIn);
        }
    }

    void BaseCollisionWorld::RemoveCollisionObject(CollisionObject &collisionObject)
    {
        collisionObject.SetWorld(nullptr);
        this->GetInternalWorld().removeCollisionObject(&collisionObject);
    }


    void BaseCollisionWorld::UpdateAABB(btCollisionObject &object)
    {
        this->GetInternalWorld().updateSingleAabb(&object);
    }

    void BaseCollisionWorld::UpdateAllAABBs()
    {
        this->GetInternalWorld().updateAabbs();
    }


    void BaseCollisionWorld::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, btCollisionWorld::RayResultCallback &resultCallback) const
    {
        resultCallback.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
        this->GetInternalWorld().rayTest(ToBulletVector3(rayStart), ToBulletVector3(rayEnd), resultCallback);
    }

    void BaseCollisionWorld::ContactTest(const btCollisionObject &object, btCollisionWorld::ContactResultCallback &resultCallback) const
    {
        const_cast<BaseCollisionWorld*>(this)->GetInternalWorld().contactTest(const_cast<btCollisionObject*>(&object), resultCallback);
    }

    void BaseCollisionWorld::ContactTest(const btCollisionObject &objectA, const btCollisionObject &objectB, btCollisionWorld::ContactResultCallback &resultCallback) const
    {
        const_cast<BaseCollisionWorld*>(this)->GetInternalWorld().contactPairTest(const_cast<btCollisionObject*>(&objectA), const_cast<btCollisionObject*>(&objectB), resultCallback);
    }

    void BaseCollisionWorld::ConvexSweepTest(const btConvexShape &shape, const glm::mat4 &from, const glm::mat4 &to, btCollisionWorld::ConvexResultCallback &resultCallback, float allowedCCDPenetration)
    {
        btTransform fromTransform = GTEngine::BulletUtils::CreateTransform(from);
        btTransform toTransform   = GTEngine::BulletUtils::CreateTransform(to);

        this->ConvexSweepTest(shape, fromTransform, toTransform, resultCallback, allowedCCDPenetration);
    }

    void BaseCollisionWorld::ConvexSweepTest(const btConvexShape &shape, const btTransform &from, const btTransform &to, btCollisionWorld::ConvexResultCallback &resultCallback, float allowedCCDPenetration)
    {
        this->GetInternalWorld().convexSweepTest(&shape, from, to, resultCallback, allowedCCDPenetration);
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif