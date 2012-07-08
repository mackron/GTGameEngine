
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
        auto prevWorld = collisionObject.getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveCollisionObject(collisionObject);
        }

        collisionObject.setWorld(this);
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
        collisionObject.setWorld(nullptr);
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
        this->GetInternalWorld().rayTest(ToBulletVector3(rayStart), ToBulletVector3(rayEnd), resultCallback);
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif