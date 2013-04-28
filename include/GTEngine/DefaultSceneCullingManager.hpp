// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneCullingManager_hpp_
#define __GTEngine_DefaultSceneCullingManager_hpp_

#include "SceneCullingManager.hpp"
#include "Physics/CollisionWorld.hpp"
#include "Model.hpp"
#include "CollisionGroups.hpp"
#include "SceneNode.hpp"

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
        virtual void AddModel(SceneNode &sceneNode);

        /// SceneCullingManager::RemoveModel().
        virtual void RemoveModel(SceneNode &sceneNode);


        /// SceneCullingManager::AddPointLight().
        virtual void AddPointLight(SceneNode &sceneNode);

        /// SceneCullingManager::RemovePointLight().
        virtual void RemovePointLight(SceneNode &sceneNode);


        /// SceneCullingManager::AddSpotLight().
        virtual void AddSpotLight(SceneNode &sceneNode);

        /// SceneCullingManager::RemoveSpotLight().
        virtual void RemoveSpotLight(SceneNode &sceneNode);


        /// SceneCullingManager::AddDirectionalLight().
        virtual void AddDirectionalLight(SceneNode &sceneNode);

        /// SceneCullingManager::RemoveDirectionalLight().
        virtual void RemoveDirectionalLight(SceneNode &sceneNode);


        /// SceneCullingManager::AddAmbientLight().
        virtual void AddAmbientLight(SceneNode &sceneNode);

        /// SceneCullingManager::RemoveAmbientLight().
        virtual void RemoveAmbientLight(SceneNode &sceneNode);


        /// SceneCullingManager::AddParticleSystem().
        virtual void AddParticleSystem(SceneNode &sceneNode);

        /// SceneCullingManager::RemoveParticleSystem().
        virtual void RemoveParticleSystem(SceneNode &sceneNode);


        /// SceneCullingManager::AddOccluder().
        virtual void AddOccluder(SceneNode &sceneNode);

        /// SceneCullingManager::RemoveOccluder().
        virtual void RemoveOccluder(SceneNode &sceneNode);



        /// SceneCullingManager::UpdateModelTransform().
        virtual void UpdateModelTransform(SceneNode &sceneNode);

        /// SceneCullingManager::UpdatePointLightTransform().
        virtual void UpdatePointLightTransform(SceneNode &sceneNode);

        /// SceneCullingManager::UpdateSpotLightTransform().
        virtual void UpdateSpotLightTransform(SceneNode &sceneNode);

        /// SceneCullingManager::UpdateDirectionalLightTransform().
        virtual void UpdateDirectionalLightTransform(SceneNode &sceneNode);

        /// SceneCullingManager::UpdateAmbientLightTransform().
        virtual void UpdateAmbientLightTransform(SceneNode &sceneNode);

        /// SceneCullingManager::UpdateParticleSystemTransform().
        virtual void UpdateParticleSystemTransform(SceneNode &sceneNode);

        /// SceneCullingManager::UpdateOccluderLightTransform().
        virtual void UpdateOccluderTransform(SceneNode &sceneNode);


        /// SceneCullingManager::UpdateModelScale().
        virtual void UpdateModelScale(SceneNode &sceneNode);

        /// SceneCullingManager::UpdateOccluderScale().
        virtual void UpdateOccluderScale(SceneNode &sceneNode);


        /// SceneCullingManager::UpdateModelAABB().
        virtual void UpdateModelAABB(SceneNode &sceneNode);

        /// SceneCullingManager::UpdateParticleSystemAABB().
        virtual void UpdateParticleSystemAABB(SceneNode &sceneNode);


        /// SceneCullingManager::GetGlobalAABB().
        virtual void GetGlobalAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const;


        /// SceneCullingManager::ProcessVisibleObjects().
        ///
        /// @remarks
        ///     This is not currently thread-safe.
        virtual void ProcessVisibleSceneNodes(const glm::mat4 &mvp, VisibilityCallback &callback) const;


        /// SceneCullingManager::QueryPointLightContacts().
        virtual void QueryPointLightContacts(const SceneNode &light, VisibilityCallback &callback) const;

        /// SceneCullingManager::QuerySpotLightContacts().
        virtual void QuerySpotLightContacts(const SceneNode &light, VisibilityCallback &callback) const;



        /// Processes the model of the given object.
        ///
        /// @param sceneNode [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleSceneNodes().
        virtual void ProcessVisibleModel(SceneNode &sceneNode, VisibilityCallback &callback) const;

        /// Processes the point light of the given object.
        ///
        /// @param sceneNode [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleSceneNodes().
        virtual void ProcessVisiblePointLight(SceneNode &sceneNode, VisibilityCallback &callback) const;

        /// Processes the spot light of the given object.
        ///
        /// @param sceneNode [in] The object being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleSceneNodes().
        virtual void ProcessVisibleSpotLight(SceneNode &sceneNode, VisibilityCallback &callback) const;

        /// Processes the given particle system scene node.
        ///
        /// @param sceneNode [in] The scene node being processed.
        ///
        /// @remarks
        ///     This is called from ProcessVisibleSceneNodes().
        virtual void ProcessVisibleParticleSystem(SceneNode &sceneNode, VisibilityCallback &callback) const;


        /// Helper method for processing a visible object.
        ///
        /// @param sceneNode [in] The object being processed.
        virtual void ProcessVisibleSceneNode(SceneNode &sceneNode, VisibilityCallback &callback) const;



    protected:

        struct CullingObject;


        /// The collision world containing collision objects for everything needing culling.
        CollisionWorld world;

        /// A container for mapping metadata for models to scene nodes.
        GTCore::Map<const SceneNode*, CullingObject*> models;

        /// A container for mapping metadata for point lights to scene nodes.
        GTCore::Map<const SceneNode*, CullingObject*> pointLights;

        /// A container for mapping metadata for spot lights to scene nodes.
        GTCore::Map<const SceneNode*, CullingObject*> spotLights;

        /// The ambient light objects.
        GTCore::Vector<const SceneNode*> ambientLights;

        /// The directional light objects.
        GTCore::Vector<const SceneNode*> directionalLights;

        /// The particle system objects.
        GTCore::Map<const SceneNode*, CullingObject*> particleSystems;



        /// Base structure containing the culling object of a scene node.
        struct CullingObject
        {
            /// A reference to the owner scene node.
            SceneNode &sceneNode;

            /// The container shape of the object. This needs to be a compound shape so that an offset can be applied to sub-shapes.
            btCompoundShape collisionShapeContainer;

            /// The collision object that'll be used for culling.
            CollisionObject collisionObject;

            /// The collision group. This is used in determining the type of the object.
            short collisionGroup;

            /// The collision mask. This is used in determining the types of objects this can be culled against.
            short collisionMask;


            /// Constructor.
            CullingObject(SceneNode &sceneNodeIn, short collisionGroupIn, short collisionMaskIn)
                : sceneNode(sceneNodeIn),
                  collisionShapeContainer(), collisionObject(),
                  collisionGroup(collisionGroupIn), collisionMask(collisionMaskIn)
            {
                this->collisionObject.setUserPointer(this);
            }

            /// Destructor.
            virtual ~CullingObject()
            {
            }


            /// Attaches a culling shape to the object.
            ///
            /// @param shape [in] A reference to the shape to attach.
            void AttachShape(btCollisionShape &shape, const glm::quat &orientationOffset, const glm::vec3 &positionOffset)
            {
                auto world = this->collisionObject.GetWorld();
                if (world != nullptr)
                {
                    world->RemoveCollisionObject(this->collisionObject);
                }

                
                // Attach the shape to the shape container.
                btTransform transform;
                transform.setIdentity();
                transform.setBasis(btMatrix3x3(Math::btQuaternion_cast(orientationOffset)));
                transform.setOrigin(Math::btVector3_cast(positionOffset));
                this->collisionShapeContainer.addChildShape(transform, &shape);

                // Make sure the shape container is the shape of the collision object.
                this->collisionObject.setCollisionShape(&this->collisionShapeContainer);



                if (world != nullptr)
                {
                    world->AddCollisionObject(this->collisionObject, this->collisionGroup, this->collisionMask);
                }
            }

            /// Detaches the given culling shape.
            ///
            /// @param shape [in] A reference to theshape to detach.
            void DetachShape(btCollisionShape &shape)
            {
                auto world = this->collisionObject.GetWorld();
                if (world != nullptr)
                {
                    world->RemoveCollisionObject(this->collisionObject);
                }


                this->collisionShapeContainer.removeChildShape(&shape);


                if (world != nullptr && this->collisionShapeContainer.getNumChildShapes() > 0)
                {
                    world->AddCollisionObject(this->collisionObject, this->collisionGroup, this->collisionMask);
                }
            }


            /// Updates the transformation of the culling object.
            void SetTransform(const btTransform &transform)
            {
                this->collisionObject.setWorldTransform(transform);

                auto world = this->collisionObject.GetWorld();
                if (world != nullptr)
                {
                    world->UpdateAABB(this->collisionObject);
                }
            }


        private:    // No copying.
            CullingObject(const CullingObject &);
            CullingObject & operator=(const CullingObject &);
        };



        /////////////////////////////////////
        // AABB Culling Object.

        /// Structure representing an AABB culling object.
        struct CullingObject_AABB : public CullingObject
        {
            /// The min bounds of the AABB.
            glm::vec3 aabbMin;

            /// The max bounds of the AABB.
            glm::vec3 aabbMax;

            /// The collision shape representing the AABB.
            btBoxShape aabbShape;


            /// Constructor.
            CullingObject_AABB(SceneNode &sceneNode, short collisionGroup, short collisionMask, const glm::vec3 &aabbMinIn, const glm::vec3 &aabbMaxIn)
                : CullingObject(sceneNode, collisionGroup, collisionMask),
                  aabbMin(aabbMinIn), aabbMax(aabbMaxIn),
                  aabbShape(Math::btVector3_cast((aabbMax - aabbMin) * 0.5f))
            {
                this->AttachShape(this->aabbShape, glm::quat(), aabbMin + ((aabbMax - aabbMin) * 0.5f));
            }


            /// Sets the AABB.
            ///
            /// @remarks
            ///     This will update the collision shape appropriately.
            void SetAABB(const glm::vec3 &aabbMinIn, const glm::vec3 &aabbMaxIn)
            {
                // We might need to re-add the object to the world, so we'll grab it here.
                auto world = this->collisionObject.GetWorld();


                // 1) Detach the shape.
                this->DetachShape(this->aabbShape);

                // 2) Resize the shape.
                this->aabbMin = aabbMinIn;
                this->aabbMax = aabbMaxIn;
                this->aabbShape.setImplicitShapeDimensions(Math::btVector3_cast((aabbMax - aabbMin) * 0.5f));

                // 3) Re-attach the shape.
                this->AttachShape(this->aabbShape, glm::quat(), aabbMin + ((aabbMax - aabbMin) * 0.5f));


                // Re-add the object to the world if needed.
                if (this->collisionObject.GetWorld() == nullptr && world != nullptr)
                {
                    world->AddCollisionObject(this->collisionObject, this->collisionGroup, this->collisionMask);
                }
            }



        private:    // No copying.
            CullingObject_AABB(const CullingObject_AABB &);
            CullingObject_AABB & operator=(const CullingObject_AABB &);
        };


        /////////////////////////////////////
        // Sphere Culling Object.

        /// Structure representing a sphere culling object.
        struct CullingObject_Sphere : public CullingObject
        {
            /// The radius of the sphere.
            float radius;

            /// The sphere shape.
            btSphereShape sphereShape;


            /// Constructor.
            CullingObject_Sphere(SceneNode &sceneNode, short collisionGroup, short collisionMask, float radiusIn)
                : CullingObject(sceneNode, collisionGroup, collisionMask),
                  radius(radiusIn),
                  sphereShape(radiusIn)
            {
                this->AttachShape(this->sphereShape, glm::quat(), glm::vec3());
            }


            /// Sets the radius.
            ///
            /// @remarks
            ///     This will update the collision shape appropriately.
            void SetRadius(float radiusIn)
            {
                // We might need to re-add the object to the world, so we'll grab it here.
                auto world = this->collisionObject.GetWorld();


                // 1) Detach the shape.
                this->DetachShape(this->sphereShape);

                // 2) Resize the shape.
                this->radius = radiusIn;
                this->sphereShape.setImplicitShapeDimensions(btVector3(radiusIn, radiusIn, radiusIn));

                // 3) Re-attach the shape.
                this->AttachShape(this->sphereShape, glm::quat(), glm::vec3());


                // Re-add the object to the world if needed.
                if (this->collisionObject.GetWorld() == nullptr && world != nullptr)
                {
                    world->AddCollisionObject(this->collisionObject, this->collisionGroup, this->collisionMask);
                }
            }



        private:    // No copying.
            CullingObject_Sphere(const CullingObject_Sphere &);
            CullingObject_Sphere & operator=(const CullingObject_Sphere &);
        };



        /// Structure containing the metadata for each spot light.
        struct CullingObject_SpotLightCone : public CullingObject
        {
            /// The outer angle of the cone shape.
            float outerAngle;

            /// The height/length of the cone shape.
            float height;

            /// The collision shape.
            btConeShapeZ coneShape;



            /// Constructor.
            CullingObject_SpotLightCone(SceneNode &sceneNode, short collisionGroup, short collisionMask, float outerAngleIn, float heightIn)
                : CullingObject(sceneNode, collisionGroup, collisionMask),
                  outerAngle(outerAngleIn),
                  height(heightIn),
                  coneShape(glm::sin(glm::radians(outerAngle)) * height, height)
            {
                glm::quat rotation = glm::angleAxis(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 position = glm::vec3(0.0f, 0.0f, -height * 0.5f);

                this->AttachShape(this->coneShape, rotation, position);
            }


            /// Sets the cone shape.
            ///
            /// @remarks
            ///     This will update the collision shape appropriately.
            void SetCone(float outerAngleIn, float heightIn)
            {
                // We might need to re-add the object to the world, so we'll grab it here.
                auto world = this->collisionObject.GetWorld();


                // 1) Detach the shape.
                this->DetachShape(this->coneShape);

                // 2) Resize the shape.
                this->outerAngle = outerAngleIn;
                this->height     = heightIn;
                new (&this->coneShape) btConeShapeZ(glm::sin(glm::radians(outerAngle)) * height, height);

                
                // 3) Re-attach the shape.
                glm::quat rotation = glm::angleAxis(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 position = glm::vec3(0.0f, 0.0f, -height * 0.5f);

                this->AttachShape(this->coneShape, rotation, position);


                // Re-add the object to the world if needed.
                if (this->collisionObject.GetWorld() == nullptr && world != nullptr)
                {
                    world->AddCollisionObject(this->collisionObject, this->collisionGroup, this->collisionMask);
                }
            }



        private:
            CullingObject_SpotLightCone(const CullingObject_SpotLightCone &);
            CullingObject_SpotLightCone & operator=(const CullingObject_SpotLightCone &);
        };



        //////////////////////////////////////////////////////////////////
        // The Bullet Dbvt policy structure. Implemented in the cpp file.

        struct DbvtPolicy : btDbvt::ICollide
        {
            /// Constructor.
            DbvtPolicy(const DefaultSceneCullingManager &cullingManager, size_t bufferWidth = 128, size_t bufferHeight = 128);

            /// Destructor.
            virtual ~DbvtPolicy();


            /// Initialises the policy with a new callback and matrix.
            void Initialize(VisibilityCallback &callback, const glm::mat4 &mvp);


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
            VisibilityCallback* callback;

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
        
        /// The persistent DBVT policy to use for culling. This is intended to make things more efficient because it has an expensive constructor, but
        /// it eliminates thread safty for occlusion culling.
        mutable DbvtPolicy dbvtPolicy;



        /// Contact callback for lights.
        class LightContactTestCallback : public btCollisionWorld::ContactResultCallback
        {
        public:

            /// Constructor.
            LightContactTestCallback(const SceneNode &lightIn, VisibilityCallback &callbackIn, short collisionGroup, short collisionMask)
                : light(lightIn), callback(callbackIn)
            {
                this->m_collisionFilterGroup = collisionGroup;
                this->m_collisionFilterMask  = collisionMask;
            }


            btScalar addSingleResult(btManifoldPoint &, const btCollisionObjectWrapper* colObj0, int, int, const btCollisionObjectWrapper* colObj1, int, int)
            {
                assert(colObj0 != nullptr);
                assert(colObj1 != nullptr);

                
                // We assume the user pointer is the culling object.
                auto cullingObject0 = static_cast<CullingObject*>(colObj0->getCollisionObject()->getUserPointer());
                auto cullingObject1 = static_cast<CullingObject*>(colObj1->getCollisionObject()->getUserPointer());

                assert(cullingObject0 != nullptr && cullingObject1 != nullptr);
                {
                    // We can determine the object types by looking at the collision group.
                    if (cullingObject0->collisionGroup == this->m_collisionFilterGroup)
                    {
                        if ((cullingObject1->collisionGroup & CollisionGroups::Model))
                        {
                            this->callback.ProcessModel(cullingObject1->sceneNode);
                        }
                        else if ((cullingObject1->collisionGroup & CollisionGroups::ParticleSystem))
                        {
                            this->callback.ProcessParticleSystem(cullingObject1->sceneNode);
                        }
                    }
                    else
                    {
                        if ((cullingObject0->collisionGroup & CollisionGroups::Model))
                        {
                            this->callback.ProcessModel(cullingObject0->sceneNode);
                        }
                        else if ((cullingObject0->collisionGroup & CollisionGroups::ParticleSystem))
                        {
                            this->callback.ProcessParticleSystem(cullingObject0->sceneNode);
                        }
                    }
                }

                return 0.0f;
            }

            

            /// The main light.
            const SceneNode &light;

            /// A reference to the owner culling manager.
            VisibilityCallback &callback;



        private:    // No copying.
            LightContactTestCallback(const LightContactTestCallback &);
            LightContactTestCallback & operator=(const LightContactTestCallback &);
        };
    };
}

#endif
