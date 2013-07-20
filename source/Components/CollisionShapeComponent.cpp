// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.
#include <GTEngine/Components/CollisionShapeComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#undef max

namespace GTEngine
{
    /// Some metadata to associate with each child shape of a base collision shape.
    ///
    /// A metadata structure is allocated for each shape, and deleted when they are removed.
    struct ChildCollisionShapeMetadata
    {
        /// Constructor.
        ChildCollisionShapeMetadata(const glm::vec3 &unscaledOffsetIn, const glm::vec3 &rotationIn = glm::vec3(0.0f, 0.0f, 0.0f))
            : unscaledOffset(unscaledOffsetIn), rotation(rotationIn)
        {
        }


        /// The unscaled offset of the shape.
        glm::vec3 unscaledOffset;

        /// The rotational offset, as euler angles in degrees.
        glm::vec3 rotation;
    };



    CollisionShapeComponent::CollisionShapeComponent(SceneNode &node)
        : Component(node),
          collisionShape(true),
          collisionGroup(1), collisionMask(-1),
          usingConvexHullsOfModel(false)
    {
    }

    CollisionShapeComponent::~CollisionShapeComponent()
    {
        // Now we need to delete the collision shapes.
        this->RemoveAllCollisionShapes(false);
    }



    void CollisionShapeComponent::AddBoxCollisionShape(float halfX, float halfY, float halfZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btBoxShape(btVector3(halfX, halfY, halfZ)), offsetX, offsetY, offsetZ);
    }

    void CollisionShapeComponent::AddSphereCollisionShape(float radius, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btSphereShape(radius), offsetX, offsetY, offsetZ);
    }

    void CollisionShapeComponent::AddEllipsoidCollisionShape(float radiusX, float radiusY, float radiusZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btEllipsoidShape(btVector3(radiusX, radiusY, radiusZ)), offsetX, offsetY, offsetZ);
    }


    void CollisionShapeComponent::AddCylinderXCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCylinderShapeX(btVector3(length * 0.5f, radius, radius)), offsetX, offsetY, offsetZ);
    }
    void CollisionShapeComponent::AddCylinderYCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCylinderShape(btVector3(radius, length * 0.5f, radius)), offsetX, offsetY, offsetZ);
    }
    void CollisionShapeComponent::AddCylinderZCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCylinderShapeZ(btVector3(radius, radius, length * 0.5f)), offsetX, offsetY, offsetZ);
    }


    void CollisionShapeComponent::AddCapsuleXCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCapsuleShapeX(radius, length), offsetX, offsetY, offsetZ);
    }
    void CollisionShapeComponent::AddCapsuleYCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCapsuleShape(radius, length), offsetX, offsetY, offsetZ);
    }
    void CollisionShapeComponent::AddCapsuleZCollisionShape(float radius, float length, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btCapsuleShapeZ(radius, length), offsetX, offsetY, offsetZ);
    }


    void CollisionShapeComponent::AddStaticPlaneShape(float a, float b, float c, float d)
    {
        this->AddCollisionShape(new btStaticPlaneShape(btVector3(a, b, c), d), 0.0f, 0.0f, 0.0f);
    }


    void CollisionShapeComponent::AddConvexHullCollisionShape(const float* points, size_t pointCount, size_t stride, float margin)
    {
        auto shape = new btConvexHullShape(static_cast<const btScalar*>(points), pointCount, stride);
        shape->setMargin(margin);

        this->AddCollisionShape(shape, 0.0f, 0.0f, 0.0f);
    }

    void CollisionShapeComponent::AddConvexHullCollisionShape(const ConvexHull &convexHull, float margin)
    {
        this->AddConvexHullCollisionShape(convexHull.GetVertices(), convexHull.GetVertexCount(), sizeof(glm::vec3), margin);
    }


    void CollisionShapeComponent::AddModelConvexHullsCollisionShape(float margin, float offsetX, float offsetY, float offsetZ)
    {
        // Even if we don't have an attached model comonent, we still want to add the collision shape.
        auto shape = new btCompoundShape(false);
        this->AddModelConvexHullsToCompoundShape(shape, margin);

        this->AddCollisionShape(shape, offsetX, offsetY, offsetZ);
    }


    void CollisionShapeComponent::RefreshModelConvexHullsShapes()
    {
        for (int iChildShape = 0; iChildShape < this->collisionShape.getNumChildShapes(); ++iChildShape)
        {
            // If the child shape is another compound shape, we can assume it's the shape of a model convex hulls.
            auto childShape = this->collisionShape.getChildShape(iChildShape);
            assert(childShape != nullptr);
            {
                if (childShape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
                {
                    // This is a shape containing the model convex hulls. We just delete all of the shapes and re-create them, giving them the same margin
                    // as the parent compound shape.
                    auto convexHullsParentShape = static_cast<btCompoundShape*>(childShape);

                    while (convexHullsParentShape->getNumChildShapes() > 0)
                    {
                        auto convexHullShape = convexHullsParentShape->getChildShape(0);
                        assert(convexHullShape != nullptr);
                        {
                            convexHullsParentShape->removeChildShapeByIndex(0);
                            delete convexHullShape;
                        }
                    }

                    
                    // Now we need to re-attach new convex hull shapes.
                    this->AddModelConvexHullsToCompoundShape(convexHullsParentShape, convexHullsParentShape->getMargin());
                }
            }
        }
    }


    void CollisionShapeComponent::SetCollisionShapesToModelConvexHulls(const Model &model, float margin)
    {
        this->OnPreCollisionShapeChanged();
        {
            this->RemoveAllCollisionShapes(false);      // <-- 'false' prevents events from getting posted.
            auto &definition = model.GetDefinition();

            // Unintuitively, we're not actually going to use AddConvexHullShape() here. Instead, we're going to go a little lower-level here
            // to avoid some unnecessary calculations.

            for (size_t i = 0; i < definition.convexHulls.count; ++i)
            {
                auto hull = definition.convexHulls[i];
                assert(hull != nullptr);

                auto shape = new btConvexHullShape(static_cast<const btScalar*>(hull->GetVertices()), hull->GetVertexCount(), 12);
                shape->setMargin(margin);

                /// The child collision shape needs metadata.
                shape->setUserPointer(new ChildCollisionShapeMetadata(glm::vec3(0.0f, 0.0f, 0.0f)));

                // All we need to do is add the new shape to the compound shape...
                this->collisionShape.addChildShape(btTransform::getIdentity(), shape);
            }


            // We need to make sure the shape is scaled correctly.
            glm::vec3 nodeScale = glm::max(glm::vec3(0.0001f), this->node.GetWorldScale());
            this->collisionShape.setLocalScaling(btVector3(nodeScale.x, nodeScale.y, nodeScale.z));


            this->usingConvexHullsOfModel = true;
        }
        this->OnPostCollisionShapeChanged();
        this->OnChanged();
    }

    void CollisionShapeComponent::SetCollisionShapesToModelConvexHulls(float margin)
    {
        auto modelComponent = this->node.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                this->SetCollisionShapesToModelConvexHulls(*model, margin);
            }
        }
    }


    void CollisionShapeComponent::RemoveAllCollisionShapes(bool postEvent)
    {
        if (postEvent) this->OnPreCollisionShapeChanged();
        {
            // All children need to be removed from the shape.
            while (this->collisionShape.getNumChildShapes() > 0)
            {
                auto child = this->collisionShape.getChildShape(0);
                delete reinterpret_cast<ChildCollisionShapeMetadata*>(child->getUserPointer());

                this->collisionShape.removeChildShapeByIndex(0);
                delete child;
            }

            this->usingConvexHullsOfModel = false;
        }
        if (postEvent) this->OnPostCollisionShapeChanged();
        if (postEvent) this->OnChanged();
    }

    void CollisionShapeComponent::RemoveCollisionShapeAtIndex(size_t index)
    {
        this->OnPreCollisionShapeChanged();
        {
            // All children need to be removed from the shape.
            auto child = this->collisionShape.getChildShape(static_cast<int>(index));
            delete reinterpret_cast<ChildCollisionShapeMetadata*>(child->getUserPointer());
            
            this->collisionShape.removeChildShapeByIndex(static_cast<int>(index));
            delete child;

            this->usingConvexHullsOfModel = false;
        }
        this->OnPostCollisionShapeChanged();
        this->OnChanged();
    }


    glm::vec3 CollisionShapeComponent::GetCollisionShapeOffset(size_t index) const
    {
        auto childShape = this->collisionShape.getChildShape(static_cast<int>(index));
        if (childShape != nullptr)
        {
            auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(childShape->getUserPointer());
            if (metadata != nullptr)
            {
                return metadata->unscaledOffset;
            }
            else
            {
                return Math::vec3_cast(this->collisionShape.getChildTransform(static_cast<int>(index)).getOrigin());
            }
        }
        
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void CollisionShapeComponent::SetCollisionShapeOffset(size_t index, float offsetX, float offsetY, float offsetZ, bool postOnChanged)
    {
        this->OnPreCollisionShapeChanged();
        {
            auto childShape = this->collisionShape.getChildShape(static_cast<int>(index));
            if (childShape != nullptr)
            {
                auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(childShape->getUserPointer());
                if (metadata != nullptr)
                {
                    metadata->unscaledOffset = glm::vec3(offsetX, offsetY, offsetZ);
                }
            }

            btTransform newTransform;
            newTransform.setIdentity();
            newTransform.setOrigin(btVector3(offsetX, offsetY, offsetZ) * this->collisionShape.getLocalScaling());

            this->collisionShape.updateChildTransform(index, newTransform);
        }
        this->OnPostCollisionShapeChanged();

        if (postOnChanged)
        {
            this->OnChanged();
        }
    }


    bool CollisionShapeComponent::SetBoxCollisionShapeHalfExtents(size_t index, float halfX, float halfY, float halfZ)
    {
        if (GetCollisionShapeType(this->collisionShape.getChildShape(index)) == CollisionShapeType_Box)
        {
            this->OnPreCollisionShapeChanged();
            {
                auto shape = static_cast<btBoxShape*>(this->collisionShape.getChildShape(index));
                auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                auto scale    = shape->getLocalScaling();

                // And this is how we are going to change the extents... Can't find a better way to do this
                shape->~btBoxShape();
                new (shape) btBoxShape(btVector3(halfX, halfY, halfZ));
                shape->setLocalScaling(scale);
                shape->setUserPointer(metadata);


                // We need to update the child's transform
                btTransform childTransform;
                childTransform.setBasis(Math::btMatrix3x3_cast(glm::quat(glm::radians(metadata->rotation))));
                childTransform.setOrigin(Math::btVector3_cast(metadata->unscaledOffset));
                this->collisionShape.updateChildTransform(index, childTransform);
            }
            this->OnPostCollisionShapeChanged();
            this->OnChanged();


            return true;
        }

        return false;
    }

    bool CollisionShapeComponent::SetSphereCollisionShapeRadius(size_t index, float radius)
    {
        if (GetCollisionShapeType(this->collisionShape.getChildShape(index)) == CollisionShapeType_Sphere)
        {
            this->OnPreCollisionShapeChanged();
            {
                auto shape = static_cast<btSphereShape*>(this->collisionShape.getChildShape(index));
                auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                auto scale    = shape->getLocalScaling();

                // And this is how we are going to change the extents... Can't find a better way to do this
                shape->~btSphereShape();
                new (shape) btSphereShape(radius);
                shape->setLocalScaling(scale);
                shape->setUserPointer(metadata);


                // We need to call this in order to get everything looking correct.
                this->collisionShape.updateChildTransform(index, this->collisionShape.getChildTransform(index));
            }
            this->OnPostCollisionShapeChanged();
            this->OnChanged();


            return true;
        }

        return false;
    }

    bool CollisionShapeComponent::SetEllipsoidCollisionShapeRadius(size_t index, float radiusX, float radiusY, float radiusZ)
    {
        if (GetCollisionShapeType(this->collisionShape.getChildShape(index)) == CollisionShapeType_Ellipsoid)
        {
            this->OnPreCollisionShapeChanged();
            {
                auto shape = static_cast<btEllipsoidShape*>(this->collisionShape.getChildShape(index));
                auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                auto scale    = shape->getLocalScaling();

                // And this is how we are going to change the extents... Can't find a better way to do this
                shape->~btEllipsoidShape();
                new (shape) btEllipsoidShape(btVector3(radiusX, radiusY, radiusZ));
                shape->setLocalScaling(scale);
                shape->setUserPointer(metadata);


                // We need to call this in order to get everything looking correct.
                this->collisionShape.updateChildTransform(index, this->collisionShape.getChildTransform(index));
            }
            this->OnPostCollisionShapeChanged();
            this->OnChanged();


            return true;
        }

        return false;
    }

    bool CollisionShapeComponent::SetCylinderCollisionShapeSize(size_t index, float radius, float length)
    {
        auto type = GetCollisionShapeType(this->collisionShape.getChildShape(index));

        if (type == CollisionShapeType_CylinderX || type == CollisionShapeType_CylinderY || type == CollisionShapeType_CylinderZ)
        {
            this->OnPreCollisionShapeChanged();
            {
                if (type == CollisionShapeType_CylinderX)
                {
                    auto shape = static_cast<btCylinderShapeX*>(this->collisionShape.getChildShape(index));
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                    auto scale    = shape->getLocalScaling();

                    // And this is how we are going to change the extents... Can't find a better way to do this
                    shape->~btCylinderShapeX();
                    new (shape) btCylinderShapeX(btVector3(length * 0.5f, radius, radius));
                    shape->setLocalScaling(scale);
                    shape->setUserPointer(metadata);
                }
                else if (type == CollisionShapeType_CylinderY)
                {
                    auto shape = static_cast<btCylinderShape*>(this->collisionShape.getChildShape(index));
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                    auto scale    = shape->getLocalScaling();

                    // And this is how we are going to change the extents... Can't find a better way to do this
                    shape->~btCylinderShape();
                    new (shape) btCylinderShape(btVector3(radius, length * 0.5f, radius));
                    shape->setLocalScaling(scale);
                    shape->setUserPointer(metadata);
                }
                else if (type == CollisionShapeType_CylinderZ)
                {
                    auto shape = static_cast<btCylinderShapeZ*>(this->collisionShape.getChildShape(index));
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                    auto scale    = shape->getLocalScaling();

                    // And this is how we are going to change the extents... Can't find a better way to do this
                    shape->~btCylinderShapeZ();
                    new (shape) btCylinderShapeZ(btVector3(radius, radius, length * 0.5f));
                    shape->setLocalScaling(scale);
                    shape->setUserPointer(metadata);
                }


                // We need to call this in order to get everything looking correct.
                this->collisionShape.updateChildTransform(index, this->collisionShape.getChildTransform(index));
            }
            this->OnPostCollisionShapeChanged();
            this->OnChanged();


            return true;
        }

        return false;
    }

    bool CollisionShapeComponent::SetCapsuleCollisionShapeSize(size_t index, float radius, float height)
    {
        auto type = GetCollisionShapeType(this->collisionShape.getChildShape(index));

        if (type == CollisionShapeType_CapsuleX || type == CollisionShapeType_CapsuleY || type == CollisionShapeType_CapsuleZ)
        {
            this->OnPreCollisionShapeChanged();
            {
                if (type == CollisionShapeType_CapsuleX)
                {
                    auto shape = static_cast<btCapsuleShapeX*>(this->collisionShape.getChildShape(index));
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                    auto scale    = shape->getLocalScaling();

                    // And this is how we are going to change the extents... Can't find a better way to do this
                    shape->~btCapsuleShapeX();
                    new (shape) btCapsuleShapeX(radius, height);
                    shape->setLocalScaling(scale);
                    shape->setUserPointer(metadata);
                }
                else if (type == CollisionShapeType_CapsuleY)
                {
                    auto shape = static_cast<btCapsuleShape*>(this->collisionShape.getChildShape(index));
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                    auto scale    = shape->getLocalScaling();

                    // And this is how we are going to change the extents... Can't find a better way to do this
                    shape->~btCapsuleShape();
                    new (shape) btCapsuleShape(radius, height);
                    shape->setLocalScaling(scale);
                    shape->setUserPointer(metadata);
                }
                else if (type == CollisionShapeType_CapsuleZ)
                {
                    auto shape = static_cast<btCapsuleShapeZ*>(this->collisionShape.getChildShape(index));
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape->getUserPointer());
                    auto scale    = shape->getLocalScaling();

                    // And this is how we are going to change the extents... Can't find a better way to do this
                    shape->~btCapsuleShapeZ();
                    new (shape) btCapsuleShapeZ(radius, height);
                    shape->setLocalScaling(scale);
                    shape->setUserPointer(metadata);
                }


                // We need to call this in order to get everything looking correct.
                this->collisionShape.updateChildTransform(index, this->collisionShape.getChildTransform(index));
            }
            this->OnPostCollisionShapeChanged();
            this->OnChanged();


            return true;
        }

        return false;
    }




    size_t CollisionShapeComponent::GetCollisionShapeCount() const
    {
        return static_cast<size_t>(this->collisionShape.getNumChildShapes());
    }


    btCollisionShape & CollisionShapeComponent::GetCollisionShapeAtIndex(size_t index)
    {
        assert(index < this->GetCollisionShapeCount());
        {
            auto shape = this->collisionShape.getChildShape(index);
            assert(shape != nullptr);
            {
                return *shape;
            }
        }
    }

    const btCollisionShape & CollisionShapeComponent::GetCollisionShapeAtIndex(size_t index) const
    {
        assert(index < this->GetCollisionShapeCount());
        {
            auto shape = this->collisionShape.getChildShape(index);
            assert(shape != nullptr);
            {
                return *shape;
            }
        }
    }


    void CollisionShapeComponent::SetCollisionFilter(short group, short mask)
    {
        if (this->collisionGroup != group || this->collisionMask != mask)
        {
            this->OnPreCollisionShapeChanged();
            {
                this->collisionGroup = group;
                this->collisionMask  = mask;
            }
            this->OnPostCollisionShapeChanged();
            this->OnChanged();
        }
    }


    void CollisionShapeComponent::ApplyScaling(float x, float y, float z)
    {
        this->OnPreCollisionShapeChanged();
        {
            x = GTCore::Max(x, 0.0001f);
            y = GTCore::Max(y, 0.0001f);
            z = GTCore::Max(z, 0.0001f);

            // Now we simply apply the scaling to the shape.
            this->collisionShape.setLocalScaling(btVector3(x, y, z));

            // Changing the scale can cause issues with the offset of child shapes. We need to re-apply the unscaled offset of every child shape.
            for (int iChild = 0; iChild < this->collisionShape.getNumChildShapes(); ++iChild)
            {
                auto childShape = this->collisionShape.getChildShape(iChild);
                assert(childShape != nullptr);
                {
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(childShape->getUserPointer());
                    if (metadata != nullptr)
                    {
                        this->SetCollisionShapeOffset(static_cast<size_t>(iChild), metadata->unscaledOffset, false);        // <-- 'false' means to no post OnChanged.
                    }
                }
            }
        }
        this->OnPostCollisionShapeChanged();
    }

    void CollisionShapeComponent::ApplySceneNodeScaling()
    {
        this->ApplyScaling(this->node.GetWorldScale());
    }



    VertexArray* CollisionShapeComponent::CreateCollisionShapeMesh(bool applyNodeTransform) const
    {
        int shapeCount = this->collisionShape.getNumChildShapes();

        // The way we do things is we first build vertex arrays for each individual shape. We that combine them all into a single
        // vertex array to produce the final array.
        GTCore::Vector<VertexArray*> shapeGeometry(shapeCount);

        for (int i = 0; i < shapeCount; ++i)
        {
            auto shape = this->collisionShape.getChildShape(i);
            assert(shape != nullptr);

            auto va = VertexArrayLibrary::CreateFromShape(*shape);
            if (va != nullptr)
            {
                va->ApplyTransform(ToGLMMatrix4(this->collisionShape.getChildTransform(i)));
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

    void CollisionShapeComponent::Serialize(GTCore::Serializer &serializer) const
    {
        // We'll need to use an intermediary serializer for getting accurate sizes.
        GTCore::BasicSerializer intermediarySerializer;
        intermediarySerializer.Write(static_cast<uint32_t>(this->GetCollisionShapeCount()));
        intermediarySerializer.Write(static_cast<uint32_t>(this->collisionGroup));
        intermediarySerializer.Write(static_cast<uint32_t>(this->collisionMask));
        intermediarySerializer.Write(this->usingConvexHullsOfModel);
        

        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_CollisionShapeComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);



        for (size_t i = 0; i < this->GetCollisionShapeCount(); ++i)
        {
            auto &shape = this->GetCollisionShapeAtIndex(i);

            switch (GetCollisionShapeType(shape))
            {
            case CollisionShapeType_Box:
                {
                    auto &box = static_cast<const btBoxShape &>(shape);

                    header.id          = Serialization::ChunkID_CollisionShapeComponent_BoxShape;
                    header.version     = 1;
                    header.sizeInBytes =
                        sizeof(glm::vec3) +                         // <-- Half extents.
                        sizeof(glm::vec3) + sizeof(glm::vec3);      // <-- Unscaled offset and rotation.

                    serializer.Write(header);
                    serializer.Write(ToGLMVector3(box.getHalfExtentsWithMargin() / box.getLocalScaling()));
                    
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape.getUserPointer());
                    assert(metadata != nullptr);
                    {
                        serializer.Write(metadata->unscaledOffset);
                        serializer.Write(metadata->rotation);
                    }


                    break;
                }

            case CollisionShapeType_Sphere:
                {
                    auto &sphere = static_cast<const btSphereShape &>(shape);

                    header.id          = Serialization::ChunkID_CollisionShapeComponent_SphereShape;
                    header.version     = 1;
                    header.sizeInBytes =
                        sizeof(float) +                             // <-- Radius.
                        sizeof(glm::vec3) + sizeof(glm::vec3);      // <-- Unscaled offset and rotation.

                    serializer.Write(header);
                    serializer.Write(static_cast<float>(sphere.getRadius() / sphere.getLocalScaling().getX()));
                    
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape.getUserPointer());
                    assert(metadata != nullptr);
                    {
                        serializer.Write(metadata->unscaledOffset);
                        serializer.Write(metadata->rotation);
                    }

                    break;
                }

            case CollisionShapeType_Ellipsoid:
                {
                    auto &ellipsoid = static_cast<const btEllipsoidShape &>(shape);

                    btVector3 margin(ellipsoid.getMargin(), ellipsoid.getMargin(), ellipsoid.getMargin());

                    header.id          = Serialization::ChunkID_CollisionShapeComponent_EllipsoidShape;
                    header.version     = 1;
                    header.sizeInBytes =
                        sizeof(glm::vec3) +                         // <-- Half extents
                        sizeof(glm::vec3) + sizeof(glm::vec3);      // <-- Unscaled offset and rotation.

                    serializer.Write(header);
                    serializer.Write(ToGLMVector3((ellipsoid.getImplicitShapeDimensions() + margin) / ellipsoid.getLocalScaling()));
                    
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape.getUserPointer());
                    assert(metadata != nullptr);
                    {
                        serializer.Write(metadata->unscaledOffset);
                        serializer.Write(metadata->rotation);
                    }

                    break;
                }

            case CollisionShapeType_CylinderX:
            case CollisionShapeType_CylinderY:
            case CollisionShapeType_CylinderZ:
                {
                    auto &cylinder = static_cast<const btCylinderShape &>(shape);

                    uint32_t upAxis = static_cast<uint32_t>(cylinder.getUpAxis());

                    header.id          = Serialization::ChunkID_CollisionShapeComponent_CylinderShape;
                    header.version     = 1;
                    header.sizeInBytes =
                        sizeof(uint32_t)  +                         // <-- Axis - 0 = X, 1 = Y, 2 = Z
                        sizeof(glm::vec3) +                         // <-- Half extents
                        sizeof(glm::vec3) + sizeof(glm::vec3);      // <-- Unscaled offset and rotation.

                    serializer.Write(header);
                    serializer.Write(upAxis);
                    serializer.Write(ToGLMVector3(cylinder.getHalfExtentsWithMargin() / cylinder.getLocalScaling()));
                    
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape.getUserPointer());
                    assert(metadata != nullptr);
                    {
                        serializer.Write(metadata->unscaledOffset);
                        serializer.Write(metadata->rotation);
                    }

                    break;
                }

            case CollisionShapeType_CapsuleX:
            case CollisionShapeType_CapsuleY:
            case CollisionShapeType_CapsuleZ:
                {
                    auto &capsule = static_cast<const btCapsuleShape &>(shape);

                    uint32_t upAxis     = static_cast<uint32_t>(capsule.getUpAxis());
                    uint32_t radiusAxis = (upAxis + 2) % 3;

                    header.id          = Serialization::ChunkID_CollisionShapeComponent_CapsuleShape;
                    header.version     = 1;
                    header.sizeInBytes =
                        sizeof(uint32_t) +                          // <-- Axis - 0 = X, 1 = Y, 2 = Z
                        sizeof(float)    +                          // <-- Radius
                        sizeof(float)    +                          // <-- Height
                        sizeof(glm::vec3) + sizeof(glm::vec3);      // <-- Unscaled offset and rotation.

                    serializer.Write(header);
                    serializer.Write(upAxis);
                    serializer.Write(static_cast<float>(capsule.getRadius()            / capsule.getLocalScaling()[radiusAxis]));
                    serializer.Write(static_cast<float>(capsule.getHalfHeight() * 2.0f / capsule.getLocalScaling()[upAxis]));
                    
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape.getUserPointer());
                    assert(metadata != nullptr);
                    {
                        serializer.Write(metadata->unscaledOffset);
                        serializer.Write(metadata->rotation);
                    }

                    break;
                }

            case CollisionShapeType_ConvexHull:
                {
                    auto &convexHull = static_cast<const btConvexHullShape &>(shape);

                    header.id          = Serialization::ChunkID_CollisionShapeComponent_ConvexHullShape;
                    header.version     = 1;
                    header.sizeInBytes =
                        sizeof(uint32_t)  +                                     // <-- Vertex count.
                        sizeof(glm::vec3) * convexHull.getNumVertices() +       // <-- Each vertex.
                        sizeof(float);                                          // <-- Margin. Important for convex hulls.

                    serializer.Write(header);
                    serializer.Write(static_cast<uint32_t>(convexHull.getNumVertices()));

                    for (int iVertex = 0; iVertex < convexHull.getNumVertices(); ++iVertex)
                    {
                        btVector3 vertex;
                        convexHull.getVertex(iVertex, vertex);

                        // We want this unscaled.
                        vertex = vertex / convexHull.getLocalScaling();

                        serializer.Write(ToGLMVector3(vertex));
                    }

                    serializer.Write(static_cast<float>(convexHull.getMargin()));

                    break;
                }

            case CollisionShapeType_ModelConvexHulls:
                {
                    header.id          = Serialization::ChunkID_CollisionShapeComponent_ModelConvexHullShape;
                    header.version     = 1;
                    header.sizeInBytes =
                        sizeof(float) +                             // <-- Margin.
                        sizeof(glm::vec3) + sizeof(glm::vec3);      // <-- Unscaled offset and rotation.

                    serializer.Write(header);
                    serializer.Write(shape.getMargin());
                    
                    auto metadata = reinterpret_cast<ChildCollisionShapeMetadata*>(shape.getUserPointer());
                    assert(metadata != nullptr);
                    {
                        serializer.Write(metadata->unscaledOffset);
                        serializer.Write(metadata->rotation);
                    }

                    break;
                }


            case CollisionShapeType_None:
            default: break;
            }
        }
    }

    void CollisionShapeComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        // Before deserializing, we need to revert the scaling back to 1.0, 1.0, 1.0f. If we don't do this, the scale won't be
        // be set correctly because of the way the shapes are used.
        this->ApplyScaling(1.0f, 1.0f, 1.0f);



        Serialization::ChunkHeader header;
        deserializer.Read(header);

        uint32_t deserializedShapeCount;
        uint32_t deserializedCollisionGroup;
        uint32_t deserializedCollisionMask;
        bool     deserializedUsingConvexHullsOfModel;

        // The first chunk must the main one. It will contain the shape count.
        if (header.id == Serialization::ChunkID_CollisionShapeComponent_Main)
        {
            switch (header.version)
            {
            case 1:
                {
                    deserializer.Read(deserializedShapeCount);
                    deserializer.Read(deserializedCollisionGroup);
                    deserializer.Read(deserializedCollisionMask);
                    deserializer.Read(deserializedUsingConvexHullsOfModel);

                    break;
                }


            default:
                {
                    GTEngine::PostError("Error deserializing  CollisionShapeComponent. Main chunk version (%d) is unknown. This chunk has been skipped. Nothing has been modified.", header.version);

                    deserializer.Seek(header.sizeInBytes);
                    return;
                }
            }
        }
        else
        {
            GTEngine::PostError("Error deserializing CollisionShapeComponent. The first chunk must the main chunk, but instead it is (%d). This chunk has been skipped. Nothing has been modified.", header.id);

            deserializer.Seek(header.sizeInBytes);
            return;
        }



        // The next chunks will be the shapes. The old shapes need to be removed.
        this->RemoveAllCollisionShapes(false);


        for (uint32_t i = 0; i < deserializedShapeCount; ++i)
        {
            deserializer.Read(header);

            switch (header.id)
            {
            case Serialization::ChunkID_CollisionShapeComponent_BoxShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            glm::vec3 halfExtents;
                            glm::vec3 unscaledOffset;
                            glm::vec3 rotation;

                            deserializer.Read(halfExtents);
                            deserializer.Read(unscaledOffset);
                            deserializer.Read(rotation);

                            btTransform childTransform;
                            childTransform.setBasis(Math::btMatrix3x3_cast(glm::quat(glm::radians(rotation))));
                            childTransform.setOrigin(Math::btVector3_cast(unscaledOffset));

                            auto childShape = new btBoxShape(ToBulletVector3(halfExtents));
                            childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                            this->collisionShape.addChildShape(childTransform, childShape);

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing CollisionShapeComponent. Box shape chunk version (%d) is unknown. Skipping.", header.version);

                            deserializer.Seek(header.sizeInBytes);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_CollisionShapeComponent_SphereShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            float     radius;
                            glm::vec3 unscaledOffset;
                            glm::vec3 rotation;

                            deserializer.Read(radius);
                            deserializer.Read(unscaledOffset);
                            deserializer.Read(rotation);

                            btTransform childTransform;
                            childTransform.setBasis(Math::btMatrix3x3_cast(glm::quat(glm::radians(rotation))));
                            childTransform.setOrigin(Math::btVector3_cast(unscaledOffset));

                            auto childShape = new btSphereShape(radius);
                            childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                            this->collisionShape.addChildShape(childTransform, childShape);

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing CollisionShapeComponent. Sphere shape chunk version (%d) is unknown. Skipping.", header.version);

                            deserializer.Seek(header.sizeInBytes);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_CollisionShapeComponent_EllipsoidShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            glm::vec3 halfExtents;
                            glm::vec3 unscaledOffset;
                            glm::vec3 rotation;

                            deserializer.Read(halfExtents);
                            deserializer.Read(unscaledOffset);
                            deserializer.Read(rotation);

                            btTransform childTransform;
                            childTransform.setBasis(Math::btMatrix3x3_cast(glm::quat(glm::radians(rotation))));
                            childTransform.setOrigin(Math::btVector3_cast(unscaledOffset));

                            auto childShape = new btEllipsoidShape(ToBulletVector3(halfExtents));
                            childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                            this->collisionShape.addChildShape(childTransform, childShape);

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing CollisionShapeComponent. Ellipsoid shape chunk version (%d) is unknown. Skipping.", header.version);

                            deserializer.Seek(header.sizeInBytes);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_CollisionShapeComponent_CylinderShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            uint32_t  upAxis;
                            glm::vec3 halfExtents;
                            glm::vec3 unscaledOffset;
                            glm::vec3 rotation;

                            deserializer.Read(upAxis);
                            deserializer.Read(halfExtents);
                            deserializer.Read(unscaledOffset);
                            deserializer.Read(rotation);

                            btTransform childTransform;
                            childTransform.setBasis(Math::btMatrix3x3_cast(glm::quat(glm::radians(rotation))));
                            childTransform.setOrigin(Math::btVector3_cast(unscaledOffset));

                            if (upAxis == 0)
                            {
                                auto childShape = new btCylinderShapeX(ToBulletVector3(halfExtents));
                                childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                                this->collisionShape.addChildShape(childTransform, childShape);
                            }
                            else if (upAxis == 1)
                            {
                                auto childShape = new btCylinderShape(ToBulletVector3(halfExtents));
                                childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                                this->collisionShape.addChildShape(childTransform, childShape);
                            }
                            else
                            {
                                auto childShape = new btCylinderShapeZ(ToBulletVector3(halfExtents));
                                childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                                this->collisionShape.addChildShape(childTransform, childShape);
                            }

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing CollisionShapeComponent. Box shape chunk version (%d) is unknown. Skipping.", header.version);

                            deserializer.Seek(header.sizeInBytes);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_CollisionShapeComponent_CapsuleShape:
                {
                    switch (header.version)
                    {
                    case 1:
                        {
                            uint32_t upAxis;
                            float    radius;
                            float    height;
                            glm::vec3 unscaledOffset;
                            glm::vec3 rotation;

                            deserializer.Read(upAxis);
                            deserializer.Read(radius);
                            deserializer.Read(height);
                            deserializer.Read(unscaledOffset);
                            deserializer.Read(rotation);

                            btTransform childTransform;
                            childTransform.setBasis(Math::btMatrix3x3_cast(glm::quat(glm::radians(rotation))));
                            childTransform.setOrigin(Math::btVector3_cast(unscaledOffset));

                            if (upAxis == 0)
                            {
                                auto childShape = new btCapsuleShapeX(radius, height);
                                childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                                this->collisionShape.addChildShape(childTransform, childShape);
                            }
                            else if (upAxis == 1)
                            {
                                auto childShape = new btCapsuleShape(radius, height);
                                childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                                this->collisionShape.addChildShape(childTransform, childShape);
                            }
                            else
                            {
                                auto childShape = new btCapsuleShapeZ(radius, height);
                                childShape->setUserPointer(new ChildCollisionShapeMetadata(unscaledOffset, rotation));

                                this->collisionShape.addChildShape(childTransform, childShape);
                            }

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing CollisionShapeComponent. Box shape chunk version (%d) is unknown. Skipping.", header.version);

                            deserializer.Seek(header.sizeInBytes);
                            break;
                        }
                    }

                    break;
                }


            case Serialization::ChunkID_CollisionShapeComponent_ConvexHullShape:
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


                            this->collisionShape.addChildShape(btTransform::getIdentity(), new btConvexHullShape(&vertices[0].x, vertexCount, 12));


                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing CollisionShapeComponent. Convex hull chunk version (%d) is unknown. Skipping.", header.version);

                            deserializer.Seek(header.sizeInBytes);
                            break;
                        }
                    }

                    break;
                }


            case Serialization::ChunkID_CollisionShapeComponent_ModelConvexHullShape:
                {
                    if (header.version == 1)
                    {
                        float margin;
                        deserializer.Read(margin);

                        this->AddModelConvexHullsCollisionShape(margin);
                    }
                    else
                    {
                        GTEngine::Log("Error deserializing CollisionShapeComponent. Model convex hull chunk version (%d) is unknown. Skipping.", header.version);
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }


            default:
                {
                    GTEngine::Log("Error deserializing CollisionShapeComponent. Unknown shape chunk (%d). Skipping.", header.id);

                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }
    }



    //////////////////////////////////////////////////
    // Private

    void CollisionShapeComponent::AddCollisionShape(btCollisionShape* shape, float offsetX, float offsetY, float offsetZ)
    {
        this->OnPreCollisionShapeChanged();
        {
            /// The child collision shape needs metadata.
            shape->setUserPointer(new ChildCollisionShapeMetadata(glm::vec3(offsetX, offsetY, offsetZ)));


            // Before adding the shape, we need to temporarily reset the scale back to 1,1,1. If we don't do this, the child shape will
            // not have the correct scale applied to it when we update the scale properly.
            this->collisionShape.setLocalScaling(btVector3(1.0f, 1.0f, 1.0f));

            // All we need to do is add the new shape to the compound shape...
            this->collisionShape.addChildShape(btTransform(btMatrix3x3::getIdentity(), btVector3(offsetX, offsetY, offsetZ)), shape);

            // We need to make sure the shape is scaled correctly.
            glm::vec3 nodeScale = glm::max(glm::vec3(0.0001f), this->node.GetWorldScale());
            this->collisionShape.setLocalScaling(btVector3(nodeScale.x, nodeScale.y, nodeScale.z));
        }
        this->OnPostCollisionShapeChanged();
        this->OnChanged();
    }


    void CollisionShapeComponent::AddModelConvexHullsToCompoundShape(btCompoundShape* shape, float margin)
    {
        assert(shape != nullptr);
        {
            auto modelComponent = this->node.GetComponent<ModelComponent>();
            if (modelComponent != nullptr)
            {
                auto model = modelComponent->GetModel();
                if (model != nullptr)
                {
                    auto &definition = model->GetDefinition();

                    for (size_t iConvexHull = 0; iConvexHull < definition.convexHulls.count; ++iConvexHull)
                    {
                        auto convexHull = definition.convexHulls[iConvexHull];
                        assert(convexHull != nullptr);
                        {
                            auto convexHullShape = new btConvexHullShape(static_cast<const btScalar*>(convexHull->GetVertices()), convexHull->GetVertexCount(), 12);
                            convexHullShape->setMargin(margin);

                            shape->addChildShape(btTransform::getIdentity(), convexHullShape);
                        }
                    }
                }
            }

            // The local AABB must be recalculated because we passed 'false' to the shape's constructor (no dynamic recalculating of the local AABB).
            shape->recalculateLocalAabb();
        }
    }
}