// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneCullingManager_hpp_
#define __GTEngine_DefaultSceneCullingManager_hpp_

#include "SceneCullingManager.hpp"
#include "Physics/CollisionWorld.hpp"
#include "Model.hpp"
#include "CollisionGroups.hpp"

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


        /// SceneCullingManager::AddModel().
        virtual void AddModel(SceneObject &object);

        /// SceneCullingManager::RemoveModel().
        virtual void RemoveModel(SceneObject &object);


        /// SceneCullingManager::AddPointLight().
        virtual void AddPointLight(SceneObject &object);

        /// SceneCullingManager::RemovePointLight().
        virtual void RemovePointLight(SceneObject &object);


        /// SceneCullingManager::AddSpotLight().
        virtual void AddSpotLight(SceneObject &object);

        /// SceneCullingManager::RemoveSpotLight().
        virtual void RemoveSpotLight(SceneObject &object);


        /// SceneCullingManager::AddDirectionalLight().
        virtual void AddDirectionalLight(SceneObject &object);

        /// SceneCullingManager::RemoveDirectionalLight().
        virtual void RemoveDirectionalLight(SceneObject &object);


        /// SceneCullingManager::AddAmbientLight().
        virtual void AddAmbientLight(SceneObject &object);

        /// SceneCullingManager::RemoveAmbientLight().
        virtual void RemoveAmbientLight(SceneObject &object);


        /// SceneCullingManager::AddOccluder().
        virtual void AddOccluder(SceneObject &object);

        /// SceneCullingManager::RemoveOccluder().
        virtual void RemoveOccluder(SceneObject &object);



        /// SceneCullingManager::UpdateModelTransform().
        virtual void UpdateModelTransform(SceneObject &object);

        /// SceneCullingManager::UpdatePointLightTransform().
        virtual void UpdatePointLightTransform(SceneObject &object);

        /// SceneCullingManager::UpdateSpotLightTransform().
        virtual void UpdateSpotLightTransform(SceneObject &object);

        /// SceneCullingManager::UpdateDirectionalLightTransform().
        virtual void UpdateDirectionalLightTransform(SceneObject &object);

        /// SceneCullingManager::UpdateAmbientLightTransform().
        virtual void UpdateAmbientLightTransform(SceneObject &object);

        /// SceneCullingManager::UpdateOccluderLightTransform().
        virtual void UpdateOccluderTransform(SceneObject &object);


        /// SceneCullingManager::UpdateModelScale().
        virtual void UpdateModelScale(SceneObject &object);

        /// SceneCullingManager::UpdateOccluderScale().
        virtual void UpdateOccluderScale(SceneObject &object);



        /// SceneCullingManager::GetGlobalAABB().
        virtual void GetGlobalAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const;


        /// SceneCullingManager::ProcessVisibleObjects().
        virtual void ProcessVisibleObjects(const glm::mat4 &mvp, VisibilityCallback &callback) const;


        /// SceneCullingManager::QueryPointLightContacts().
        virtual void QueryPointLightContacts(const SceneObject &light, VisibilityCallback &callback) const;

        /// SceneCullingManager::QuerySpotLightContacts().
        virtual void QuerySpotLightContacts(const SceneObject &light, VisibilityCallback &callback) const;



        /// Processes the model of the given object.
        ///
        /// @param object [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleObjects().
        virtual void ProcessVisibleObjectModel(SceneObject &object, VisibilityCallback &callback) const;

        /// Processes the point light of the given object.
        ///
        /// @param object [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleObjects().
        virtual void ProcessVisibleObjectPointLight(SceneObject &object, VisibilityCallback &callback) const;

        /// Processes the spot light of the given object.
        ///
        /// @param object [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleObjects().
        virtual void ProcessVisibleObjectSpotLight(SceneObject &object, VisibilityCallback &callback) const;


        /// Helper method for processing a visible object.
        ///
        /// @param object [in] The object being processed.
        virtual void ProcessVisibleObject(SceneObject &object, VisibilityCallback &callback) const;



    protected:

        struct ModelMetadata;
        struct PointLightMetadata;
        struct SpotLightMetadata;


        /// The collision world containing collision objects for everything needing culling.
        CollisionWorld world;

        /// A container for mapping metadata for models to scene nodes.
        GTCore::Map<const SceneObject*, ModelMetadata*> models;

        /// A container for mapping metadata for point lights to scene nodes.
        GTCore::Map<const SceneObject*, PointLightMetadata*> pointLights;

        /// A container for mapping metadata for spot lights to scene nodes.
        GTCore::Map<const SceneObject*, SpotLightMetadata*> spotLights;

        /// The ambient light objects.
        GTCore::Vector<const SceneObject*> ambientLights;

        /// The directional light objects.
        GTCore::Vector<const SceneObject*> directionalLights;


        /// Structure containing metadata for each model.
        struct ModelMetadata
        {
            ModelMetadata(Model &model, const btTransform &worldTransform, const glm::vec3 &scale)
                : collisionObject(nullptr), collisionShape(nullptr), collisionObjectAABBMin(), collisionObjectAABBMax()
            {
                this->collisionObject = new CollisionObject;
                this->collisionShape  = new btCompoundShape;

                // We attach a single box to the compound shape, which will be the size of the AABB.
                model.GetBaseAABB(this->collisionObjectAABBMin, this->collisionObjectAABBMax);

                glm::vec3 aabbMin     = this->collisionObjectAABBMin * scale;
                glm::vec3 aabbMax     = this->collisionObjectAABBMax * scale;
                glm::vec3 halfExtents = (aabbMax - aabbMin) * 0.5f;

                btTransform boxTransform;
                boxTransform.setIdentity();
                boxTransform.setOrigin(btVector3(aabbMin.x + halfExtents.x, aabbMin.y + halfExtents.y, aabbMin.z + halfExtents.z));

                this->collisionShape->addChildShape(boxTransform, new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z)));
                this->collisionShape->recalculateLocalAabb();


                this->collisionObject->setCollisionShape(this->collisionShape);
                this->collisionObject->setWorldTransform(worldTransform);
            }

            ~ModelMetadata()
            {
                if (this->collisionShape != nullptr)
                {
                    while (this->collisionShape->getNumChildShapes() > 0)
                    {
                        auto child = this->collisionShape->getChildShape(0);
                        this->collisionShape->removeChildShapeByIndex(0);

                        delete child;
                    }
                }

                delete this->collisionObject;
                delete this->collisionShape;

                this->collisionObject = nullptr;
                this->collisionShape  = nullptr;
            }


            /// Updates the transform.
            void UpdateTransform(const btTransform &transform)
            {
                assert(this->collisionObject != nullptr);
                {
                    this->collisionObject->setWorldTransform(transform);
                    this->collisionObject->GetWorld()->UpdateAABB(*this->collisionObject);
                }
            }

            /// Updates the scale.
            void UpdateScale(const glm::vec3 &scale)
            {
                assert(this->collisionObject != nullptr);
                {
                    auto world = this->collisionObject->GetWorld();
                    if (world != nullptr)
                    {
                        world->RemoveCollisionObject(*this->collisionObject);
                    }


                    // We need to remove the shape and re-add a new one.
                    delete this->collisionShape->getChildShape(0);
                    this->collisionShape->removeChildShapeByIndex(0);


                    glm::vec3 aabbMin     = this->collisionObjectAABBMin * scale;
                    glm::vec3 aabbMax     = this->collisionObjectAABBMax * scale;
                    glm::vec3 halfExtents = (aabbMax - aabbMin) * 0.5f;

                    btTransform boxTransform;
                    boxTransform.setIdentity();
                    boxTransform.setOrigin(btVector3(aabbMin.x + halfExtents.x, aabbMin.y + halfExtents.y, aabbMin.z + halfExtents.z));


                    this->collisionShape->addChildShape(boxTransform, new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z)));
                    this->collisionShape->recalculateLocalAabb();


                    if (world != nullptr)
                    {
                        world->AddCollisionObject(*this->collisionObject,
                            CollisionGroups::Model,
                            CollisionGroups::PointLight | CollisionGroups::SpotLight);
                    }
                }
            }



            ////////////////////////////////////////////////////////////
            // Attributes.

            /// A pointer to the collision object for the model component. Can be null.
            CollisionObject* collisionObject;

            /// The collision shape to use for culling the model. Can be null only if <modelCollisionObject> is also null. A model's culling shape
            /// is defined by it's AABB, which will require an offset to be applied. Thus, we're going to use a compound shape.
            btCompoundShape* collisionShape;

            /// The min bounds of the model's unscaled AABB.
            glm::vec3 collisionObjectAABBMin;

            /// The max bounds of the model's unscaled AABB.
            glm::vec3 collisionObjectAABBMax;


        private:    // No copying.
            ModelMetadata(const ModelMetadata &);
            ModelMetadata & operator=(const ModelMetadata &);
        };


        /// Structure containing the metadata for each point light.
        struct PointLightMetadata
        {
            PointLightMetadata(float radius, const btTransform &worldTransform)
                : collisionObject(nullptr), collisionShape(nullptr)
            {
                this->collisionObject = new CollisionObject;
                this->collisionShape  = new btSphereShape(radius);

                this->collisionObject->setCollisionShape(this->collisionShape);
                this->collisionObject->setWorldTransform(worldTransform);
            }

            ~PointLightMetadata()
            {
                delete this->collisionObject;
                delete this->collisionShape;
            }


            /// Updates the transformation.
            void UpdateTransform(const btTransform &transform)
            {
                assert(this->collisionObject != nullptr);
                {
                    this->collisionObject->setWorldTransform(transform);
                    this->collisionObject->GetWorld()->UpdateAABB(*this->collisionObject);
                }
            }


            ////////////////////////////////////////////////////////////
            // Attributes.

            /// A pointer to the collision object for the point light component. Can be null.
            CollisionObject* collisionObject;

            /// The collision shape to use with the point light collision object. Can be null only if <pointLightCollisionObject> is also null.
            btSphereShape* collisionShape;


        private:    // No copying.
            PointLightMetadata(const PointLightMetadata &);
            PointLightMetadata & operator=(const PointLightMetadata &);
        };


        /// Structure containing the metadata for each spot light.
        struct SpotLightMetadata
        {
            SpotLightMetadata(float outerAngle, float height, const btTransform &worldTransform)
                : collisionObject(nullptr), collisionShape(nullptr)
            {
                this->collisionObject = new CollisionObject;
                this->collisionShape  = new btCompoundShape;

                // Here we create the cone shape. We need to offset by half the height because Bullet creates it's cones centered.
                btTransform coneTransform;
                coneTransform.setIdentity();
                coneTransform.setOrigin(btVector3(0.0f, 0.0f, -height * 0.5f));
                this->collisionShape->addChildShape(coneTransform, new btConeShapeZ(glm::sin(glm::radians(outerAngle)) * height, height));

                this->collisionObject->setCollisionShape(this->collisionShape);
                this->collisionObject->setWorldTransform(worldTransform);
            }

            ~SpotLightMetadata()
            {
                // Delete the cone shape.
                assert(this->collisionShape->getNumChildShapes() == 1);
                {
                    auto childShape = this->collisionShape->getChildShape(0);
                    assert(childShape != nullptr);
                    {
                        delete childShape;
                        this->collisionShape->removeChildShapeByIndex(0);
                    }
                }


                delete this->collisionObject;
                delete this->collisionShape;
            }


            /// Updates the transform.
            void UpdateTransform(const btTransform &transform)
            {
                assert(this->collisionObject != nullptr);
                {
                    this->collisionObject->setWorldTransform(transform);
                    this->collisionObject->GetWorld()->UpdateAABB(*this->collisionObject);
                }
            }



            ////////////////////////////////////////////////////////////
            // Attributes.

            /// A pointer to the collision object for the spot light component. Can be null.
            CollisionObject* collisionObject;

            /// The collision shape to use with the spot light collision object. Can be null only if <spotLightCollisionObject> is also null. We
            /// need to use a compound shape here because the cone will need to be offset by half it's height.
            btCompoundShape* collisionShape;


        private:    // No copying.
            SpotLightMetadata(const SpotLightMetadata &);
            SpotLightMetadata & operator=(const SpotLightMetadata &);
        };


        //////////////////////////////////////////////////////////////////
        // The Bullet Dbvt policy structure. Implemented in the cpp file.

        struct DbvtPolicy : btDbvt::ICollide
        {
            /// Constructor.
            DbvtPolicy(const DefaultSceneCullingManager &cullingManager, VisibilityCallback &callback, const glm::mat4 &mvp, size_t bufferWidth = 128, size_t bufferHeight = 128);

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
            const DefaultSceneCullingManager &cullingManager;

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
