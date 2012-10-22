
#ifndef __GTEngine_DefaultSceneCullingManager_hpp_
#define __GTEngine_DefaultSceneCullingManager_hpp_

#include "SceneCullingManager.hpp"
#include "Physics/CollisionWorld.hpp"
#include "Model.hpp"

namespace GTEngine
{
    /// The default culling manager for scenes.
    ///
    /// This can oftern act as the base class for custom managers instead of SceneCullingManager.
    ///
    /// This class uses Bullet for occlusion and frustum culling.
    class DefaultSceneCullingManager : public SceneCullingManager
    {
    public:

        /// Constructor.
        DefaultSceneCullingManager();

        /// Destructor.
        virtual ~DefaultSceneCullingManager();


        /// SceneCullingManager::AddObject().
        virtual void AddObject(SceneObject &object);

        /// SceneCullingManager::RemoveObject().
        virtual void RemoveObject(SceneObject &object);

        /// SceneCullingManager::UpdateTransform().
        virtual void UpdateTransform(SceneObject &object);

        /// SceneCullingManager::UpdateScale().
        virtual void UpdateScale(SceneObject &object);

        /// SceneCullingManageR::UpdateObject().
        virtual void UpdateObject(SceneObject &object);


        /// SceneCullingManager::GetGlobalAABB().
        virtual void GetGlobalAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const;


        /// SceneCullingManager::ProcessVisibleObjects().
        virtual void ProcessVisibleObjects(const glm::mat4 &mvp, VisibilityCallback &callback);


        /// Processes the model of the given object.
        ///
        /// @param object [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleObjects().
        virtual void ProcessVisibleObjectModel(SceneObject &object, VisibilityCallback &callback);

        /// Processes the point light of the given object.
        ///
        /// @param object [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleObjects().
        virtual void ProcessVisibleObjectPointLight(SceneObject &object, VisibilityCallback &callback);

        /// Processes the spot light of the given object.
        ///
        /// @param object [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleObjects().
        virtual void ProcessVisibleObjectSpotLight(SceneObject &object, VisibilityCallback &callback);


        /// Helper method for processing a visible object.
        ///
        /// @param object [in] The object being processed.
        virtual void ProcessVisibleObject(SceneObject &object, VisibilityCallback &callback);



    protected:

        /// The collision world containing collision objects for everything needing culling.
        CollisionWorld world;

        /// The ambient light objects.
        GTCore::Vector<GTEngine::SceneObject*> ambientLights;

        /// The directional light objects.
        GTCore::Vector<GTEngine::SceneObject*> directionalLights;


        /// Structure containing metadata for each scene node object.
        struct SceneNodeMetadata
        {
            SceneNodeMetadata()
                : modelCollisionObject(nullptr),      modelCollisionShape(nullptr),
                  pointLightCollisionObject(nullptr), pointLightCollisionShape(nullptr),
                  spotLightCollisionObject(nullptr),  spotLightCollisionShape(nullptr)
            {
            }

            ~SceneNodeMetadata()
            {
                this->DeleteModelCollisionObject();
                this->DeletePointLightCollisionObject();
                this->DeleteSpotLightCollisionObject();
            }


            void AllocateModelCollisionObject(Model &model, const btTransform &worldTransform, const glm::vec3 &scale)
            {
                this->DeleteModelCollisionObject();

                this->modelCollisionObject = new CollisionObject;
                this->modelCollisionShape  = new btCompoundShape;

                // We attach a single box to the compound shape, which will be the size of the AABB.
                glm::vec3 aabbMin, aabbMax;
                model.GetBaseAABB(aabbMin, aabbMax);

                glm::vec3 halfExtents = (aabbMax - aabbMin) * 0.5f;

                btTransform boxTransform;
                boxTransform.setIdentity();
                boxTransform.setOrigin(btVector3(aabbMin.x + halfExtents.x, aabbMin.y + halfExtents.y, aabbMin.z + halfExtents.z));

                auto boxShape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
                boxShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

                this->modelCollisionShape->addChildShape(boxTransform, boxShape);


                this->modelCollisionObject->setCollisionShape(this->modelCollisionShape);
                this->modelCollisionObject->setWorldTransform(worldTransform);
            }

