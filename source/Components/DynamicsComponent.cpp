
// HACD headers need to be first.
#include <GTEngine/HACD/hacdCircularList.h>
#include <GTEngine/HACD/hacdVector.h>
#include <GTEngine/HACD/hacdICHull.h>
#include <GTEngine/HACD/hacdGraph.h>
#include <GTEngine/HACD/hacdHACD.h>

#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Physics.hpp>
#include <GTEngine/Logging.hpp>


namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(DynamicsComponent, "Dynamics");

    DynamicsComponent::DynamicsComponent(SceneNode &node)
        : Component(node),
          collisionShape(new btCompoundShape(true)),
          motionState(node),
          rigidBody(new RigidBody(0.0f, *collisionShape, &motionState)),
          mass(0.0f),
          isKinematic(false),
          useWithNavigationMesh(true),
          collisionGroup(1), collisionMask(-1)
    {
        this->rigidBody->setUserPointer(&node);
    }

    DynamicsComponent::~DynamicsComponent()
    {
        // We're first going to remove the rigid body from the dynamics world, if we have one.
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }

        // Now we need to delete the collision shapes.
        while (this->collisionShape->getNumChildShapes() > 0)
        {
            auto child = this->collisionShape->getChildShape(0);
            this->collisionShape->removeChildShapeByIndex(0);

            delete child;
        }

        // We will delete the physics objects last.
        delete this->rigidBody;
        delete this->collisionShape;
    }

    void DynamicsComponent::AddBoxCollisionShape(float halfX, float halfY, float halfZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btBoxShape(btVector3(halfX, halfY, halfZ)), offsetX, offsetY, offsetZ);
    }

    void DynamicsComponent::AddSphereCollisionShape(float radius, float offsetX, float offsetY, float offsetZ)
    {
        // Bullet's sphere shape does not support scaling, so we will create an ellipsoid instead.
        this->AddEllipsoidCollisionShape(radius, radius, radius, offsetX, offsetY, offsetZ);
    }

    void DynamicsComponent::AddEllipsoidCollisionShape(float radiusX, float radiusY, float radiusZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btEllipsoidShape(btVector3(radiusX, radiusY, radiusZ)), offsetX, offsetY, offsetZ);
    }


    void DynamicsComponent::AddCylinderXCollisionShape(float halfX, float halfY, float halfZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCylinderShapeX(btVector3(halfX, halfY, halfZ)), offsetX, offsetY, offsetZ);
    }
    void DynamicsComponent::AddCylinderYCollisionShape(float halfX, float halfY, float halfZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCylinderShape(btVector3(halfX, halfY, halfZ)), offsetX, offsetY, offsetZ);
    }
    void DynamicsComponent::AddCylinderZCollisionShape(float halfX, float halfY, float halfZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCylinderShapeZ(btVector3(halfX, halfY, halfZ)), offsetX, offsetY, offsetZ);
    }


    void DynamicsComponent::AddCapsuleXCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCapsuleShapeX(radius, length), offsetX, offsetY, offsetZ);
    }
    void DynamicsComponent::AddCapsuleYCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCapsuleShape(radius, length), offsetX, offsetY, offsetZ);
    }
    void DynamicsComponent::AddCapsuleZCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCapsuleShapeZ(radius, length), offsetX, offsetY, offsetZ);
    }


    void DynamicsComponent::AddStaticPlaneShape(float a, float b, float c, float d)
    {
        this->AddCollisionShape(new btStaticPlaneShape(btVector3(a, b, c), d), 0.0f, 0.0f, 0.0f);
    }


    void DynamicsComponent::AddConvexHullShape(const float* points, size_t pointCount, size_t stride, float margin)
    {
        auto shape = new btConvexHullShape(static_cast<const btScalar*>(points), pointCount, stride);
        shape->setMargin(margin);

        this->AddCollisionShape(shape, 0.0f, 0.0f, 0.0f);
    }

    void DynamicsComponent::AddConvexHullShape(const ConvexHull &convexHull, float margin)
    {
        this->AddConvexHullShape(convexHull.GetVertices(), convexHull.GetVertexCount(), sizeof(glm::vec3), margin);
    }


    void DynamicsComponent::AddConvexHullShapesFromModel(const Model &model, float margin)
    {
        auto &definition = model.GetDefinition();

        // Unintuitively, we're not actually going to use AddConvexHullShape() here. Instead, we're going to go a little lower-level here
        // to avoid some unnecessary calculations.

        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }


        for (size_t i = 0; i < definition.convexHulls.count; ++i)
        {
            auto hull = definition.convexHulls[i];
            assert(hull != nullptr);

            auto shape = new btConvexHullShape(static_cast<const btScalar*>(hull->GetVertices()), hull->GetVertexCount(), 12);
            shape->setMargin(margin);

            // All we need to do is add the new shape to the compound shape...
            this->collisionShape->addChildShape(btTransform::getIdentity(), shape);
        }


        // We need to make sure the shape is scaled correctly.
        glm::vec3 nodeScale = glm::max(glm::vec3(0.0001f), this->node.GetWorldScale());
        this->collisionShape->setLocalScaling(btVector3(nodeScale.x, nodeScale.y, nodeScale.z));

        // We should also update the mass, not that it would matter. We will do it for correctness.
        this->UpdateMass();

        // Now the rigid body needs to be re-added.
        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }


    void DynamicsComponent::RemoveAllCollisionShapes()
    {
        // Since the shapes are being changed we need to remove the rigid body from the world first.
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }


        // All children need to be removed from the shape.
        while (this->collisionShape->getNumChildShapes() > 0)
        {
            auto child = this->collisionShape->getChildShape(0);
            this->collisionShape->removeChildShapeByIndex(0);

            delete child;
        }


        // We should also update the mass, not that it would matter. We will do it for correctness.
        this->UpdateMass();

        // Now the rigid body needs to be re-added.
        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }


    void DynamicsComponent::SetMass(float newMass)
    {
        if (this->mass != newMass)
        {
            this->mass = newMass;
            this->UpdateMass();
        }
    }

    float DynamicsComponent::GetMass() const
    {
        return this->mass;
    }


    void DynamicsComponent::IsKinematic(bool kinematic)
    {
        if (this->isKinematic != kinematic)
        {
            this->isKinematic = kinematic;

            // When changing the kinematic state, the object must first be removed from the world. It will be readded at the end.
            auto world = this->rigidBody->GetWorld();
            if (world != nullptr)
            {
                world->RemoveRigidBody(*this->rigidBody);
            }

            if (kinematic)
            {
                this->SetMass(0.0f);

                this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
            }
            else
            {
                this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
                this->rigidBody->setActivationState(ACTIVE_TAG);
                this->rigidBody->setDeactivationTime(0.0f);
            }

            // The rigid body needs to be readded to the world where applicable.
            if (world != nullptr)
            {
                world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
            }
        }
    }

    bool DynamicsComponent::IsKinematic() const
    {
        return this->isKinematic;
    }

    bool DynamicsComponent::IsStatic() const
    {
        return this->mass == 0.0f && !this->isKinematic;
    }


    void DynamicsComponent::SetFriction(float friction)
    {
        this->rigidBody->setFriction(friction);
    }

    float DynamicsComponent::GetFriction() const
    {
        return this->rigidBody->getFriction();
    }

    void DynamicsComponent::SetRestitution(float restitution)
    {
        this->rigidBody->setRestitution(restitution);
    }

    float DynamicsComponent::GetRestitution() const
    {
        return this->rigidBody->getRestitution();
    }


    void DynamicsComponent::SetDamping(float linear, float angular)
    {
        this->rigidBody->setDamping(linear, angular);
    }

    float DynamicsComponent::GetLinearDamping() const
    {
        return this->rigidBody->getLinearDamping();
    }

    float DynamicsComponent::GetAngularDamping() const
    {
        return this->rigidBody->getAngularDamping();
    }


    void DynamicsComponent::SetCollisionFilter(short group, short mask)
    {
        if (this->collisionGroup != group || this->collisionMask != mask)
        {
            this->collisionGroup = group;
            this->collisionMask  = mask;

            // The body needs to be removed and re-added to it's world for changes to take effect.
            auto world = this->rigidBody->GetWorld();
            if (world != nullptr)
            {
                world->RemoveRigidBody(*this->rigidBody);
                world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
            }
        }
    }


    void DynamicsComponent::ApplyScaling(float x, float y, float z)
    {
        // The geometry is changing, thus we need to remove the body from the world and re-add it after the changes.
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }

        x = GTCore::Max(x, 0.0001f);
        y = GTCore::Max(y, 0.0001f);
        z = GTCore::Max(z, 0.0001f);

        // Now we simply apply the scaling to the shape.
        this->collisionShape->setLocalScaling(btVector3(x, y, z));

        // With a change in the geometry, we need to update the mass.
        this->UpdateMass();

        // Now we need to re-add the body.
        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }


    void DynamicsComponent::ApplySceneNodeScaling()
    {
        this->ApplyScaling(this->node.GetWorldScale());
    }

    void DynamicsComponent::ApplySceneNodeTransformation()
    {
        btTransform transform;
        if (this->rigidBody->getMotionState() != nullptr)
        {
            this->rigidBody->getMotionState()->getWorldTransform(transform);

            this->rigidBody->setWorldTransform(transform);
            this->rigidBody->setInterpolationWorldTransform(transform);
        }
    }


    void DynamicsComponent::SetLinearVelocity(float x, float y, float z)
    {
        this->rigidBody->setLinearVelocity(btVector3(x, y, z) * this->rigidBody->getLinearFactor());
    }

    glm::vec3 DynamicsComponent::GetLinearVelocity() const
    {
        return GTEngine::ToGLMVector3(this->rigidBody->getLinearVelocity());
    }


    void DynamicsComponent::SetAngularVelocity(float x, float y, float z)
    {
        this->rigidBody->setAngularVelocity(btVector3(x, y, z) * this->rigidBody->getAngularFactor());
    }

    glm::vec3 DynamicsComponent::GetAngularVelocity() const
    {
        return GTEngine::ToGLMVector3(this->rigidBody->getAngularVelocity());
    }


    void DynamicsComponent::SetLinearFactor(float x, float y, float z)
    {
        this->rigidBody->setLinearFactor(btVector3(x, y, z));
    }

    glm::vec3 DynamicsComponent::GetLinearFactor() const
    {
        return ToGLMVector3(this->rigidBody->getLinearFactor());
    }


    void DynamicsComponent::SetAngularFactor(float factor)
    {
        this->rigidBody->setAngularFactor(factor);
    }

    void DynamicsComponent::SetAngularFactor(float x, float y, float z)
    {
        this->rigidBody->setAngularFactor(btVector3(x, y, z));
    }

    glm::vec3 DynamicsComponent::GetAngularFactor() const
    {
        return ToGLMVector3(this->rigidBody->getAngularFactor());
    }



    void DynamicsComponent::SetGravity(float x, float y, float z)
    {
        this->rigidBody->setGravity(btVector3(x, y, z));
    }

    glm::vec3 DynamicsComponent::GetGravity() const
    {
        return ToGLMVector3(this->rigidBody->getGravity());
    }

    void DynamicsComponent::ApplyGravity()
    {
        this->rigidBody->applyGravity();
    }


    void DynamicsComponent::ApplyCentralForce(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyCentralForce(btVector3(x, y, z));
    }

    void DynamicsComponent::ApplyTorque(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyTorque(btVector3(x, y, z));
    }


    void DynamicsComponent::ApplyImpulse(float x, float y, float z, float relPosX, float relPosY, float relPosZ)
    {
        this->rigidBody->activate();
        this->rigidBody->applyImpulse(btVector3(x, y, z), btVector3(relPosX, relPosY, relPosZ));
    }

    void DynamicsComponent::ApplyCentralImpulse(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyCentralImpulse(btVector3(x, y, z));
    }

    void DynamicsComponent::ApplyTorqueImpulse(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyTorqueImpulse(btVector3(x, y, z));
    }


    void DynamicsComponent::DisableDeactivation()
    {
        this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
        this->rigidBody->activate();
    }

    void DynamicsComponent::EnableDeactivation()
    {
        this->rigidBody->setActivationState(ACTIVE_TAG);
        this->rigidBody->setDeactivationTime(0.0f);
    }

    bool DynamicsComponent::IsDeactivationEnabled() const
    {
        return this->rigidBody->getActivationState() == ACTIVE_TAG;
    }

    void DynamicsComponent::Activate()
    {
        this->rigidBody->activate();
    }


    void DynamicsComponent::DisableNavigationMeshGeneration()
    {
        this->useWithNavigationMesh = false;
    }

    void DynamicsComponent::EnableNavigationMeshGeneration()
    {
        this->useWithNavigationMesh = true;
    }

    bool DynamicsComponent::IsNavigationMeshGenerationEnabled() const
    { 
        return this->useWithNavigationMesh && this->IsStatic();
    }


    VertexArray* DynamicsComponent::CreateCollisionShapeMesh(bool applyNodeTransform)
    {
        int shapeCount = this->collisionShape->getNumChildShapes();

        // The way we do things is we first build vertex arrays for each individual shape. We that combine them all into a single
        // vertex array to produce the final array.
        GTCore::Vector<VertexArray*> shapeGeometry(shapeCount);

        for (int i = 0; i < shapeCount; ++i)
        {
            auto shape = this->collisionShape->getChildShape(i);
            assert(shape != nullptr);

            auto va = VertexArrayLibrary::CreateFromShape(*shape);
            if (va != nullptr)
            {
                va->ApplyTransform(ToGLMMatrix4(this->collisionShape->getChildTransform(i)));
                shapeGeometry.PushBack(va);
            }
        }

        // At this point we will have a list of vertex arrays for each shape. We need to combine them into a single vertex array.
        if (shapeGeometry.count > 0)
        {
            // TODO:
            //
            // If later one we decide to return a format other than P3, we can do an optimization where we just return shapeGeometry[0] in cases where there is only
            // a single vertex array.

            // Need to combine.
            auto combined = VertexArrayLibrary::CreateCombined(shapeGeometry.buffer, shapeGeometry.count, VertexFormat::P3);
            assert(combined != nullptr);

            // We need to ensure the temp vertex arrays are deleted...
            for (size_t i = 0; i < shapeGeometry.count; ++i)
            {
                delete shapeGeometry[i];
            }

            if (applyNodeTransform)
            {
                combined->ApplyTransform(this->node.GetWorldTransform());
            }
            else
            {
                // We still need to apply the scaling in any case.
                combined->ApplyTransform(glm::scale(this->node.GetWorldScale()));
            }

            // Here we optimize the vertex array to remove duplicate values.
            combined->Optimize();

            return combined;
        }

        return nullptr;
    }


    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void DynamicsComponent::Serialize(GTCore::Serializer &serializer) const
    {
        // We will use the same chunk system here like usual. We have a "general" chunk that will hold basic info such as the mass
        // and collision filters.
        //
        // For everything Bullet related, we want to ensure we save floats and not btScalars. The reason for this is that Bullet can
        // be built so that btScalar is a double. We want these to be compatible between 32- and 64-bit builds.

        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_DynamicsComponent_Main;
        header.version     = 1;
        header.sizeInBytes = 
            sizeof(bool)                  +     // <-- Is deactivation enabled?
            sizeof(bool)                  +     // <-- Is navigation mesh generation enabled?
            sizeof(short) + sizeof(short) +     // <-- Collision filter.
            sizeof(uint32_t);

        serializer.Write(header);
        {
            serializer.Write(this->IsDeactivationEnabled());
            serializer.Write(this->IsNavigationMeshGenerationEnabled());
            serializer.Write(this->collisionGroup);
            serializer.Write(this->collisionMask);
            serializer.Write(static_cast<uint32_t>((this->collisionShape != nullptr) ? this->collisionShape->getNumChildShapes() : 0));
        }

        
        // Now we need to write rigid body chunk, which simply contains information about the current state of the rigid body, not including collision shapes.
        header.id          = Serialization::ChunkID_DynamicsComponent_RigidBody;
        header.version     = 1;
        header.sizeInBytes =
            sizeof(float)                 +     // <-- Mass
            sizeof(float)                 +     // <-- Friction
            sizeof(float)                 +     // <-- Restitution
            sizeof(float) + sizeof(float) +     // <-- Linear + Angular damping
            sizeof(glm::vec3)             +     // <-- Linear velocity
            sizeof(glm::vec3)             +     // <-- Angular velocity
            sizeof(glm::vec3)             +     // <-- Linear factor
            sizeof(glm::vec3)             +     // <-- Angular factor
            sizeof(glm::vec3)             +     // <-- Gravity
            sizeof(bool);                       // <-- Is Kinematic?

        serializer.Write(header);
        {
            serializer.Write(this->mass);
            serializer.Write(this->GetFriction());
            serializer.Write(this->GetRestitution());
            serializer.Write(this->GetLinearDamping());
            serializer.Write(this->GetAngularDamping());
            serializer.Write(this->GetLinearVelocity());
            serializer.Write(this->GetAngularVelocity());
            serializer.Write(this->GetLinearFactor());
            serializer.Write(this->GetAngularFactor());
            serializer.Write(this->GetGravity());
            serializer.Write(this->IsKinematic());
        }


        // We do a single chunk for each attached shape. The idea behind this system is that adding new shapes won't cause any
        // disturbances to anything else.
        if (this->collisionShape != nullptr)
        {
            for (int i = 0; i < this->collisionShape->getNumChildShapes(); ++i)
            {
                auto shape = this->collisionShape->getChildShape(i);
                assert(shape != nullptr);
                {
                    switch (shape->getShapeType())
                    {
                    case BOX_SHAPE_PROXYTYPE:
                        {
                            auto box = static_cast<btBoxShape*>(shape);
                            
                            header.id          = Serialization::ChunkID_DynamicsComponent_BoxShape;
                            header.version     = 1;
                            header.sizeInBytes =
                                sizeof(glm::vec3) +     // <-- Half extents
                                sizeof(glm::mat4);      // <-- Offset transform, as an OpenGL matrix.

                            serializer.Write(header);
                            serializer.Write(ToGLMVector3(box->getHalfExtentsWithMargin() / box->getLocalScaling()));
                            serializer.Write(ToGLMMatrix4(this->collisionShape->getChildTransform(i)));
                            

                            break;
                        }

                    case SPHERE_SHAPE_PROXYTYPE:
                        {
                            auto sphere = static_cast<btSphereShape*>(shape);

                            header.id          = Serialization::ChunkID_DynamicsComponent_SphereShape;
                            header.version     = 1;
                            header.sizeInBytes =
                                sizeof(float) +         // <-- Radius.
                                sizeof(glm::mat4);      // <-- Offset transform, as an OpenGL matrix.

                            serializer.Write(header);
                            serializer.Write(static_cast<float>(sphere->getRadius() / sphere->getLocalScaling().getX()));
                            serializer.Write(ToGLMMatrix4(this->collisionShape->getChildTransform(i)));

                            break;
                        }

                    case CUSTOM_CONVEX_SHAPE_TYPE:          // <-- Ellipsoid for now, but need to change! If anything else uses this, we're broken!
                        {
                            auto ellipsoid = static_cast<btEllipsoidShape*>(shape);

                            btVector3 margin(ellipsoid->getMargin(), ellipsoid->getMargin(), ellipsoid->getMargin());

                            header.id          = Serialization::ChunkID_DynamicsComponent_EllipsoidShape;
                            header.version     = 1;
                            header.sizeInBytes = 
                                sizeof(glm::vec3) +     // <-- Half extents
                                sizeof(glm::mat4);      // <-- Offset transform, as an OpenGL matrix.

                            serializer.Write(header);
                            serializer.Write(ToGLMVector3((ellipsoid->getImplicitShapeDimensions() + margin) / ellipsoid->getLocalScaling()));
                            serializer.Write(ToGLMMatrix4(this->collisionShape->getChildTransform(i)));

                            break;
                        }

                    case CYLINDER_SHAPE_PROXYTYPE:
                        {
                            auto cylinder = static_cast<btCylinderShape*>(shape);

                            uint32_t upAxis = static_cast<uint32_t>(cylinder->getUpAxis());

                            header.id          = Serialization::ChunkID_DynamicsComponent_CylinderShape;
                            header.version     = 1;
                            header.sizeInBytes = 
                                sizeof(uint32_t)  +     // <-- Axis - 0 = X, 1 = Y, 2 = Z
                                sizeof(glm::vec3) +     // <-- Half extents
                                sizeof(glm::mat4);      // <-- Offset transform, as an OpenGL matrix.

                            serializer.Write(header);
                            serializer.Write(upAxis);
                            serializer.Write(ToGLMVector3(cylinder->getHalfExtentsWithMargin() / cylinder->getLocalScaling()));
                            serializer.Write(ToGLMMatrix4(this->collisionShape->getChildTransform(i)));

                            break;
                        }

                    case CAPSULE_SHAPE_PROXYTYPE:
                        {
                            auto capsule = static_cast<btCapsuleShape*>(shape);

                            uint32_t upAxis     = static_cast<uint32_t>(capsule->getUpAxis());
                            uint32_t radiusAxis = (upAxis + 2) % 3;

                            header.id          = Serialization::ChunkID_DynamicsComponent_CapsuleShape;
                            header.version     = 1;
                            header.sizeInBytes = 
                                sizeof(uint32_t) +      // <-- Axis - 0 = X, 1 = Y, 2 = Z
                                sizeof(float)    +      // <-- Radius
                                sizeof(float)    +      // <-- Height
                                sizeof(glm::mat4);      // <-- Offset transform, as an OpenGL matrix.

                            serializer.Write(header);
                            serializer.Write(upAxis);
                            serializer.Write(static_cast<float>(capsule->getRadius()     / capsule->getLocalScaling()[radiusAxis]));
                            serializer.Write(static_cast<float>(capsule->getHalfHeight() / capsule->getLocalScaling()[upAxis]));
                            serializer.Write(ToGLMMatrix4(this->collisionShape->getChildTransform(i)));

                            break;
                        }

                    case CONVEX_HULL_SHAPE_PROXYTYPE:
                        {
                            auto convexHull = static_cast<btConvexHullShape*>(shape);
                            
                            header.id          = Serialization::ChunkID_DynamicsComponent_ConvexHullShape;
                            header.version     = 1;
                            header.sizeInBytes = 
                                sizeof(uint32_t)  +                                     // <-- Vertex count.
                                sizeof(glm::vec3) * convexHull->getNumVertices() +      // <-- Each vertex.
                                sizeof(float);                                          // <-- Margin. Important for convex hulls.

                            serializer.Write(header);
                            serializer.Write(static_cast<uint32_t>(convexHull->getNumVertices()));
                            
                            for (int iVertex = 0; iVertex < convexHull->getNumVertices(); ++iVertex)
                            {
                                btVector3 vertex;
                                convexHull->getVertex(iVertex, vertex);

                                // We want this unscaled.
                                vertex = vertex / convexHull->getLocalScaling();

                                serializer.Write(ToGLMVector3(vertex));
                            }

                            serializer.Write(static_cast<float>(convexHull->getMargin()));

                            break;
                        }


                    default: break;
                    }
                }
            }
        }
    }

    void DynamicsComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        // When deserializing, it's much, much more efficient to first remove the object from the existing scene, change the settings, and then
        // re-add it than it is to call each individual method. Thus, that's what we're doing here.


        bool      isDeactivationEnabled      = false;
        bool      isNavMeshGenerationEnabled = false;
        short     newCollisionGroup          = this->collisionGroup;
        short     newCollisionMask           = this->collisionMask;
        uint32_t  shapeCount                 = 0;

        float     newMass                    = this->mass;
        float     newFriction                = this->GetFriction();
        float     newRestitution             = this->GetRestitution();
        float     newLinearDamping           = this->GetLinearDamping();
        float     newAngularDamping          = this->GetAngularDamping();
        glm::vec3 newLinearVelocity          = this->GetLinearVelocity();
        glm::vec3 newAngularVelocity         = this->GetAngularVelocity();
        glm::vec3 newLinearFactor            = this->GetLinearFactor();
        glm::vec3 newAngularFactor           = this->GetAngularFactor();
        glm::vec3 newGravity                 = this->GetGravity();
        bool      newIsKinematic             = this->IsKinematic();




        // First chunk is general info.
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        switch (header.version)
        {
        case 1:
            {
                deserializer.Read(isDeactivationEnabled);
                deserializer.Read(isNavMeshGenerationEnabled);
                deserializer.Read(newCollisionGroup);
                deserializer.Read(newCollisionMask);
                deserializer.Read(shapeCount);

                break;
            }

        default:
            {
                GTEngine::Log("DynamicsComponent deserialization error. The 'general info' chunk version (%d) is unsupported. Deserialization will continue, but do not expect stability!", header.version);
                break;
            }
        }


        // The next chunk should be the rigid body info.
        deserializer.Read(header);

        switch (header.version)
        {
            case 1:
            {
                deserializer.Read(newMass);
                deserializer.Read(newFriction);
                deserializer.Read(newRestitution);
                deserializer.Read(newLinearDamping);
                deserializer.Read(newAngularDamping);
                deserializer.Read(newLinearVelocity);
                deserializer.Read(newAngularVelocity);
                deserializer.Read(newLinearFactor);
                deserializer.Read(newAngularFactor);
                deserializer.Read(newGravity);
                deserializer.Read(newIsKinematic);

                break;
            }

        default:
            {
                GTEngine::Log("DynamicsComponent deserialization error. The 'rigid body' chunk version (%d) is unsupported. Deserialization will continue, but do not expect stability!", header.version);
                break;
            }
        }


        // At this point we'll have everything except our shapes. What we want to do is remove the rigid body, change the settings, and then
        // re-add it to the scene.
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }


        if (isDeactivationEnabled)
        {
            this->EnableDeactivation();
        }
        else
        {
            this->DisableDeactivation();
        }

        if (isNavMeshGenerationEnabled)
        {
            this->EnableNavigationMeshGeneration();
        }
        else
        {
            this->DisableNavigationMeshGeneration();
        }

        this->collisionGroup = newCollisionGroup;
        this->collisionMask  = newCollisionMask;


        this->mass = newMass;
        this->rigidBody->setFriction(newFriction);
        this->rigidBody->setRestitution(newRestitution);
        this->rigidBody->setDamping(newLinearDamping, newAngularDamping);
        this->rigidBody->setLinearVelocity(ToBulletVector3(newLinearVelocity));
        this->rigidBody->setAngularVelocity(ToBulletVector3(newAngularVelocity));
        this->rigidBody->setLinearFactor(ToBulletVector3(newLinearFactor));
        this->rigidBody->setAngularFactor(ToBulletVector3(newAngularFactor));
        this->rigidBody->setGravity(ToBulletVector3(newGravity));

        this->isKinematic = newIsKinematic;

        if (newIsKinematic)
        {
            this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
        else
        {
            this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
            this->rigidBody->setActivationState(ACTIVE_TAG);
            this->rigidBody->setDeactivationTime(0.0f);
        }


        // Now we need to load the shapes. The old shapes need to be removed.
        while (this->collisionShape->getNumChildShapes() > 0)
        {
            auto child = this->collisionShape->getChildShape(0);
            this->collisionShape->removeChildShapeByIndex(0);

            delete child;
        }

        for (uint32_t iShape = 0; iShape < shapeCount; ++iShape)
        {
            deserializer.Read(header);

            switch (header.id)
            {
            case Serialization::ChunkID_DynamicsComponent_BoxShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            glm::vec3 halfExtents;
                            glm::mat4 transform;

                            deserializer.Read(halfExtents);
                            deserializer.Read(transform);

                            this->collisionShape->addChildShape(ToBulletTransform(transform), new btBoxShape(ToBulletVector3(halfExtents)));

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing dynamics component. Box shape chunk version (%d) is unknown.", header.version);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_DynamicsComponent_SphereShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            float     radius;
                            glm::mat4 transform;

                            deserializer.Read(radius);
                            deserializer.Read(transform);

                            this->collisionShape->addChildShape(ToBulletTransform(transform), new btSphereShape(radius));

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing dynamics component. Sphere shape chunk version (%d) is unknown.", header.version);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_DynamicsComponent_EllipsoidShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            glm::vec3 halfExtents;
                            glm::mat4 transform;

                            deserializer.Read(halfExtents);
                            deserializer.Read(transform);

                            this->collisionShape->addChildShape(ToBulletTransform(transform), new btEllipsoidShape(ToBulletVector3(halfExtents)));

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing dynamics component. Ellipsoid shape chunk version (%d) is unknown.", header.version);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_DynamicsComponent_CylinderShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            uint32_t  upAxis;
                            glm::vec3 halfExtents;
                            glm::mat4 transform;

                            deserializer.Read(upAxis);
                            deserializer.Read(halfExtents);
                            deserializer.Read(transform);

                            if (upAxis == 0)
                            {
                                this->collisionShape->addChildShape(ToBulletTransform(transform), new btCylinderShapeX(ToBulletVector3(halfExtents)));
                            }
                            else if (upAxis == 1)
                            {
                                this->collisionShape->addChildShape(ToBulletTransform(transform), new btCylinderShape(ToBulletVector3(halfExtents)));
                            }
                            else
                            {
                                this->collisionShape->addChildShape(ToBulletTransform(transform), new btCylinderShapeZ(ToBulletVector3(halfExtents)));
                            }

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing dynamics component. Box shape chunk version (%d) is unknown.", header.version);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_DynamicsComponent_CapsuleShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            uint32_t upAxis;
                            float    radius;
                            float    height;
                            glm::mat4 transform;

                            deserializer.Read(upAxis);
                            deserializer.Read(radius);
                            deserializer.Read(height);
                            deserializer.Read(transform);

                            if (upAxis == 0)
                            {
                                this->collisionShape->addChildShape(ToBulletTransform(transform), new btCapsuleShapeX(radius, height));
                            }
                            else if (upAxis == 1)
                            {
                                this->collisionShape->addChildShape(ToBulletTransform(transform), new btCapsuleShape(radius, height));
                            }
                            else
                            {
                                this->collisionShape->addChildShape(ToBulletTransform(transform), new btCapsuleShapeZ(radius, height));
                            }

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing dynamics component. Box shape chunk version (%d) is unknown.", header.version);
                            break;
                        }
                    }

                    break;
                }


            case Serialization::ChunkID_DynamicsComponent_ConvexHullShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            uint32_t                  vertexCount;
                            GTCore::Vector<glm::vec3> vertices;
                            float                     margin;

                            deserializer.Read(vertexCount);

                            vertices.Reserve(vertexCount);
                            vertices.count = vertexCount;
                            deserializer.Read(vertices.buffer, vertexCount * sizeof(glm::vec3));

                            deserializer.Read(margin);


                            this->collisionShape->addChildShape(btTransform::getIdentity(), new btConvexHullShape(&vertices[0].x, vertexCount, 12));


                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing dynamics component. Box shape chunk version (%d) is unknown.", header.version);
                            break;
                        }
                    }

                    break;
                }


            default:
                {
                    // Skip over unknown chunks.
                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }
        
        



        // The scale needs to be applied.
        this->ApplySceneNodeScaling();
        this->ApplySceneNodeTransformation();

        // The mass needs to be updated.
        this->UpdateMass();

        


        // At this point the rigid body should be in it's new state and we can re-add it to the world.
        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }



    void DynamicsComponent::AddCollisionShape(btCollisionShape* shape, float offsetX, float offsetY, float offsetZ)
    {
        // When changing a collision shape, we always want to first remove the rigid body from it's world. We then re-add the body
        // when we're finished with the modifications.
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }

        // All we need to do is add the new shape to the compound shape...
        this->collisionShape->addChildShape(btTransform(btMatrix3x3::getIdentity(), btVector3(offsetX, offsetY, offsetZ)), shape);

        // We need to make sure the shape is scaled correctly.
        glm::vec3 nodeScale = glm::max(glm::vec3(0.0001f), this->node.GetWorldScale());
        this->collisionShape->setLocalScaling(btVector3(nodeScale.x, nodeScale.y, nodeScale.z));


        // With a change in the shape, we also need to update the mass.
        this->UpdateMass();

        // Now we need to re-add the rigid body to the world, if it has one.
        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }

    void DynamicsComponent::UpdateMass()
    {
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }

        // If we only have a single shape attached to the compound shape, we're going to calculate the local inertia based on that shape.
        btVector3 inertia;

        if (this->collisionShape->getNumChildShapes() == 1)
        {
            inertia = GTEngine::BulletUtils::CalculateLocalInertia(this->mass, *this->collisionShape->getChildShape(0));
        }
        else
        {
            inertia = GTEngine::BulletUtils::CalculateLocalInertia(this->mass, *this->collisionShape);
        }

        this->rigidBody->setMassProps(this->mass, inertia);
        this->rigidBody->updateInertiaTensor();


        // If we've turned into a static object there's a few things we'll want to do. First, we want the linear and angular velocities to
        // be reset to zero. After that, we want to ensure the rigid body is positioned at the same position as the scene node.
        if (this->mass == 0.0f)
        {
            this->rigidBody->setLinearVelocity( btVector3(0.0f, 0.0f, 0.0f));
            this->rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

            this->ApplySceneNodeTransformation();
        }

        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }
}
