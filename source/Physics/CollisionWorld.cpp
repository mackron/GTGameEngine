
#include <GTEngine/Physics.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialiser list.
#endif

namespace GTEngine
{
    CollisionWorld::CollisionWorld()
        : btDefaultCollisionConfiguration(),
          btCollisionDispatcher(this),
          btDbvtBroadphase(),
          btCollisionWorld(this, this, this)
    {
    }

    CollisionWorld::~CollisionWorld()
    {
        // Now we need to remove all of our rigid bodies. We go backwards here to avoid reshuffling of the internal buffer.
        for (int i = this->getNumCollisionObjects() - 1; i >= 0; --i)
        {
            this->removeCollisionObject(this->getCollisionObjectArray()[i]);
        }
    }

    void CollisionWorld::rayTest(const glm::vec3 &rayFromWorld, const glm::vec3 &rayToWorld, RayResultCallback &resultCallback) const
    {
        btCollisionWorld::rayTest(ToBulletVector3(rayFromWorld), ToBulletVector3(rayToWorld), resultCallback);
    }

    void CollisionWorld::addCollisionObject(CollisionObject *collisionObject, short int collisionFilterGroup, short int collisionFilterMask)
    {
        auto prevWorld = collisionObject->getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->removeCollisionObject(collisionObject);
        }

        collisionObject->setWorld(this);
        btCollisionWorld::addCollisionObject(collisionObject, collisionFilterGroup, collisionFilterMask);
    }

    void CollisionWorld::removeCollisionObject(CollisionObject* collisionObject)
    {
        collisionObject->setWorld(nullptr);
        btCollisionWorld::removeCollisionObject(collisionObject);
    }


    void CollisionWorld::addCollisionObject(btCollisionObject *objIn, short int collisionFilterGroup, short int collisionFilterMask)
    {
        auto obj = dynamic_cast<CollisionObject *>(objIn);
        if (obj != nullptr)
        {
            this->addCollisionObject(obj, collisionFilterGroup, collisionFilterMask);
        }
        else
        {
            btCollisionWorld::addCollisionObject(objIn, collisionFilterGroup, collisionFilterMask);
        }
    }

    void CollisionWorld::removeCollisionObject(btCollisionObject* objIn)
    {
        auto obj = dynamic_cast<CollisionObject *>(objIn);
        if (obj != nullptr)
        {
            this->removeCollisionObject(obj);
        }
        else
        {
            btCollisionWorld::removeCollisionObject(objIn);
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
