
#include <GTEngine/Scene.hpp>
#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/DefaultSceneCullingManager.hpp>


namespace GTEngine
{
    struct SceneContactTestCallback : public btCollisionWorld::ContactResultCallback
    {
        ContactTestCallback &callback;


        SceneContactTestCallback(ContactTestCallback &callback)
            : callback(callback)
        {
            this->m_collisionFilterGroup = callback.collisionGroup;
            this->m_collisionFilterMask  = callback.collisionMask;
        }

        virtual bool needsCollision(btBroadphaseProxy *proxy0) const
		{
            auto collisionObject = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            assert(collisionObject != nullptr);

            auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                return this->callback.NeedsCollision(proxy0->m_collisionFilterGroup, proxy0->m_collisionFilterMask, *sceneNode);
            }

            return false;
		}

        virtual	btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper* colObj0, int, int, const btCollisionObjectWrapper* colObj1, int, int)
        {
            assert(colObj0 != nullptr);
            assert(colObj1 != nullptr);

            // We assume the user pointer is the scene node.
            auto sceneNodeA = static_cast<SceneNode*>(colObj0->getCollisionObject()->getUserPointer());
            auto sceneNodeB = static_cast<SceneNode*>(colObj1->getCollisionObject()->getUserPointer());

            if (sceneNodeA != nullptr && sceneNodeB != nullptr)
            {
                callback.ProcessCollision(*sceneNodeA, *sceneNodeB, cp);
            }

            return 0.0f;
        }


    private:
        SceneContactTestCallback(const SceneContactTestCallback &);
        SceneContactTestCallback & operator=(const SceneContactTestCallback &);
    };



    struct SceneBulletRayResultCallback : public btCollisionWorld::RayResultCallback
    {
        /// A reference to the GTEngine ray test callback object that is used for input and output.
        RayTestCallback &callback;

        /// A pointer to the closest scene node.
        SceneNode* closestSceneNode;


        /// Constructor.
        SceneBulletRayResultCallback(RayTestCallback &callback)
            : callback(callback), closestSceneNode(nullptr)
        {
            this->m_collisionFilterGroup = callback.collisionGroup;
            this->m_collisionFilterMask  = callback.collisionMask;
        }

        virtual bool needsCollision(btBroadphaseProxy* proxy0) const
		{
            auto collisionObject = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            assert(collisionObject != nullptr);

            auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                return this->callback.NeedsCollision(proxy0->m_collisionFilterGroup, proxy0->m_collisionFilterMask, *sceneNode);
            }

            return false;
		}

        virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace)
		{
            // The user data of the collision object should be a scene node.
            auto sceneNode = static_cast<SceneNode*>(rayResult.m_collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                this->closestSceneNode = sceneNode;

                // Some stuff for Bullet.
                this->m_closestHitFraction = rayResult.m_hitFraction;
                this->m_collisionObject    = rayResult.m_collisionObject;

                // Now we need to find the world position and normal.
                glm::vec3 worldHitPosition = glm::mix(callback.rayStart, callback.rayEnd, rayResult.m_hitFraction);
                glm::vec3 worldHitNormal;

                if (normalInWorldSpace)
                {
                    worldHitNormal = ToGLMVector3(rayResult.m_hitNormalLocal);
                }
                else
                {
                    worldHitNormal = ToGLMVector3(rayResult.m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal);
                }

                // Now we can call the corresponding handler on the GTEngine callback.
                this->callback.ProcessResult(*sceneNode, worldHitPosition, worldHitNormal);
            }

            return this->m_closestHitFraction;
		}


    private:    // No copying.
        SceneBulletRayResultCallback(const SceneBulletRayResultCallback &);
        SceneBulletRayResultCallback & operator=(const SceneBulletRayResultCallback &);
    };
}


namespace GTEngine
{
    Scene::Scene()
        : updateManager(*new DefaultSceneUpdateManager), physicsManager(*new DefaultScenePhysicsManager), cullingManager(*new DefaultSceneCullingManager),
          deleteUpdateManager(true), deletePhysicsManager(true), deleteCullingManager(true),
          paused(false)
    {
    }

    Scene::Scene(SceneUpdateManager &updateManagerIn, ScenePhysicsManager &physicsManagerIn, SceneCullingManager &cullingManagerIn)
        : updateManager(updateManagerIn), physicsManager(physicsManagerIn), cullingManager(cullingManagerIn),
          deleteUpdateManager(false), deletePhysicsManager(false), deleteCullingManager(false),
          paused(false)
    {
    }

    Scene::~Scene()
    {
        if (deleteUpdateManager)
        {
            delete &this->updateManager;
        }

        if (deletePhysicsManager)
        {
            delete &this->physicsManager;
        }

        if (deleteCullingManager)
        {
            delete &this->cullingManager;
        }
    }

    void Scene::AddObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            this->AddSceneNode(static_cast<SceneNode &>(object));
        }
    }

    void Scene::RemoveObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            this->RemoveSceneNode(static_cast<SceneNode &>(object));
        }
    }


    void Scene::RefreshObject(SceneObject &object)
    {
        this->RemoveObject(object);
        this->AddObject(object);
    }

    void Scene::Pause()
    {
        if (!this->paused)
        {
            this->paused = true;
            this->OnPause();
        }
    }

    void Scene::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
            this->OnResume();
        }
    }


    SceneNode* Scene::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback)
    {
        // Before creating the Bullet ray test callback object, we first need to set the rayStart and rayEnd attributes of the input callback structure. We do this
        // because the Bullet callback will use them in calculating the world position of interestion points.
        callback.rayStart = rayStart;
        callback.rayEnd   = rayEnd;

        // We need to use our own ray test callback for this.
        SceneBulletRayResultCallback rayTestResult(callback);
        this->physicsManager.RayTest(rayStart, rayEnd, rayTestResult);

        return rayTestResult.closestSceneNode;
    }


    void Scene::ContactTest(const SceneNode &node, ContactTestCallback &callback)
    {
        // We do the contact test against the nodes proximity component.
        auto proximity = node.GetComponent<GTEngine::ProximityComponent>();
        if (proximity != nullptr)
        {
            callback.collisionGroup = proximity->GetCollisionGroup();
            callback.collisionMask  = proximity->GetCollisionMask();

            SceneContactTestCallback bulletCallback(callback);
            this->physicsManager.ContactTest(proximity->GetGhostObject(), bulletCallback);
        }
    }


    void Scene::OnSceneNodeAdded(SceneNode &)
    {
    }

    void Scene::OnSceneNodeRemoved(SceneNode &)
    {
    }

    void Scene::OnSceneNodeTransform(SceneNode &)
    {
    }

    void Scene::OnSceneNodeScale(SceneNode &)
    {
    }

    void Scene::OnSceneNodeStaticChanged(SceneNode &)
    {
    }

    void Scene::OnSceneNodeVisibleChanged(SceneNode &)
    {
    }

    void Scene::OnSceneNodeComponentChanged(SceneNode &, Component &)
    {
    }

    void Scene::OnPause()
    {
    }

    void Scene::OnResume()
    {
    }
}