            void DeleteModelCollisionObject()
            {
                if (modelCollisionShape != nullptr)
                {
                    while (this->modelCollisionShape->getNumChildShapes() > 0)
                    {
                        auto child = this->modelCollisionShape->getChildShape(0);
                        this->modelCollisionShape->removeChildShapeByIndex(0);

                        delete child;
                    }
                }

                delete this->modelCollisionObject;
                delete this->modelCollisionShape;

                this->modelCollisionObject = nullptr;
                this->modelCollisionShape  = nullptr;
            }


            void AllocatePointLightCollisionObject(float radius, const btTransform &worldTransform)
            {
                this->DeletePointLightCollisionObject();

                this->pointLightCollisionObject = new CollisionObject;
                this->pointLightCollisionShape  = new btSphereShape(radius);

                this->pointLightCollisionObject->setCollisionShape(this->pointLightCollisionShape);
                this->pointLightCollisionObject->setWorldTransform(worldTransform);
            }

            void DeletePointLightCollisionObject()
            {
                delete this->pointLightCollisionObject;
                delete this->pointLightCollisionShape;

                this->pointLightCollisionObject = nullptr;
                this->pointLightCollisionShape  = nullptr;
            }


            void AllocateSpotLightCollisionObject(float outerAngle, float height, const btTransform &worldTransform)
            {
                this->DeleteSpotLightCollisionObject();

                this->spotLightCollisionObject = new CollisionObject;
                this->spotLightCollisionShape  = new btCompoundShape;

                // Here we create the cone shape. We need to offset by half the height because Bullet creates it's cones centered.
                btTransform coneTransform;
                coneTransform.setIdentity();
                coneTransform.setOrigin(btVector3(0.0f, 0.0f, -height * 0.5f));
                spotLightCollisionShape->addChildShape(coneTransform, new btConeShapeZ(glm::sin(glm::radians(outerAngle)) * height, height));

                this->spotLightCollisionObject->setCollisionShape(this->spotLightCollisionShape);
                this->spotLightCollisionObject->setWorldTransform(worldTransform);
            }

            void DeleteSpotLightCollisionObject()
            {
                if (spotLightCollisionShape != nullptr)
                {
                    while (this->spotLightCollisionShape->getNumChildShapes() > 0)
                    {
                        auto child = this->spotLightCollisionShape->getChildShape(0);
                        this->spotLightCollisionShape->removeChildShapeByIndex(0);

                        delete child;
                    }
                }

                delete this->spotLightCollisionObject;
                delete this->spotLightCollisionShape;

                this->spotLightCollisionObject = nullptr;
                this->spotLightCollisionShape  = nullptr;
            }


            /// Updates the transformation of the collision objects.
            void UpdateCollisionObjectTransforms(const btTransform &transform)
            {
                if (this->modelCollisionObject != nullptr)
                {
                    this->modelCollisionObject->setWorldTransform(transform);
                    this->modelCollisionObject->GetWorld()->UpdateAABB(*this->modelCollisionObject);
                }

                if (this->pointLightCollisionObject != nullptr)
                {
                    this->pointLightCollisionObject->setWorldTransform(transform);
                    this->pointLightCollisionObject->GetWorld()->UpdateAABB(*this->pointLightCollisionObject);
                }

                if (this->spotLightCollisionObject != nullptr)
                {
                    this->spotLightCollisionObject->setWorldTransform(transform);
                    this->spotLightCollisionObject->GetWorld()->UpdateAABB(*this->spotLightCollisionObject);
                }
            }

            /// Updates the scale of the relevant collision objects.
            void UpdateCollisionObjectScales(const glm::vec3 &scale)
            {
                // We don't scale lights. That should be controlled by modifying the attentuation. To apply scaling, we actually need to remove the
                // collision object, apply the local scaling to the shape, and then re-add the collision object. This is why we do scaling seperate
                // from the position and orientation.
                if (this->modelCollisionObject != nullptr)
                {
                    auto world = this->modelCollisionObject->GetWorld();
                    if (world != nullptr)
                    {
                        world->RemoveCollisionObject(*this->modelCollisionObject);
                    }

                    this->modelCollisionShape->getChildShape(0)->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

                    if (world != nullptr)
                    {
                        world->AddCollisionObject(*this->modelCollisionObject);
                    }
                }
            }

