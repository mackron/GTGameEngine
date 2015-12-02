// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Components/CollisionShapeComponent.hpp>
#include <GTGE/SceneNode.hpp>
#include <GTGE/GTEngine.hpp>
#undef max

namespace GT
{
    #define GTENGINE_MIN_CONVEX_HULL_MARGIN     0.005f


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
          collisionGroup(1),       collisionMask(-1),
          collisionGroupStrings(), collisionGroupMaskStrings()
    {
        collisionGroupStrings.PushBack("All");
        collisionGroupMaskStrings.PushBack("All");
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


    void CollisionShapeComponent::AddConvexHullCollisionShape(const float* points, unsigned int pointCount, unsigned int stride, float margin)
    {
        if (margin < GTENGINE_MIN_CONVEX_HULL_MARGIN) margin = GTENGINE_MIN_CONVEX_HULL_MARGIN;

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
        if (margin < GTENGINE_MIN_CONVEX_HULL_MARGIN) margin = GTENGINE_MIN_CONVEX_HULL_MARGIN;

        // Even if we don't have an attached model comonent, we still want to add the collision shape.
        auto shape = new btCompoundShape(true);
        shape->setMargin(margin);
        this->AddModelConvexHullsToCompoundShape(shape, margin);

        this->AddCollisionShape(shape, offsetX, offsetY, offsetZ);
    }


    void CollisionShapeComponent::RefreshModelConvexHullsShapes()
    {
        this->OnPreCollisionShapeChanged();
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
        this->OnPostCollisionShapeChanged();
    }

    void CollisionShapeComponent::RemoveAllCollisionShapes(bool postEvent)
    {
        if (postEvent) this->OnPreCollisionShapeChanged();
        {
            // All children need to be removed from the shape.
            while (this->collisionShape.getNumChildShapes() > 0)
            {
                this->RemoveCollisionShapeAtIndex(0, false);        // <-- 'false' is important here - it prevents posting Change events.
            }
        }
        if (postEvent) this->OnPostCollisionShapeChanged();
        if (postEvent) this->OnChanged();
    }

    void CollisionShapeComponent::RemoveCollisionShapeAtIndex(size_t index, bool postEvent)
    {
        if (postEvent) this->OnPreCollisionShapeChanged();
        {
            // All children need to be removed from the shape.
            auto child = this->collisionShape.getChildShape(static_cast<int>(index));
            delete reinterpret_cast<ChildCollisionShapeMetadata*>(child->getUserPointer());

            // If the child is a compound shape, we assert that it is a model convex hulls shape, which means we need to also delete it's children.
            if (child->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
            {
                while (static_cast<btCompoundShape*>(child)->getNumChildShapes() > 0)
                {
                    auto convexHullChildShape = static_cast<btCompoundShape*>(child)->getChildShape(0);

                    static_cast<btCompoundShape*>(child)->removeChildShapeByIndex(0);
                    delete convexHullChildShape;
                }
            }
            
            this->collisionShape.removeChildShapeByIndex(static_cast<int>(index));
            delete child;
        }
        if (postEvent) this->OnPostCollisionShapeChanged();
        if (postEvent) this->OnChanged();
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
        if (postOnChanged) this->OnPreCollisionShapeChanged();
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
        if (postOnChanged) this->OnPostCollisionShapeChanged();
        if (postOnChanged) this->OnChanged();
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

    bool CollisionShapeComponent::SetModelConvexHullsMargins(size_t index, float margin)
    {
        if (margin < GTENGINE_MIN_CONVEX_HULL_MARGIN) margin = GTENGINE_MIN_CONVEX_HULL_MARGIN;

        auto type = GetCollisionShapeType(this->collisionShape.getChildShape(index));

        if (type == CollisionShapeType_ModelConvexHulls)
        {
            this->OnPreCollisionShapeChanged();
            {
                auto convexHullsParentShape = static_cast<btCompoundShape*>(this->collisionShape.getChildShape(index));
                assert(convexHullsParentShape != nullptr);
                {
                    convexHullsParentShape->setMargin(margin);

                    for (int iChildShape = 0; iChildShape < convexHullsParentShape->getNumChildShapes(); ++iChildShape)
                    {
                        auto childShape = convexHullsParentShape->getChildShape(iChildShape);
                        assert(childShape != nullptr);
                        {
                            childShape->setMargin(margin);
                        }
                    }
                }
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



    void CollisionShapeComponent::AddCollisionGroup(const char* groupName)
    {
        if (!this->collisionGroupStrings.Exists(groupName))
        {
            this->collisionGroupStrings.PushBack(groupName);
            this->UpdateCollisionFilter();      // <-- This is O(n). Can be optimized if it becomes an issue. Shouldn't be a problem in practice.
        }
    }

    void CollisionShapeComponent::RemoveCollisionGroup(const char* groupName)
    {
        this->collisionGroupStrings.RemoveFirstOccuranceOf(groupName);
        this->UpdateCollisionFilter();          // <-- This is O(n). Can be optimized if it becomes an issue. Shouldn't be a problem in practice.
    }

    void CollisionShapeComponent::RemoveCollisionGroupByIndex(size_t groupIndex)
    {
        this->collisionGroupStrings.Remove(groupIndex);
        this->UpdateCollisionFilter();          // <-- This is O(n). Can be optimized if it becomes an issue. Shouldn't be a problem in practice.
    }

    size_t CollisionShapeComponent::GetCollisionGroupCount() const
    {
        return this->collisionGroupStrings.count;
    }

    const char* CollisionShapeComponent::GetCollisionGroup(size_t groupIndex) const
    {
        return this->collisionGroupStrings[groupIndex].c_str();
    }


    void CollisionShapeComponent::AddCollisionGroupMask(const char* groupName)
    {
        if (!this->collisionGroupMaskStrings.Exists(groupName))
        {
            this->collisionGroupMaskStrings.PushBack(groupName);
            this->UpdateCollisionFilter();      // <-- This is O(n). Can be optimized if it becomes an issue. Shouldn't be a problem in practice.
        }
    }

    void CollisionShapeComponent::RemoveCollisionGroupMask(const char* groupName)
    {
        this->collisionGroupMaskStrings.RemoveFirstOccuranceOf(groupName);
        this->UpdateCollisionFilter();          // <-- This is O(n). Can be optimized if it becomes an issue. Shouldn't be a problem in practice.
    }

    void CollisionShapeComponent::RemoveCollisionGroupMaskByIndex(size_t groupIndex)
    {
        this->collisionGroupMaskStrings.Remove(groupIndex);
        this->UpdateCollisionFilter();          // <-- This is O(n). Can be optimized if it becomes an issue. Shouldn't be a problem in practice.
    }

    size_t CollisionShapeComponent::GetCollisionGroupMaskCount() const
    {
        return this->collisionGroupMaskStrings.count;
    }

    const char* CollisionShapeComponent::GetCollisionGroupMask(size_t groupIndex) const
    {
        return this->collisionGroupMaskStrings[groupIndex].c_str();
    }


    void CollisionShapeComponent::UpdateCollisionFilter()
    {
        short newCollisionGroup = 0;
        short newCollisionMask  = 0;
        
        String base("Game.CollisionGroups.");
        
        for (size_t iGroup = 0; iGroup < this->collisionGroupStrings.count; ++iGroup)
        {
            int group = GlobalGame->GetScript().GetInteger((base + this->collisionGroupStrings[iGroup]).c_str());
            if (group == -1)
            {
                newCollisionGroup = static_cast<short>(-1);
            }
            else if (group == 0)
            {
                newCollisionGroup = 0;
            }
            else
            {
                newCollisionGroup |= static_cast<short>(1 << (group - 1));
            }
        }
        
        for (size_t iMask = 0; iMask < this->collisionGroupMaskStrings.count; ++iMask)
        {
            int group = GlobalGame->GetScript().GetInteger((base + this->collisionGroupMaskStrings[iMask]).c_str());
            if (group == -1)
            {
                newCollisionMask = -1;
            }
            else if (group == 0)
            {
                newCollisionMask = 0;
            }
            else
            {
                newCollisionMask |= static_cast<short>(1 << (group - 1));
            }
        }
        
        this->SetCollisionFilter(newCollisionGroup, newCollisionMask);
    }



    void CollisionShapeComponent::ApplyScaling(float x, float y, float z)
    {
        this->OnPreCollisionShapeChanged();
        {
            x = Max(x, 0.0001f);
            y = Max(y, 0.0001f);
            z = Max(z, 0.0001f);

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



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void CollisionShapeComponent::Serialize(Serializer &serializer) const
    {
        // We'll need to use an intermediary serializer for getting accurate sizes.
        BasicSerializer intermediarySerializer;
        intermediarySerializer.Write(static_cast<uint32_t>(this->GetCollisionShapeCount()));
        intermediarySerializer.Write(static_cast<uint32_t>(this->collisionGroup));
        intermediarySerializer.Write(static_cast<uint32_t>(this->collisionMask));
        
        // TODO: Clean up this boolean backwards compatibility thing.
        //
        // The boolean was previously just a dummy value for backwards compatibility. We're still going to use this for
        // the sake of backwards compatibility, only this time we're going to set it to true, which will indicate there
        // is some collision strings that need to be read.
        intermediarySerializer.Write(true);
        
        intermediarySerializer.Write(static_cast<uint32_t>(this->collisionGroupStrings.count));
        for (size_t iGroup = 0; iGroup < this->collisionGroupStrings.count; ++iGroup)
        {
            intermediarySerializer.WriteString(this->collisionGroupStrings[iGroup]);
        }
        
        intermediarySerializer.Write(static_cast<uint32_t>(this->collisionGroupMaskStrings.count));
        for (size_t iGroup = 0; iGroup < this->collisionGroupMaskStrings.count; ++iGroup)
        {
            intermediarySerializer.WriteString(this->collisionGroupMaskStrings[iGroup]);
        }
        
        

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
                        // We want this unscaled.
                        serializer.Write(ToGLMVector3(convexHull.getUnscaledPoints()[iVertex]));
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

    void CollisionShapeComponent::Deserialize(Deserializer &deserializer)
    {
        // Before deserializing, we need to revert the scaling back to 1.0, 1.0, 1.0f. If we don't do this, the scale won't be
        // be set correctly because of the way the shapes are used.
        this->ApplyScaling(1.0f, 1.0f, 1.0f);



        Serialization::ChunkHeader header;
        deserializer.Read(header);

        uint32_t deserializedShapeCount;
        uint32_t deserializedCollisionGroup;
        uint32_t deserializedCollisionMask;
        Vector<String> deserializedCollisionGroupStrings;
        Vector<String> deserializedCollisionGroupMaskStrings;

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

                    // The next boolean is a backwards compatibility flag. If set to true, it means there are string collision groups that
                    // need to be read.
                    bool readCollisionGroupStrings;
                    deserializer.Read(readCollisionGroupStrings);
                    
                    if (readCollisionGroupStrings)
                    {
                        uint32_t collisionGroupStringsCount;
                        deserializer.Read(collisionGroupStringsCount);
                        
                        for (uint32_t iGroup = 0; iGroup < collisionGroupStringsCount; ++iGroup)
                        {
                            String group;
                            deserializer.ReadString(group);
                            
                            deserializedCollisionGroupStrings.PushBack(group);
                        }
                        
                        
                        uint32_t collisionGroupMaskStringsCount;
                        deserializer.Read(collisionGroupMaskStringsCount);
                        
                        for (uint32_t iGroup = 0; iGroup < collisionGroupMaskStringsCount; ++iGroup)
                        {
                            String group;
                            deserializer.ReadString(group);
                            
                            deserializedCollisionGroupMaskStrings.PushBack(group);
                        }
                    }
                    

                    break;
                }


            default:
                {
                    g_EngineContext->LogErrorf("Error deserializing  CollisionShapeComponent. Main chunk version (%d) is unknown. This chunk has been skipped. Nothing has been modified.", header.version);

                    deserializer.Seek(header.sizeInBytes);
                    return;
                }
            }
        }
        else
        {
            g_EngineContext->LogErrorf("Error deserializing CollisionShapeComponent. The first chunk must the main chunk, but instead it is (%d). This chunk has been skipped. Nothing has been modified.", header.id);

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
                            g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Box shape chunk version (%d) is unknown. Skipping.", header.version);

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
                            g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Sphere shape chunk version (%d) is unknown. Skipping.", header.version);

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
                            g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Ellipsoid shape chunk version (%d) is unknown. Skipping.", header.version);

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
                            g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Box shape chunk version (%d) is unknown. Skipping.", header.version);

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
                            g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Box shape chunk version (%d) is unknown. Skipping.", header.version);

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
                            Vector<glm::vec3> vertices;
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
                            g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Convex hull chunk version (%d) is unknown. Skipping.", header.version);

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
                        glm::vec3 unscaledOffset;
                        glm::vec3 rotation;

                        deserializer.Read(margin);
                        deserializer.Read(unscaledOffset);
                        deserializer.Read(rotation);

                        this->AddModelConvexHullsCollisionShape(margin);
                    }
                    else
                    {
                        g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Model convex hull chunk version (%d) is unknown. Skipping.", header.version);
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }


            default:
                {
                    g_EngineContext->Logf("Error deserializing CollisionShapeComponent. Unknown shape chunk (%d). Skipping.", header.id);

                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }

        
        // Can't forget to set the collision filter.
        this->SetCollisionFilter(static_cast<short>(deserializedCollisionGroup), static_cast<short>(deserializedCollisionMask));
        
        // The collision group strings need to be set, too.
        this->collisionGroupStrings     = deserializedCollisionGroupStrings;
        this->collisionGroupMaskStrings = deserializedCollisionGroupMaskStrings;
        this->UpdateCollisionFilter();
    }

    void CollisionShapeComponent::OnPostSceneNodeDeserialized()
    {
        this->RefreshModelConvexHullsShapes();
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

                    // Before adding the children, what we do is set the local scaling to 1, then restore it after the children are added. This
                    // will properly preserve the scaling.
                    btVector3 localScaling = shape->getLocalScaling();
                    shape->setLocalScaling(btVector3(1.0f, 1.0f, 1.0f));

                    for (size_t iConvexHull = 0; iConvexHull < definition.m_convexHulls.count; ++iConvexHull)
                    {
                        auto convexHull = definition.m_convexHulls[iConvexHull];
                        assert(convexHull != nullptr);
                        {
                            auto convexHullShape = new btConvexHullShape(static_cast<const btScalar*>(convexHull->GetVertices()), convexHull->GetVertexCount(), 12);
                            convexHullShape->setMargin(margin);

                            shape->addChildShape(btTransform::getIdentity(), convexHullShape);
                        }
                    }

                    shape->setLocalScaling(localScaling);
                }
            }

            // The local AABB must be recalculated because we may have passed 'false' to the shape's constructor (no dynamic recalculating of the local AABB).
            shape->recalculateLocalAabb();
        }
    }
}
