
#include <GTEngine/DefaultSceneCullingManager.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/CollisionGroups.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)
#endif

namespace GTEngine
{
    DefaultSceneCullingManager::DefaultSceneCullingManager()
        : world(), ambientLights(), directionalLights()
    {
    }

    DefaultSceneCullingManager::~DefaultSceneCullingManager()
    {
    }

    void DefaultSceneCullingManager::AddObject(SceneObject &object)
    {
        // All we care about is objects with a visual representation. Culling doesn't really make sense for anything else, by default.
        if (object.GetType() != SceneObjectType_PhysicsObject)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                auto &node = static_cast<SceneNode &>(object);

                btTransform worldTransform;
                node.GetWorldTransform(worldTransform);

                auto metadata = new SceneNodeMetadata;
                node.SetDataPointer(reinterpret_cast<size_t>(this), metadata);

                auto modelComponent = node.GetComponent<GTEngine::ModelComponent>();
                if (modelComponent != nullptr)
                {
                    auto model = modelComponent->GetModel();
                    if (model != nullptr)
                    {
                        metadata->AllocateModelCollisionObject(*model, worldTransform, node.GetWorldScale());

                        metadata->modelCollisionObject->setUserPointer(&object);
                        this->world.AddCollisionObject(*metadata->modelCollisionObject,
                            CollisionGroups::Model,
                            CollisionGroups::PointLight | CollisionGroups::SpotLight);
                    }
                }

                auto pointLightComponent = node.GetComponent<GTEngine::PointLightComponent>();
                if (pointLightComponent != nullptr)
                {
                    metadata->AllocatePointLightCollisionObject(pointLightComponent->GetApproximateRadius(), worldTransform);

                    metadata->pointLightCollisionObject->setUserPointer(&object);
                    this->world.AddCollisionObject(*metadata->pointLightCollisionObject,
                        CollisionGroups::PointLight,
                        CollisionGroups::Model);
                }

                auto spotLightComponent = node.GetComponent<GTEngine::SpotLightComponent>();
                if (spotLightComponent != nullptr)
                {
                    metadata->AllocateSpotLightCollisionObject(spotLightComponent->GetOuterAngle(), spotLightComponent->GetApproximateLength(), worldTransform);

                    metadata->spotLightCollisionObject->setUserPointer(&object);
                    this->world.AddCollisionObject(*metadata->spotLightCollisionObject,
                        CollisionGroups::SpotLight,
                        CollisionGroups::Model);
                }


                // We're going to do ambient and directional lights here. Currently, these do not have any bounding volumes so we will handle these
                // outside of the collision world.
                auto ambientLightComponent = node.GetComponent<GTEngine::AmbientLightComponent>();
                if (ambientLightComponent != nullptr)
                {
                    this->ambientLights.PushBack(&node);
                }

                auto directionalLightComponent = node.GetComponent<GTEngine::DirectionalLightComponent>();
                if (directionalLightComponent != nullptr)
                {
                    this->directionalLights.PushBack(&node);
                }


                // Occluder.
                auto occluder = node.GetComponent<GTEngine::OccluderComponent>();
                if (occluder != nullptr)
                {
                    auto &collisionObject = occluder->GetCollisionObject();

                    btTransform transform;
                    node.GetWorldTransform(transform);
                    collisionObject.setWorldTransform(transform);

                    this->world.AddCollisionObject(collisionObject, CollisionGroups::Occluder, CollisionGroups::All);
                }
            }
        }
    }

    void DefaultSceneCullingManager::RemoveObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            // If we're an occluder, the occluder shape also needs to be removed.
            auto occluderComponent = node.GetComponent<OccluderComponent>();
            if (occluderComponent != nullptr)
            {
                this->world.RemoveCollisionObject(occluderComponent->GetCollisionObject());
            }


            // Ambient/Directional lights.
            auto ambientComponent = node.GetComponent<AmbientLightComponent>();
            if (ambientComponent != nullptr)
            {
                this->ambientLights.RemoveFirst(&node);
            }

            auto directionalComponent = node.GetComponent<DirectionalLightComponent>();
            if (directionalComponent != nullptr)
            {
                this->directionalLights.RemoveFirst(&node);
            }


            // All we need to do here is remove the metadata. This will remove the objects from the collision world.
            auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
            delete metadata;

            node.RemoveDataPointer(reinterpret_cast<size_t>(this));
        }
    }

    void DefaultSceneCullingManager::UpdateTransform(SceneObject &object)
    {
        // If the object is a scene node, we want to use the world transformation.
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            // We first update the culling objects.
            auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
            if (metadata != nullptr)            // <-- Important that we don't use an assert here! Must handle cases where 'object' was not added with AddObject().
            {
                btTransform transform;
                node.GetWorldTransform(transform);

                metadata->UpdateCollisionObjectTransforms(transform);
            }

            // If it's an occluder, it also needs updating.
            auto occluderComponent = node.GetComponent<OccluderComponent>();
            if (occluderComponent != nullptr)
            {
                auto &collisionObject = occluderComponent->GetCollisionObject();

                auto world = collisionObject.GetWorld();
                if (world != nullptr)
                {
                    btTransform transform;
                    node.GetWorldTransform(transform);

                    collisionObject.setWorldTransform(transform);
                    world->UpdateAABB(collisionObject);
                }
            }
        }
    }

    void DefaultSceneCullingManager::UpdateScale(SceneObject &object)
    {
        // We want to use the absolute scale if the object is a scene node.
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
            if (metadata != nullptr)            // <-- Important that we don't use an assert here! Must handle cases where 'object' was not added with AddObject().
            {
                metadata->UpdateCollisionObjectScales(node.GetWorldScale());
            }


            // The occluders also need scaling.
            auto occluder = node.GetComponent<OccluderComponent>();
            if (occluder != nullptr)
            {
                occluder->ApplyScaling(node.GetWorldScale());
            }
        }
    }

    void DefaultSceneCullingManager::UpdateObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
            if (metadata != nullptr)                        // <-- Important that we don't use an assert here! Must handle cases where 'object' was not added with AddObject().
            {
                /*
                auto modelComponent = node.GetComponent<GTEngine::ModelComponent>();
                if (modelComponent != nullptr)
                {
                    auto model = modelComponent->GetModel();
                    if (model != nullptr)
                    {
                        metadata->RefreshModelCollisionObject(*model);        // <-- Does not update transformation or scale.
                    }
                }
                */

                auto pointLightComponent = node.GetComponent<GTEngine::PointLightComponent>();
                if (pointLightComponent != nullptr)
                {
                    metadata->RefreshPointLightCollisionObject(pointLightComponent->GetApproximateRadius());
                }

                auto spotLightComponent = node.GetComponent<GTEngine::SpotLightComponent>();
                if (spotLightComponent != nullptr)
                {
                    metadata->RefreshSpotLightCollisionObject(spotLightComponent->GetOuterAngle(), spotLightComponent->GetApproximateLength());
                }
            }
        }
    }


    /*
    void DefaultSceneCullingManager::UpdateObjectModel(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            btTransform worldTransform;
            node.GetWorldTransform(worldTransform);


        }
    }

    void DefaultSceneCullingManager::UpdateObjectPointLight(SceneObject &object)
    {
    }

    void DefaultSceneCullingManager::UpdateObjectSpotLight(SceneObject &object)
    {
    }
    */


    void DefaultSceneCullingManager::GetGlobalAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
    {
        btVector3 tempMin;
        btVector3 tempMax;
        this->world.GetBroadphase().getBroadphaseAabb(tempMin, tempMax);

        aabbMin = ToGLMVector3(tempMin);
        aabbMax = ToGLMVector3(tempMax);
    }


    void DefaultSceneCullingManager::ProcessVisibleObjects(const glm::mat4 &mvp, VisibilityCallback &callback)
    {
        auto flags = this->GetFlags();
        if ((flags & SceneCullingManager::NoFrustumCulling) && (flags & SceneCullingManager::NoOcclusionCulling))
        {
            auto collisionObjects = this->world.GetInternalWorld().getCollisionObjectArray();
            for (int i = 0; i < collisionObjects.size(); ++i)
            {
                auto collisionObject = collisionObjects[i];
                assert(collisionObject != nullptr);

                auto sceneObject = static_cast<SceneObject*>(collisionObject->getUserPointer());
                if (sceneObject != nullptr)
                {
                    this->ProcessVisibleObject(*sceneObject, callback);
                }
            }
        }
        else
        {
            // The first thing we're going to do is convert our planes to a format Bullet will like.
            Math::Plane planes[6];
            Math::CalculateFrustumPlanes(mvp, planes, false);       // <-- last arguments means "don't normalize" (not needed by Bullet).

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
            DbvtPolicy dbvtPolicy(*this, callback, mvp);

            if (!(flags & SceneCullingManager::NoOcclusionCulling))
            {
                btDbvt::collideOCL(this->world.GetBroadphase().m_sets[1].m_root, planes_n, planes_o, sortDirection, 6, dbvtPolicy);
		        btDbvt::collideOCL(this->world.GetBroadphase().m_sets[0].m_root, planes_n, planes_o, sortDirection, 6, dbvtPolicy);
            }
            else
            {
                btDbvt::collideKDOP(this->world.GetBroadphase().m_sets[1].m_root, planes_n, planes_o, 6, dbvtPolicy);
			    btDbvt::collideKDOP(this->world.GetBroadphase().m_sets[0].m_root, planes_n, planes_o, 6, dbvtPolicy);
            }
        }


        for (size_t i = 0; i < this->ambientLights.count; ++i)
        {
            callback.ProcessObjectAmbientLight(*this->ambientLights[i]);
        }

        for (size_t i = 0; i < this->directionalLights.count; ++i)
        {
            callback.ProcessObjectDirectionalLight(*this->directionalLights[i]);
        }
    }

    void DefaultSceneCullingManager::ProcessVisibleObjectModel(SceneObject &object, VisibilityCallback &callback)
    {
        callback.ProcessObjectModel(object);
    }

    void DefaultSceneCullingManager::ProcessVisibleObjectPointLight(SceneObject &object, VisibilityCallback &callback)
    {
        callback.ProcessObjectPointLight(object);
    }

    void DefaultSceneCullingManager::ProcessVisibleObjectSpotLight(SceneObject &object, VisibilityCallback &callback)
    {
        callback.ProcessObjectSpotLight(object);
    }


    void DefaultSceneCullingManager::ProcessVisibleObject(SceneObject &object, VisibilityCallback &callback)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &sceneNode = static_cast<SceneNode &>(object);

            if (sceneNode.IsVisible())
            {
                auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                if (modelComponent != nullptr)
                {
                    if (modelComponent->GetModel() != nullptr && modelComponent->IsModelVisible())
                    {
                        this->ProcessVisibleObjectModel(sceneNode, callback);
                    }
                }
                
                if (sceneNode.GetComponent<PointLightComponent>() != nullptr)
                {
                    this->ProcessVisibleObjectPointLight(sceneNode, callback);
                }

                if (sceneNode.GetComponent<SpotLightComponent>() != nullptr)
                {
                    this->ProcessVisibleObjectSpotLight(sceneNode, callback);
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

    DefaultSceneCullingManager::DbvtPolicy::DbvtPolicy(DefaultSceneCullingManager &cullingManagerIn, VisibilityCallback &callbackIn, const glm::mat4 &mvpIn, size_t bufferWidthIn, size_t bufferHeightIn)
        : cullingManager(cullingManagerIn), callback(callbackIn),
          mvp(),
          bufferWidth(bufferWidthIn), bufferHeight(bufferHeightIn),
          buffer()
    {
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

        buffer.resize(bufferWidth * bufferHeight, 0);
    }

    DefaultSceneCullingManager::DbvtPolicy::~DbvtPolicy()
    {
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


        auto object = static_cast<SceneObject*>(collisionObject->getUserPointer());
        if (object != nullptr)
        {
            if (object->GetType() == SceneObjectType_SceneNode)
            {
                auto sceneNode = static_cast<SceneNode*>(object);

                if (sceneNode->IsVisible())
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
                        auto modelComponent = sceneNode->GetComponent<ModelComponent>();
                        assert(modelComponent != nullptr);

                        if (modelComponent->GetModel() != nullptr && modelComponent->IsModelVisible())
                        {
                            this->cullingManager.ProcessVisibleObjectModel(*sceneNode, this->callback);
                        }
                    }
                    else if (proxy->m_collisionFilterGroup & CollisionGroups::PointLight)
                    {
                        assert(sceneNode->GetComponent<PointLightComponent>() != nullptr);

                        this->cullingManager.ProcessVisibleObjectPointLight(*sceneNode, this->callback);
                    }
                    else if (proxy->m_collisionFilterGroup & CollisionGroups::SpotLight)
                    {
                        assert(sceneNode->GetComponent<SpotLightComponent>() != nullptr);

                        this->cullingManager.ProcessVisibleObjectSpotLight(*sceneNode, this->callback);
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