            /// Refreshes the model collision object.
            void RefreshModelCollisionObject(Model &model)
            {
                if (this->modelCollisionObject != nullptr)
                {
                    auto world = this->modelCollisionObject->GetWorld();
                    if (world != nullptr)
                    {
                        world->RemoveCollisionObject(*this->modelCollisionObject);
                    }

                    glm::vec3 aabbMin, aabbMax;
                    model.GetBaseAABB(aabbMin, aabbMax);

                    glm::vec3 halfExtents = (aabbMax - aabbMin) * 0.5f;

                    btTransform boxTransform;
                    boxTransform.setIdentity();
                    boxTransform.setOrigin(btVector3(aabbMin.x + halfExtents.x, aabbMin.y + halfExtents.y, aabbMin.z + halfExtents.z));

                    auto boxShape = static_cast<btBoxShape*>(this->modelCollisionShape->getChildShape(0));
                    boxShape->setImplicitShapeDimensions(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));

                    // Here we need to ensure the box has the correct transformation.
                    this->modelCollisionShape->updateChildTransform(0, boxTransform);
                    
                    if (world != nullptr)
                    {
                        world->AddCollisionObject(*this->modelCollisionObject);
                    }
                }
            }

            void RefreshPointLightCollisionObject(float radius)
            {
                if (this->pointLightCollisionObject != nullptr)
                {
                    auto world = this->pointLightCollisionObject->GetWorld();
                    if (world != nullptr)
                    {
                        world->RemoveCollisionObject(*this->pointLightCollisionObject);
                    }

                    this->pointLightCollisionShape->setUnscaledRadius(radius);

                    if (world != nullptr)
                    {
                        world->AddCollisionObject(*this->pointLightCollisionObject);
                    }
                }
            }

            void RefreshSpotLightCollisionObject(float outerAngle, float height)
            {
                if (this->spotLightCollisionObject != nullptr)
                {
                    auto world = this->spotLightCollisionObject->GetWorld();
                    if (world != nullptr)
                    {
                        world->RemoveCollisionObject(*this->spotLightCollisionObject);
                    }

                    btTransform coneTransform;
                    coneTransform.setIdentity();
                    coneTransform.setOrigin(btVector3(0.0f, 0.0f, -height * 0.5f));
                    
                    // I hate it, but it looks like we will have to delete and re-create the cone shape. Can't find a way to update the radius/length cleanly.
                    auto coneShape = static_cast<btConeShapeZ*>(this->spotLightCollisionShape->getChildShape(0));

                    this->spotLightCollisionShape->removeChildShapeByIndex(0);

                    delete coneShape;
                    coneShape = new btConeShapeZ(glm::sin(glm::radians(outerAngle)) * height, height);

                    this->spotLightCollisionShape->addChildShape(coneTransform, coneShape);

                    if (world != nullptr)
                    {
                        world->AddCollisionObject(*this->spotLightCollisionObject);
                    }
                }
            }


            ////////////////////////////////////////////////////////////
            // Attributes.

            /// A pointer to the collision object for the model component. Can be null.
            CollisionObject* modelCollisionObject;

            /// The collision shape to use for culling the model. Can be null only if <modelCollisionObject> is also null. A model's culling shape
            /// is defined by it's AABB, which will require an offset to be applied. Thus, we're going to use a compound shape.
            btCompoundShape* modelCollisionShape;


            /// A pointer to the collision object for the point light component. Can be null.
            CollisionObject* pointLightCollisionObject;

            /// The collision shape to use with the point light collision object. Can be null only if <pointLightCollisionObject> is also null.
            btSphereShape* pointLightCollisionShape;


            /// A pointer to the collision object for the spot light component. Can be null.
            CollisionObject* spotLightCollisionObject;

            /// The collision shape to use with the spot light collision object. Can be null only if <spotLightCollisionObject> is also null. We
            /// need to use a compound shape here because the cone will need to be offset by half it's height.
            btCompoundShape* spotLightCollisionShape;
        };


        //////////////////////////////////////////////////////////////////
        // The Bullet Dbvt policy structure. Implemented in the cpp file.

