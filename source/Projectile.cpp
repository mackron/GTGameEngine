// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Projectile.hpp>

namespace GTEngine
{
    Projectile::Projectile()
        : position(), velocity()
    {
    }

    Projectile::~Projectile()
    {
    }


    void Projectile::Launch(const glm::vec3 &positionIn, const glm::vec3 &velocityIn)
    {
        this->position = positionIn;
        this->velocity = velocityIn;

        this->OnLaunch();
    }

    void Projectile::Update(double dtSeconds)
    {
        float dtSecondsF = static_cast<float>(dtSeconds);

        glm::vec3 targetPosition = this->position + (this->velocity * dtSecondsF);
        this->OnUpdate(dtSeconds, targetPosition);

        // We now have the new position of the projectile. We'll need to now set a few properties.
        this->velocity = (targetPosition - this->position) / dtSecondsF;
        this->position = targetPosition;


        // Now we need to modify our velocity based on gravity and acceleration. It's important that we call GetGravity() and GetAcceleration()
        // after updating so that a sub-class has a change to modify those values if need be.
        glm::vec3 gravity;
        this->GetGravity(gravity);

        glm::vec3 acceleration;
        this->GetAcceleration(acceleration);

        this->velocity += gravity      * dtSecondsF;
        this->velocity += acceleration * dtSecondsF;
    }





    ///////////////////////////////////////////
    // Virtual Methods

    void Projectile::OnUpdate(double deltaTimeInSeconds, glm::vec3 &targetPosition)
    {
        (void)deltaTimeInSeconds;
        (void)targetPosition;
    }

    void Projectile::OnLaunch()
    {
    }

    void Projectile::GetGravity(glm::vec3 &gravity) const
    {
        gravity.x = 0.0f;
        gravity.y = 0.0f;
        gravity.z = 0.0f;
    }

    void Projectile::GetAcceleration(glm::vec3 &acceleration) const
    {
        acceleration.x = 0.0f;
        acceleration.y = 0.0f;
        acceleration.z = 0.0f;
    }
}


#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    DefaultProjectile::DefaultProjectile(Scene &sceneIn, short collisionGroupIn, short collisionMaskIn)
        : scene(sceneIn), collisionGroup(collisionGroupIn), collisionMask(collisionMaskIn),
          acceleration(),
          gravityFactor(1.0f, 1.0f, 1.0f),
          ignoredNode(nullptr)
    {
    }

    DefaultProjectile::~DefaultProjectile()
    {
    }

    void DefaultProjectile::SetCollisionFilter(short group, short mask)
    {
        this->collisionGroup = group;
        this->collisionMask  = mask;
    }

    void DefaultProjectile::SetAcceleration(const glm::vec3 &accelerationIn)
    {
        this->acceleration = accelerationIn;
    }

    void DefaultProjectile::SetGravityFactor(const glm::vec3 &factor)
    {
        this->gravityFactor = factor;
    }

    void DefaultProjectile::SetIgnoredNode(SceneNode* ignoredNodeIn)
    {
        this->ignoredNode = ignoredNodeIn;
    }


    /////////////////////////////////////////////////////////
    // Virtual Methods

    struct DefaultProjectileRayTestCallback : public ClosestRayTestCallback
    {
        GTEngine::SceneNode* ignoredObject;

        DefaultProjectileRayTestCallback(short collisionGroup, short collisionMask, SceneNode* ignoredObjectIn)
            : ClosestRayTestCallback(collisionGroup, collisionMask), ignoredObject(ignoredObjectIn)
        {
        }

        bool NeedsCollision(short collisionGroupIn, short collisionMaskIn, SceneNode &object) const
        {
            if (&object == this->ignoredObject)
            {
                return false;
            }
            else
            {
                return ClosestRayTestCallback::NeedsCollision(collisionGroupIn, collisionMaskIn, object);
            }
        }


    private:    // No copying.
        DefaultProjectileRayTestCallback(const DefaultProjectileRayTestCallback &);
        DefaultProjectileRayTestCallback & operator=(const DefaultProjectileRayTestCallback &);
    };


    void DefaultProjectile::OnUpdate(double deltaTimeInSeconds, glm::vec3 &targetPosition)
    {
        (void)deltaTimeInSeconds;

        // We need to cast a ray between the current position and the target. If we collide with anything, we need to call OnCollide().
        DefaultProjectileRayTestCallback rayTestCallback(this->collisionGroup, this->collisionMask, this->ignoredNode);
        auto node = this->scene.RayTest(this->position, targetPosition, rayTestCallback);

        if (node != nullptr)
        {
            this->OnCollide(*node, rayTestCallback);
        }
    }

    void DefaultProjectile::GetGravity(glm::vec3 &gravityOut) const
    {
        this->scene.GetGravity(gravityOut);

        gravityOut *= this->gravityFactor;
    }

    void DefaultProjectile::GetAcceleration(glm::vec3 &accelerationOut) const
    {
        accelerationOut = this->acceleration;
    }
}
