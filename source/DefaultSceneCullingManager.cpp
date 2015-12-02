// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/DefaultSceneCullingManager.hpp>
#include <GTGE/SceneNode.hpp>
#include <GTGE/CollisionGroups.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)       // new behavior: elements of array 'DefaultSceneCullingManager::DbvtPolicy::mvp' will be default initialized
    #pragma warning(disable:4355)       // 'this' used in initializer list.
#endif

namespace GT
{
    DefaultSceneCullingManager::DefaultSceneCullingManager()
        : m_world(), models(), pointLights(), spotLights(), ambientLights(), directionalLights(), particleSystems(),
          dbvtPolicy(*this)
    {
    }

    DefaultSceneCullingManager::~DefaultSceneCullingManager()
    {
    }


    void DefaultSceneCullingManager::AddModel(SceneNode &sceneNode)
    {
        auto modelComponent = sceneNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            auto model = modelComponent->GetModel();
            assert(model != nullptr);
            {
                glm::vec3 aabbMin;
                glm::vec3 aabbMax;
                model->GetAABB(aabbMin, aabbMax);

                aabbMin *= sceneNode.GetWorldScale();
                aabbMax *= sceneNode.GetWorldScale();

                auto cullingObject = new CullingObject_AABB(sceneNode, CollisionGroups::Model, CollisionGroups::PointLight | CollisionGroups::SpotLight, aabbMin, aabbMax);

                
                // The transform needs to be set properly.
                btTransform worldTransform;
                sceneNode.GetWorldTransform(worldTransform);

                cullingObject->SetTransform(worldTransform);


                // The culling object needs to be added to the world...
                m_world.AddCollisionObject(cullingObject->collisionObject, cullingObject->collisionGroup, cullingObject->collisionMask);


                // With everything setup we now just add the store a pointer culling object.
                this->models.Add(&sceneNode, cullingObject);
            }
        }
    }

    void DefaultSceneCullingManager::RemoveModel(SceneNode &sceneNode)
    {
        auto iCullingObject = this->models.Find(&sceneNode);
        if (iCullingObject != nullptr)
        {
            delete iCullingObject->value;
            this->models.RemoveByIndex(iCullingObject->index);
        }
    }


    void DefaultSceneCullingManager::AddPointLight(SceneNode &sceneNode)
    {
        auto pointLightComponent = sceneNode.GetComponent<PointLightComponent>();
        assert(pointLightComponent != nullptr);
        {
            float radius = pointLightComponent->GetRadius();

            auto cullingObject = new CullingObject_Sphere(sceneNode, CollisionGroups::PointLight, CollisionGroups::Model | CollisionGroups::ParticleSystem, radius);


            // The transform needs to be set properly.
            btTransform worldTransform;
            sceneNode.GetWorldTransform(worldTransform);

            cullingObject->SetTransform(worldTransform);


            // The culling object needs to be added to the world...
            m_world.AddCollisionObject(cullingObject->collisionObject, cullingObject->collisionGroup, cullingObject->collisionMask);


            // With everything setup we now just add the store a pointer culling object.
            this->pointLights.Add(&sceneNode, cullingObject);
        }
    }

    void DefaultSceneCullingManager::RemovePointLight(SceneNode &sceneNode)
    {
        auto iCullingObject = this->pointLights.Find(&sceneNode);
        if (iCullingObject != nullptr)
        {
            delete iCullingObject->value;
            this->pointLights.RemoveByIndex(iCullingObject->index);
        }
    }


    void DefaultSceneCullingManager::AddSpotLight(SceneNode &sceneNode)
    {
        auto spotLightComponent = sceneNode.GetComponent<SpotLightComponent>();
        assert(spotLightComponent != nullptr);
        {
            float outerAngle = spotLightComponent->GetOuterAngle();
            float height     = spotLightComponent->GetLength();

            auto cullingObject = new CullingObject_SpotLightCone(sceneNode, CollisionGroups::SpotLight, CollisionGroups::Model | CollisionGroups::ParticleSystem, outerAngle, height);


            // The transform needs to be set properly.
            btTransform worldTransform;
            sceneNode.GetWorldTransform(worldTransform);

            cullingObject->SetTransform(worldTransform);


            // The culling object needs to be added to the world...
            m_world.AddCollisionObject(cullingObject->collisionObject, cullingObject->collisionGroup, cullingObject->collisionMask);


            // With everything setup we now just add the store a pointer culling object.
            this->spotLights.Add(&sceneNode, cullingObject);
        }
    }

    void DefaultSceneCullingManager::RemoveSpotLight(SceneNode &sceneNode)
    {
        auto iCullingObject = this->spotLights.Find(&sceneNode);
        if (iCullingObject != nullptr)
        {
            delete iCullingObject->value;
            this->spotLights.RemoveByIndex(iCullingObject->index);
        }
    }


    void DefaultSceneCullingManager::AddDirectionalLight(SceneNode &sceneNode)
    {
        assert(sceneNode.HasComponent<DirectionalLightComponent>());
        {
            assert(!this->directionalLights.Exists(&sceneNode));
            {
                this->directionalLights.PushBack(&sceneNode);
            }
        }
    }

    void DefaultSceneCullingManager::RemoveDirectionalLight(SceneNode &sceneNode)
    {
        this->directionalLights.RemoveFirstOccuranceOf(&sceneNode);
    }


    void DefaultSceneCullingManager::AddAmbientLight(SceneNode &sceneNode)
    {
        assert(sceneNode.HasComponent<AmbientLightComponent>());
        {
            assert(!this->ambientLights.Exists(&sceneNode));
            {
                this->ambientLights.PushBack(&sceneNode);
            }
        }
    }

    void DefaultSceneCullingManager::RemoveAmbientLight(SceneNode &sceneNode)
    {
        this->ambientLights.RemoveFirstOccuranceOf(&sceneNode);
    }


    void DefaultSceneCullingManager::AddParticleSystem(SceneNode &sceneNode)
    {
        auto particleSystemComponent = sceneNode.GetComponent<ParticleSystemComponent>();
        assert(particleSystemComponent != nullptr);
        {
            auto particleSystem = particleSystemComponent->GetParticleSystem();
            assert(particleSystem != nullptr);
            {
                glm::vec3 aabbMin;
                glm::vec3 aabbMax;
                particleSystem->GetAABB(aabbMin, aabbMax);

                auto cullingObject = new CullingObject_AABB(sceneNode, CollisionGroups::ParticleSystem, CollisionGroups::PointLight | CollisionGroups::SpotLight, aabbMin, aabbMax);


                // The culling object needs to be added to the world...
                m_world.AddCollisionObject(cullingObject->collisionObject, cullingObject->collisionGroup, cullingObject->collisionMask);


                // With everything setup we now just add the store a pointer culling object.
                this->particleSystems.Add(&sceneNode, cullingObject);
            }
        }
    }

    void DefaultSceneCullingManager::RemoveParticleSystem(SceneNode &sceneNode)
    {
        auto iCullingObject = this->particleSystems.Find(&sceneNode);
        if (iCullingObject != nullptr)
        {
            delete iCullingObject->value;
            this->particleSystems.RemoveByIndex(iCullingObject->index);
        }
    }


    void DefaultSceneCullingManager::AddOccluder(SceneNode &sceneNode)
    {
        auto occluderComponent = sceneNode.GetComponent<OccluderComponent>();
        assert(occluderComponent != nullptr);
        {
            auto &collisionObject = occluderComponent->GetCollisionObject();

            btTransform transform;
            sceneNode.GetWorldTransform(transform);
            collisionObject.setWorldTransform(transform);

            m_world.AddCollisionObject(collisionObject, CollisionGroups::Occluder, CollisionGroups::All);
        }
    }

    void DefaultSceneCullingManager::RemoveOccluder(SceneNode &sceneNode)
    {
        auto occluderComponent = sceneNode.GetComponent<OccluderComponent>();
        assert(occluderComponent != nullptr);
        {
            m_world.RemoveCollisionObject(occluderComponent->GetCollisionObject());
        }
    }


    void DefaultSceneCullingManager::UpdateModelTransform(SceneNode &sceneNode)
    {
        auto iCullingObject = this->models.Find(&sceneNode);
        assert(iCullingObject != nullptr);
        {
            auto cullingObject = iCullingObject->value;
            assert(cullingObject != nullptr);
            {
                btTransform transform;
                sceneNode.GetWorldTransform(transform);

                cullingObject->SetTransform(transform);
            }
        }
    }

    void DefaultSceneCullingManager::UpdatePointLightTransform(SceneNode &sceneNode)
    {
        auto iCullingObject = this->pointLights.Find(&sceneNode);
        assert(iCullingObject != nullptr);
        {
            auto cullingObject = iCullingObject->value;
            assert(cullingObject != nullptr);
            {
                btTransform transform;
                sceneNode.GetWorldTransform(transform);

                cullingObject->SetTransform(transform);
            }
        }
    }

    void DefaultSceneCullingManager::UpdateSpotLightTransform(SceneNode &sceneNode)
    {
        auto iCullingObject = this->spotLights.Find(&sceneNode);
        assert(iCullingObject != nullptr);
        {
            auto cullingObject = iCullingObject->value;
            assert(cullingObject != nullptr);
            {
                btTransform transform;
                sceneNode.GetWorldTransform(transform);

                cullingObject->SetTransform(transform);
            }
        }
    }

    void DefaultSceneCullingManager::UpdateDirectionalLightTransform(SceneNode &sceneNode)
    {
        // For now, nothing to actually do here.
        (void)sceneNode;
    }

    void DefaultSceneCullingManager::UpdateAmbientLightTransform(SceneNode &sceneNode)
    {
        // For now, nothing to actually do here.
        (void)sceneNode;
    }

    void DefaultSceneCullingManager::UpdateParticleSystemTransform(SceneNode &sceneNode)
    {
        // The particles in a particle system are always defined in world space, so we don't actually want to do anything when
        // a particle system is translated.
        (void)sceneNode;
    }

    void DefaultSceneCullingManager::UpdateOccluderTransform(SceneNode &sceneNode)
    {
        auto occluderComponent = sceneNode.GetComponent<OccluderComponent>();
        assert(occluderComponent != nullptr);
        {
            auto &collisionObject = occluderComponent->GetCollisionObject();

            auto world = collisionObject.GetWorld();
            if (world != nullptr)
            {
                btTransform transform;
                sceneNode.GetWorldTransform(transform);

                collisionObject.setWorldTransform(transform);
                world->UpdateAABB(collisionObject);
            }
        }
    }


    void DefaultSceneCullingManager::UpdateModelScale(SceneNode &sceneNode)
    {
        this->UpdateModelAABB(sceneNode);
    }

    void DefaultSceneCullingManager::UpdateOccluderScale(SceneNode &sceneNode)
    {
        auto occluderComponent = sceneNode.GetComponent<OccluderComponent>();
        assert(occluderComponent != nullptr);
        {
            occluderComponent->ApplyScaling(sceneNode.GetWorldScale());
        }
    }



    void DefaultSceneCullingManager::UpdateModelAABB(SceneNode &sceneNode)
    {
        auto iCullingObject = this->models.Find(&sceneNode);
        assert(iCullingObject != nullptr);
        {
            auto cullingObject = iCullingObject->value;
            assert(cullingObject != nullptr);
            {
                auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);
                {
                    auto model = modelComponent->GetModel();
                    assert(model != nullptr);
                    {
                        glm::vec3 aabbMin;
                        glm::vec3 aabbMax;
                        model->GetAABB(aabbMin, aabbMax);

                        aabbMin *= sceneNode.GetWorldScale();
                        aabbMax *= sceneNode.GetWorldScale();

                        static_cast<CullingObject_AABB*>(cullingObject)->SetAABB(aabbMin, aabbMax);
                    }
                }
            }
        }
    }

    void DefaultSceneCullingManager::UpdateParticleSystemAABB(SceneNode &sceneNode)
    {
        auto iCullingObject = this->particleSystems.Find(&sceneNode);
        assert(iCullingObject != nullptr);
        {
            auto cullingObject = iCullingObject->value;
            assert(cullingObject != nullptr);
            {
                auto particleSystemComponent = sceneNode.GetComponent<ParticleSystemComponent>();
                assert(particleSystemComponent != nullptr);
                {
                    auto particleSystem = particleSystemComponent->GetParticleSystem();
                    assert(particleSystem != nullptr);
                    {
                        glm::vec3 aabbMin;
                        glm::vec3 aabbMax;
                        particleSystem->GetAABB(aabbMin, aabbMax);

                        static_cast<CullingObject_AABB*>(cullingObject)->SetAABB(aabbMin, aabbMax);
                    }
                }
            }
        }
    }



    void DefaultSceneCullingManager::GetGlobalAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
    {
        btVector3 tempMin;
        btVector3 tempMax;
        m_world.GetBroadphase().getBroadphaseAabb(tempMin, tempMax);

        aabbMin = ToGLMVector3(tempMin);
        aabbMax = ToGLMVector3(tempMax);
    }


    void DefaultSceneCullingManager::ProcessVisibleSceneNodes(const glm::mat4 &mvpIn, VisibilityCallback &callbackIn) const
    {
        auto flags = this->GetFlags();

        // TODO: Occlusion culling is disabled for now since it's unstable at the moment. Need to look into this.
        flags |= SceneCullingManager::NoOcclusionCulling;       // <-- Disable occlusion culling for now.


        if ((flags & SceneCullingManager::NoFrustumCulling) && (flags & SceneCullingManager::NoOcclusionCulling))
        {
            auto collisionObjects = m_world.GetInternalWorld().getCollisionObjectArray();
            for (int i = 0; i < collisionObjects.size(); ++i)
            {
                auto collisionObject = collisionObjects[i];
                assert(collisionObject != nullptr);
                {
                    auto cullingObject = static_cast<CullingObject*>(collisionObject->getUserPointer());
                    assert(cullingObject != nullptr);
                    {
                        this->ProcessVisibleSceneNode(cullingObject->sceneNode, callbackIn);
                    }
                }
            }
        }
        else
        {
            // The first thing we're going to do is convert our planes to a format Bullet will like.
            Math::Plane planes[6];
            Math::CalculateFrustumPlanes(mvpIn, planes, false);       // <-- last arguments means "don't normalize" (not needed by Bullet).

            btVector3 planes_n[6];
	        btScalar  planes_o[6];

            planes_n[0] = btVector3(planes[0].a, planes[0].b, planes[0].c); planes_o[0] = planes[0].d;
            planes_n[1] = btVector3(planes[1].a, planes[1].b, planes[1].c); planes_o[1] = planes[1].d;
            planes_n[2] = btVector3(planes[2].a, planes[2].b, planes[2].c); planes_o[2] = planes[2].d;
            planes_n[3] = btVector3(planes[3].a, planes[3].b, planes[3].c); planes_o[3] = planes[3].d;
            planes_n[4] = btVector3(planes[4].a, planes[4].b, planes[4].c); planes_o[4] = planes[4].d;
            planes_n[5] = btVector3(planes[5].a, planes[5].b, planes[5].c); planes_o[5] = planes[5].d;

            // Here we grab the sort direction. We will retrieve this based on the normal of the near clipping plane (index 5).
            btVector3 sortDirection(planes_n[5]);
            sortDirection.normalize();


            // Now we just create our Dbvt policy and call our occlusion functions.
            this->dbvtPolicy.Initialize(callbackIn, mvpIn);

            if (!(flags & SceneCullingManager::NoOcclusionCulling))
            {
                btDbvt::collideOCL(m_world.GetBroadphase().m_sets[1].m_root, planes_n, planes_o, sortDirection, 6, dbvtPolicy);
		        btDbvt::collideOCL(m_world.GetBroadphase().m_sets[0].m_root, planes_n, planes_o, sortDirection, 6, dbvtPolicy);
            }
            else
            {
                btDbvt::collideKDOP(m_world.GetBroadphase().m_sets[1].m_root, planes_n, planes_o, 6, dbvtPolicy);
			    btDbvt::collideKDOP(m_world.GetBroadphase().m_sets[0].m_root, planes_n, planes_o, 6, dbvtPolicy);
            }
        }


        for (size_t i = 0; i < this->ambientLights.count; ++i)
        {
            callbackIn.ProcessAmbientLight(*this->ambientLights[i]);
        }

        for (size_t i = 0; i < this->directionalLights.count; ++i)
        {
            callbackIn.ProcessDirectionalLight(*this->directionalLights[i]);
        }
    }

    void DefaultSceneCullingManager::QueryPointLightContacts(const SceneNode &light, VisibilityCallback &callbackIn) const
    {
        auto iCullingObject = this->pointLights.Find(&light);
        assert(iCullingObject != nullptr);
        {
            auto cullingObject = iCullingObject->value;
            assert(cullingObject != nullptr);
            {
                LightContactTestCallback callback(light, callbackIn, cullingObject->collisionGroup, cullingObject->collisionMask);
                m_world.ContactTest(cullingObject->collisionObject, callback);
            }
        }
    }

    void DefaultSceneCullingManager::QuerySpotLightContacts(const SceneNode &light, VisibilityCallback &callbackIn) const
    {
        auto iCullingManager = this->spotLights.Find(&light);
        assert(iCullingManager != nullptr);
        {
            auto cullingObject = iCullingManager->value;
            assert(cullingObject != nullptr);
            {
                LightContactTestCallback callback(light, callbackIn, cullingObject->collisionGroup, cullingObject->collisionMask);
                m_world.ContactTest(cullingObject->collisionObject, callback);
            }
        }
    }




    void DefaultSceneCullingManager::ProcessVisibleModel(SceneNode &sceneNode, VisibilityCallback &callback) const
    {
        callback.ProcessModel(sceneNode);
    }

    void DefaultSceneCullingManager::ProcessVisiblePointLight(SceneNode &sceneNode, VisibilityCallback &callback) const
    {
        callback.ProcessPointLight(sceneNode);
    }

    void DefaultSceneCullingManager::ProcessVisibleSpotLight(SceneNode &sceneNode, VisibilityCallback &callback) const
    {
        callback.ProcessSpotLight(sceneNode);
    }

    void DefaultSceneCullingManager::ProcessVisibleParticleSystem(SceneNode &sceneNode, VisibilityCallback &callback) const
    {
        callback.ProcessParticleSystem(sceneNode);
    }


    void DefaultSceneCullingManager::ProcessVisibleSceneNode(SceneNode &sceneNode, VisibilityCallback &callback) const
    {
        if (sceneNode.IsVisible())
        {
            auto modelComponent = sceneNode.GetComponent<ModelComponent>();
            if (modelComponent != nullptr)
            {
                if (modelComponent->GetModel() != nullptr && modelComponent->IsModelVisible())
                {
                    this->ProcessVisibleModel(sceneNode, callback);
                }
            }

            if (sceneNode.GetComponent<PointLightComponent>() != nullptr)
            {
                this->ProcessVisiblePointLight(sceneNode, callback);
            }

            if (sceneNode.GetComponent<SpotLightComponent>() != nullptr)
            {
                this->ProcessVisibleSpotLight(sceneNode, callback);
            }

            auto particleSystemComponent = sceneNode.GetComponent<ParticleSystemComponent>();
            if (particleSystemComponent != nullptr)
            {
                if (particleSystemComponent->GetParticleSystem() != nullptr)
                {
                    this->ProcessVisibleParticleSystem(sceneNode, callback);
                }
            }
        }
    }



    ///////////////////////////////////////////////////////
    // DbvtPolicy

    struct WriteOCL
    {
        SIMD_FORCE_INLINE static bool Process(btScalar &q, btScalar v)
        {
            if (q < v)
            {
                q = v;
            }

            return false;
        }
    };

    struct QueryOCL
    {
        SIMD_FORCE_INLINE static bool Process(btScalar &q, btScalar v)
        {
            return q <= v;
        }
    };

    DefaultSceneCullingManager::DbvtPolicy::DbvtPolicy(const DefaultSceneCullingManager &cullingManagerIn, size_t bufferWidthIn, size_t bufferHeightIn)
        : cullingManager(cullingManagerIn), callback(nullptr),
          mvp(),
          bufferWidth(bufferWidthIn), bufferHeight(bufferHeightIn),
          buffer()
    {
        // First create the buffer.
        buffer.resize(bufferWidth * bufferHeight, 0);
    }

    DefaultSceneCullingManager::DbvtPolicy::~DbvtPolicy()
    {
    }


    void DefaultSceneCullingManager::DbvtPolicy::Initialize(VisibilityCallback &callbackIn, const glm::mat4 &mvpIn)
    {
        this->callback = &callbackIn;

        this->mvp[0]  = mvpIn[0][0];
        this->mvp[1]  = mvpIn[0][1];
        this->mvp[2]  = mvpIn[0][2];
        this->mvp[3]  = mvpIn[0][3];

        this->mvp[4]  = mvpIn[1][0];
        this->mvp[5]  = mvpIn[1][1];
        this->mvp[6]  = mvpIn[1][2];
        this->mvp[7]  = mvpIn[1][3];

        this->mvp[ 8] = mvpIn[2][0];
        this->mvp[ 9] = mvpIn[2][1];
        this->mvp[10] = mvpIn[2][2];
        this->mvp[11] = mvpIn[2][3];

        this->mvp[12] = mvpIn[3][0];
        this->mvp[13] = mvpIn[3][1];
        this->mvp[14] = mvpIn[3][2];
        this->mvp[15] = mvpIn[3][3];
    }


    bool DefaultSceneCullingManager::DbvtPolicy::Descent(const btDbvtNode* node)
    {
        return this->QueryOccluder(node->volume.Center(), node->volume.Extents());
    }

    void DefaultSceneCullingManager::DbvtPolicy::Process(const btDbvtNode* leaf)
    {
        auto proxy           = static_cast<btBroadphaseProxy*>(leaf->data);
        auto collisionObject = static_cast<btCollisionObject*>(proxy->m_clientObject);

        assert(proxy           != nullptr);
        assert(collisionObject != nullptr);


        auto cullingObject = static_cast<CullingObject*>(collisionObject->getUserPointer());
        assert(cullingObject != nullptr);
        {
            auto &sceneNode = cullingObject->sceneNode;

            if (sceneNode.IsVisible())
            {
                // We check the objects collision group to determine the type of the component we're retrieving.
                if (proxy->m_collisionFilterGroup & CollisionGroups::Occluder)
                {
                    // We need to append the occluder. We're only supporting box shapes here, and we'll be upcasting with that assumption.
                    auto shape = static_cast<btBoxShape*>(collisionObject->getCollisionShape());
                    if (shape != nullptr)
                    {
                        this->AppendOccluder(shape->getHalfExtentsWithoutMargin(), collisionObject->getWorldTransform());
                    }
                }
                else if (proxy->m_collisionFilterGroup & CollisionGroups::Model)
                {
                    auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                    assert(modelComponent != nullptr);

                    if (modelComponent->GetModel() != nullptr && modelComponent->IsModelVisible())
                    {
                        this->cullingManager.ProcessVisibleModel(sceneNode, *this->callback);
                    }
                }
                else if (proxy->m_collisionFilterGroup & CollisionGroups::PointLight)
                {
                    assert(sceneNode.GetComponent<PointLightComponent>() != nullptr);

                    this->cullingManager.ProcessVisiblePointLight(sceneNode, *this->callback);
                }
                else if (proxy->m_collisionFilterGroup & CollisionGroups::SpotLight)
                {
                    assert(sceneNode.GetComponent<SpotLightComponent>() != nullptr);

                    this->cullingManager.ProcessVisibleSpotLight(sceneNode, *this->callback);
                }
                else if (proxy->m_collisionFilterGroup & CollisionGroups::ParticleSystem)
                {
                    auto particleSystemComponent = sceneNode.GetComponent<ParticleSystemComponent>();
                    assert(particleSystemComponent != nullptr);
                    {
                        if (particleSystemComponent->GetParticleSystem() != nullptr)
                        {
                            this->cullingManager.ProcessVisibleParticleSystem(sceneNode, *this->callback);
                        }
                    }
                }
            }
        }
    }

    bool DefaultSceneCullingManager::DbvtPolicy::QueryOccluder(const btVector3 &c, const btVector3 &e)
    {
        btVector4 x[] =
        {
            this->Transform(btVector3(c[0] - e[0], c[1] - e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] - e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] + e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] - e[0], c[1] + e[1], c[2] - e[2])),
            this->Transform(btVector3(c[0] - e[0], c[1] - e[1], c[2] + e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] - e[1], c[2] + e[2])),
            this->Transform(btVector3(c[0] + e[0], c[1] + e[1], c[2] + e[2])),
            this->Transform(btVector3(c[0] - e[0], c[1] + e[1], c[2] + e[2]))
        };

        for(int i = 0; i < 8; ++i)
        {
            if ((x[i][2] + x[i][3]) <= 0)
            {
                return true;
            }
        }

        const int d[] =
        {
            1, 0, 3, 2,
            4, 5, 6, 7,
            4, 7, 3, 0,
            6, 5, 1, 2,
            7, 6, 2, 3,
            5, 4, 0, 1
        };

        for(unsigned int i = 0; i < (sizeof(d) / sizeof(d[0])); )
        {
            btVector4 p[] =
            {
                x[d[i++]],
                x[d[i++]],
                x[d[i++]],
                x[d[i++]]
            };

            if (this->ClipDraw<4, QueryOCL>(p, 0.0f))
            {
                return true;
            }
        }

        return false;
    }

    void DefaultSceneCullingManager::DbvtPolicy::AppendOccluder(const btVector3 &halfExtents, const btTransform &transform)
    {
        auto  c     = transform.getOrigin();
        auto &basis = transform.getBasis();
        auto &e     = halfExtents;

        btVector4 x[] =
        {
            this->Transform(c+basis*btVector3(-e[0], -e[1], -e[2])),
            this->Transform(c+basis*btVector3(+e[0], -e[1], -e[2])),
            this->Transform(c+basis*btVector3(+e[0], +e[1], -e[2])),
            this->Transform(c+basis*btVector3(-e[0], +e[1], -e[2])),
            this->Transform(c+basis*btVector3(-e[0], -e[1], +e[2])),
            this->Transform(c+basis*btVector3(+e[0], -e[1], +e[2])),
            this->Transform(c+basis*btVector3(+e[0], +e[1], +e[2])),
            this->Transform(c+basis*btVector3(-e[0], +e[1], +e[2]))
        };

        const int d[] =
        {
            1,0,3,2,
            4,5,6,7,
            4,7,3,0,
            6,5,1,2,
            7,6,2,3,
            5,4,0,1
        };

        for (unsigned int i = 0; i < (sizeof(d) / sizeof(d[0])); )
        {
            btVector4 p[] =
            {
                x[d[i++]],
                x[d[i++]],
                x[d[i++]],
                x[d[i++]]
            };

            this->ClipDraw<4, WriteOCL>(p, 0.0f);
        }
    }

    btVector4 DefaultSceneCullingManager::DbvtPolicy::Transform(const btVector3 &v)
    {
        btVector4 t;
        t[0] = v[0]*mvp[0] + v[1]*mvp[4] + v[2]*mvp[ 8] + mvp[12];
        t[1] = v[0]*mvp[1] + v[1]*mvp[5] + v[2]*mvp[ 9] + mvp[13];
        t[2] = v[0]*mvp[2] + v[1]*mvp[6] + v[2]*mvp[10] + mvp[14];
        t[3] = v[0]*mvp[3] + v[1]*mvp[7] + v[2]*mvp[11] + mvp[15];

        return t;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