        struct DbvtPolicy : btDbvt::ICollide
        {
            /// Constructor.
            DbvtPolicy(DefaultSceneCullingManager &cullingManager, VisibilityCallback &callback, const glm::mat4 &mvp, size_t bufferWidth = 128, size_t bufferHeight = 128);

            /// Destructor.
            virtual ~DbvtPolicy();


            /// btDbvt::ICollide::Descent()
            bool Descent(const btDbvtNode* node);

            /// btDbvt::ICollide::Process()
            void Process(const btDbvtNode* node, btScalar) { this->Process(node); }
            void Process(const btDbvtNode* leaf);


        private:

            /// Appends an occluder using the half extents and transformation.
            ///
            /// @param halfExtents [in] The half extents of the occluder.
            /// @param transform   [in] The transformation to apply to the extents.
            void AppendOccluder(const btVector3 &halfExtents, const btTransform &worldTransform);

            /// Determines whether or not the given AABB is occluded.
            ///
            /// @param center  [in] The center point of the AABB.
            /// @param extents [in] The extents of the AABB.
            bool QueryOccluder(const btVector3 &center, const btVector3 &extents);

        
            /// Transforms the given point the model-view-projection matrix given during construction.
            ///
            /// @param v [in] A reference to the vector to transform.
            ///
            /// @return The transformed vector.
            btVector4 Transform(const btVector3 &v);


            static bool	Project(btVector4* p, int n)
            {
                for(int i = 0; i < n; ++i)
                {
                    p[i][2]  = 1 / p[i][3];
                    p[i][0] *=     p[i][2];
                    p[i][1] *=     p[i][2];
                }

                return true;
            }


            /// Below are rasterizing methods.
            template <int NP>
            inline int Clip(const btVector4* pi, btVector4* po)
            {
                btScalar s[NP];
                int      m = 0;

                for(int i = 0; i < NP; ++i)
	            {
	                s[i] = pi[i][2] + pi[i][3];

	                if (s[i] < 0)
                    {
                        m += 1 << i;
                    }
	            }

                if (m == ((1 << NP) - 1))
                {
                    return(0);
                }

                if(m != 0)
                {
                    int n=0;
                    for (int i = NP - 1, j = 0; j < NP; i = j++)
                    {
                        const btVector4& a = pi[i];
                        const btVector4& b = pi[j];
                        const btScalar   t = s[i] / (a[3] + a[2] - b[3] - b[2]);

                        if ((t > 0) && (t < 1))
                        {
                            po[n][0] = a[0] + (b[0] - a[0]) * t;
                            po[n][1] = a[1] + (b[1] - a[1]) * t;
                            po[n][2] = a[2] + (b[2] - a[2]) * t;
                            po[n][3] = a[3] + (b[3] - a[3]) * t;
                            ++n;
                        }

                        if (s[j] > 0)
                        {
                            po[n++] = b;
                        }
                    }

                    return n;
                }

                for (int i = 0; i < NP; ++i)
                {
                    po[i] = pi[i];
                }

                return NP;
            }

