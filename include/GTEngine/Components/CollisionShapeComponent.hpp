// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_CollisionShapeComponent_hpp_
#define __GTEngine_CollisionShapeComponent_hpp_

#include "../Component.hpp"
#include "../Physics.hpp"
#include "../ConvexHull.hpp"

namespace GTEngine
{
    // Base class for components needing a collision shape.
    //
    // Classes inheritting from this class will need to setup the standard component properties manually.
    //
    // This class will call virtual methods when a certain action needs to take place such as removing and adding the object from the world.
    class CollisionShapeComponent : public GTEngine::Component
    {
    public:

        /// Constructor.
        CollisionShapeComponent(SceneNode &node);

        /// Destructor.
        virtual ~CollisionShapeComponent();



        /// Adds a cube collision shape.
        void AddBoxCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        
        /// Adds a sphere collision shape.
        void AddSphereCollisionShape(float radius, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds an elipsoid collision shape.
        void AddEllipsoidCollisionShape(float radiusX, float radiusY, float radiusZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds a cylinder collision shape.
        void AddCylinderXCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCylinderYCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCylinderZCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds a capsule collision shape.
        void AddCapsuleXCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCapsuleYCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCapsuleZCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds a static plane shape. This does not move with the node.
        void AddStaticPlaneShape(float a, float b, float c, float d);


        /// Adds a convex hull shape.
        ///
        /// @param points     [in] A pointer to the buffer containing the points to build the hull from.
        /// @param pointCount [in] The number of points in the buffer.
        /// @param stride     [in] The vertex stride.
        void AddConvexHullShape(const float* points, size_t pointCount, size_t stride, float margin = 0.0);
        void AddConvexHullShape(const ConvexHull &convexHull, float margin = 0.0);

        /// Sets the collision shapes to the convex hulls of the given model.
        ///
        /// @param model  [in] A reference to the model whose convex hulls are being added.
        /// @param margin [in] The margin to apply. This default to 0.0, but it may change later if we have issues.
        ///
        /// @remarks
        ///     This removes all already attached shapes before adding the new ones.
        void SetCollisionShapesToModelConvexHulls(const Model &model, float margin = 0.0f);

        /// Sets the collision shapes to the convex hulls of the model of the attached Model component, if any.
        ///
        /// @param margin [in] The margin to apply.
        ///
        /// @remarks
        ///     This removes all already attached shapes before adding the new ones.
        void SetCollisionShapesToModelConvexHulls(float margin = 0.0f);


        /// Removes every collision shape.
        void RemoveAllCollisionShapes(bool postEvent = true);

        /// Removes the collision shape at the given index.
        ///
        /// @param index [in] The index of the shape to remove.
        void RemoveCollisionShapeAtIndex(size_t index);



        /// Sets the offset to use for the shape at the given index.
        ///
        /// @param index  [in] The index of the shape whose offset is being changed.
        /// @param offset [in] The new offset.
        void SetCollisionShapeOffset(size_t index, float offsetX, float offsetY, float offsetZ);

        /// Sets the half extents of the box shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a box, this method will fail and return false.
        bool SetBoxCollisionShapeHalfExtents(size_t index, float halfX, float halfY, float halfZ);

        /// Sets the radius of the sphere shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a sphere, this method will fail and return false.
        bool SetSphereCollisionShapeRadius(size_t index, float radius);

        /// Sets the radius of the ellipsoid shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not an ellipsoid, this method will fail and return false.
        bool SetEllipsoidCollisionShapeRadius(size_t index, float radiusX, float radiusY, float radiusZ);

        /// Sets the half extents of the cylinder shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a cylinder, this method will fail and return false.
        bool SetCylinderCollisionShapeHalfExtents(size_t index, float halfX, float halfY, float halfZ);

        /// Sets the half extents of the capsule shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a capsule, this method will fail and return false.
        bool SetCapsuleCollisionShapeSize(size_t index, float radius, float height);



        /// Retrieves a reference to the main collision shape.
              btCompoundShape & GetCollisionShape()       { return this->collisionShape; }
        const btCompoundShape & GetCollisionShape() const { return this->collisionShape; }


        /// Retrieves the number of shapes attached to the main shape.
        size_t GetCollisionShapeCount() const;

        /// Retrieves a pointer to the shape at the given index.
        ///
        /// @param index [in] The index of the shape to retrieve.
        ///
        /// @remarks
        ///     This method asserts that the index is valid and the shape is non-null. If this is not the case, it probably means a bug.
              btCollisionShape & GetCollisionShapeAtIndex(size_t index);
        const btCollisionShape & GetCollisionShapeAtIndex(size_t index) const;




        /// Sets the collision group and masks to use with the scene node.
        /// @param group [in] The collision group(s) this scene node is part of.
        /// @param mask  [in] A mask of collision groups that this scene node will collide with.
        ///
        /// @remarks
        ///     Note that changing the collision filter requires that the internal dynamics objects are removed from the internal dynamics world
        ///     and then re-added. Thus, don't call this method too frequently.
        void SetCollisionFilter(short group, short mask);

        /// Retrieves the collision group as specified in SetCollisionFilter().
        short GetCollisionGroup() const { return this->collisionGroup; }
        
        /// Retrieves the collision mask as specified in SetCollisionFilter().
        short GetCollisionMask() const { return this->collisionMask; }


        /// Determines whether or not the convex hulls of a model is being used for the collision shape.
        bool IsUsingConvexHullsFromModel() const { return this->usingConvexHullsOfModel; }



        /// Applies a scale to the collision shape.
        void ApplyScaling(float x, float y, float z);
        void ApplyScaling(const glm::vec3 &scale) { this->ApplyScaling(scale.x, scale.y, scale.z); }

        /// Applies the scaling from the scene node.
        void ApplySceneNodeScaling();


        /// Creates a vertex array containing the collision mesh vertices.
        ///
        /// @param applyNodeTransform [in] Whether or not the scene node's world transformation should be applied.
        ///
        /// @remarks
        ///     This only contains vertex positions. No texture coordinates. No normals.
        ///     @par
        ///     This is useful for use by Recast (for navigation mesh generation).
        VertexArray* CreateCollisionShapeMesh(bool applyNodeTransform = false) const;



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the collision shapes.
        ///
        /// @remarks
        ///     This will be called from inheritted classes.
        virtual void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the collision shapes.
        ///
        /// @remarks
        ///     This will be called from inheritted classes. This will not call OnPreCollisionShapeChanged() or OnPostCollisionShapeChanged().
        virtual void Deserialize(GTCore::Deserializer &deserializer);


    private:

        /// Helper method for generically adding a collision shape.
        void AddCollisionShape(btCollisionShape* shape, float offsetX, float offsetY, float offsetZ);



    protected:

        /////////////////////////////////////////////
        // Protected Virtual Methods
        //
        // These need to be implemented by the derived classes. They are called when an operation is performed with the collision shapes.

        /// Called just before the main collision shape is modified.
        virtual void OnPreCollisionShapeChanged() {};

        /// Called just after the main collision shape is modified.
        virtual void OnPostCollisionShapeChanged() {};



    protected:

        /// The collision shape. This is always a compound shape.
        btCompoundShape collisionShape;

        /// The collision group the scene node is part of. This is set with SetCollisionFilter(). Defaults to 1.
        short collisionGroup;

        /// The collision mask for controlling the collision groups the scene node can collide with. This is set with SetCollisionFilter(). Defaults to -1 (collides with everything).
        short collisionMask;

        /// Keeps track of whether or not this object is using the convex hulls from the attached model for it's convex hulls.
        bool usingConvexHullsOfModel;
    };
}

#endif