            template <typename POLICY>
            inline bool	Draw(const btVector4 &a, const btVector4 &b, const btVector4 &c, const btScalar minarea)
            {
                btScalar a2 = ((b -a).cross(c - a))[2];
                if (a2 > 0)
                {
                    if (a2 < minarea)
                    {
                        return true;
                    }

                    int sizes[2] =
                    {
                        static_cast<int>(this->bufferWidth),
                        static_cast<int>(this->bufferHeight)
                    };

                    btScalar scales[2] =
                    {
                        sizes[0] * 0.5f,
                        sizes[1] * 0.5f
                    };

                    btScalar offsets[2] =
                    {
                        scales[0] + 0.5f,
                        scales[1] + 0.5f
                    };

                    int x[] =
                    {
                        static_cast<int>(a.x() * scales[0] + offsets[0]),
                        static_cast<int>(b.x() * scales[0] + offsets[0]),
                        static_cast<int>(c.x() * scales[0] + offsets[0])
                    };

                    int y[] =
                    {
                        static_cast<int>(a.y() * scales[1] + offsets[1]),
                        static_cast<int>(b.y() * scales[1] + offsets[1]),
                        static_cast<int>(c.y() * scales[1] + offsets[1])
                    };

                    btScalar z[] =
                    {
                        a.z(),
                        b.z(),
                        c.z()
                    };

                    int mix    = btMax(0,            btMin(x[0], btMin(x[1], x[2])));
                    int mxx    = btMin(sizes[0], 1 + btMax(x[0], btMax(x[1], x[2])));
                    int miy    = btMax(0,            btMin(y[0], btMin(y[1], y[2])));
                    int mxy    = btMin(sizes[1], 1 + btMax(y[0], btMax(y[1], y[2])));
                    int width  = mxx - mix;
                    int height = mxy - miy;

                    if ((width * height) > 0)
                    {
                        int dx[] =
                        {
                            y[0] - y[1],
                            y[1] - y[2],
                            y[2] - y[0]
                        };

                        int dy[] =
                        {
                            x[1] - x[0] - dx[0]*width,
                            x[2] - x[1] - dx[1]*width,
                            x[0] - x[2] - dx[2]*width
                        };

                        int      a   = x[2]*y[0] + x[0]*y[1] - x[2]*y[1] - x[0]*y[2] + x[1]*y[2] - x[1]*y[0];
                        btScalar ia  = 1.0f / static_cast<btScalar>(a);
                        btScalar dzx = ia * (y[2]*(z[1] - z[0]) + y[1]*(z[0] - z[2]) + y[0]*(z[2] - z[1]));
                        btScalar dzy = ia * (x[2]*(z[0] - z[1]) + x[0]*(z[1] - z[2]) + x[1]*(z[2] - z[0])) - (dzx * width);		

                        int c[] =
                        {
                            miy*x[1] + mix*y[0] - x[1]*y[0] - mix*y[1] + x[0]*y[1] - miy*x[0],
                            miy*x[2] + mix*y[1] - x[2]*y[1] - mix*y[2] + x[1]*y[2] - miy*x[1],
                            miy*x[0] + mix*y[2] - x[0]*y[2] - mix*y[0] + x[2]*y[0] - miy*x[2]
                        };

                        btScalar  v    = ia * ((z[2]*c[0]) + (z[0]*c[1]) + (z[1]*c[2]));
                        btScalar* scan = &buffer[miy * sizes[1]];

                        for (int iy = miy; iy < mxy; ++iy)
                        {
                            for (int ix = mix; ix < mxx; ++ix)
                            {
                                if ((c[0] >= 0) && (c[1] >=0 ) && (c[2] >= 0))
                                {
                                    if(POLICY::Process(scan[ix], v))
                                    {
                                        return true;
                                    }
                                }

                                c[0]+=dx[0];
                                c[1]+=dx[1];
                                c[2]+=dx[2];

                                v   +=dzx;
                            }

                            c[0] += dy[0];
                            c[1] += dy[1];
                            c[2] += dy[2];

                            v    += dzy;
                            scan += sizes[0];
                        }
                    }
                }

                return false;
            }



            /// Not sure what this does yet...
            template <int NP, typename POLICY>
            inline bool	ClipDraw(const btVector4* p, btScalar minarea)
            {
                btVector4 o[NP*2];
                int  n         = this->Clip<NP>(p, o);
                bool earlyexit = false;

                this->Project(o, n);

                for(int i = 2; i < n; ++i)
	            {
	                earlyexit |= Draw<POLICY>(o[0], o[i-1], o[i], minarea);
	            }

                return earlyexit;
            }


        private:

            /// The culling manager that owns this policy. We need to call methods on this structure.
            DefaultSceneCullingManager &cullingManager;
            
            /// The callback structure that is passed around to cullingManager.
            VisibilityCallback &callback;

            /// The model-view-projection matrix to use with the culling.
            btScalar mvp[16];

            /// The width of the occlusion buffer.
            size_t bufferWidth;

            /// The height of the occlusion buffer.
            size_t bufferHeight;

            /// The buffer containing "pixel" data from rasterizing.
            btAlignedObjectArray<btScalar> buffer;


        private:    // No copying.
            DbvtPolicy(const DbvtPolicy &);
            DbvtPolicy & operator=(const DbvtPolicy &);
        };
    };
}

#endif