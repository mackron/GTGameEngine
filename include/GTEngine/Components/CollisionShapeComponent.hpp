// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_CollisionShapeComponent_hpp_
#define __GTEngine_CollisionShapeComponent_hpp_

#include "../Physics.hpp"
#include "../Component.hpp"
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
        void AddCylinderXCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCylinderYCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCylinderZCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

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
        void AddConvexHullCollisionShape(const float* points, unsigned int pointCount, unsigned int stride, float margin = 0.0);
        void AddConvexHullCollisionShape(const ConvexHull &convexHull, float margin = 0.0);


        /// Adds a shape containing the convex hulls of the attached model component, if any.
        ///
        /// @param margin [in] The margin to apply to the convex hulls.
        void AddModelConvexHullsCollisionShape(float margin = 0.0f, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Refreshes model convex hull shapes based on the current state of the model component.
        ///
        /// @remarks
        ///     This will want to be done whenever the scene node is deserialized of the model definition changes.
        void RefreshModelConvexHullsShapes();


        /// Removes every collision shape.
        void RemoveAllCollisionShapes(bool postEvent = true);

        /// Removes the collision shape at the given index.
        ///
        /// @param index [in] The index of the shape to remove.
        void RemoveCollisionShapeAtIndex(size_t index, bool postEvent = true);


        /// Retrieves the unscaled offset of the shape at the given index.
        ///
        /// @param index [in] The idnex of the shape whose offset if being retrieved.
        glm::vec3 GetCollisionShapeOffset(size_t index) const;

        /// Sets the unscaled offset to use for the shape at the given index.
        ///
        /// @param index  [in] The index of the shape whose offset is being changed.
        /// @param offset [in] The new offset.
        void SetCollisionShapeOffset(size_t index, float offsetX, float offsetY, float offsetZ, bool postOnChanged = true);
        void SetCollisionShapeOffset(size_t index, const glm::vec3 &offset, bool postOnChanged = true) { this->SetCollisionShapeOffset(index, offset.x, offset.y, offset.z, postOnChanged); }


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
        bool SetCylinderCollisionShapeSize(size_t index, float radius, float length);

        /// Sets the half extents of the capsule shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a capsule, this method will fail and return false.
        bool SetCapsuleCollisionShapeSize(size_t index, float radius, float height);

        /// Sets the margins of the convex hull of the model convex hull collision shapes.
        ///
        /// @param index  [in] The index of the shape.
        /// @param margin [in] The new margin.
        bool SetModelConvexHullsMargins(size_t index, float margin);


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



        /// Adds a collision group to the shape, making the shape a member of that group.
        ///
        /// @param groupName [in] The name of the collision group to make this shape a part of.
        void AddCollisionGroup(const char* groupName);

        /// Removes a collision group from the shape, removing the shape from that group.
        ///
        /// @param groupName [in] The name of the collision group to remove this shape from.
        void RemoveCollisionGroup(const char* groupName);

        /// Removes a collision group from the shape by it's index. This will remove the shape from that group.
        ///
        /// @param groupIndex [in] The index of the collision group to remove the shape from.
        void RemoveCollisionGroupByIndex(size_t groupIndex);

        /// Retrieves the number of collision groups this shape is part of.
        size_t GetCollisionGroupCount() const;

        /// Retrieves the name of the collision group at the given index.
        ///
        /// @param groupIndex [in] The index of the collision group whose name is being retrieved.
        const char* GetCollisionGroup(size_t groupIndex) const;


        /// Adds a collision mask to the shape. This allows the shape to collide with other shapes of that group.
        ///
        /// @param groupName [in] The name of the collision group to allow this shape to collide with.
        void AddCollisionGroupMask(const char* groupName);

        /// Removes a collision group mask from the shape. This disallows the shape to collide with other shapes of that group.
        ///
        /// @param groupName [in] The name of the collision group to prevent this shape from colliding with.
        void RemoveCollisionGroupMask(const char* groupName);

        /// Removes a collision group mask from the shape by it's index. This will disallow the shape to collide with other shapes of that group.
        ///
        /// @param groupIndex [in] The index of the collision group mask to prevent this shape from colliding with.
        void RemoveCollisionGroupMaskByIndex(size_t groupIndex);

        /// Retrieves the number of collision group masks this shape is allowed to collide with.
        size_t GetCollisionGroupMaskCount() const;

        /// Retrieves the name of the collision group mask at the given index.
        ///
        /// @param groupIndex [in] The index of the collision group mask whose name is being retrieved.
        const char* GetCollisionGroupMask(size_t groupIndex) const;


        /// Updates the collision filters based on the current collision group strings.
        ///
        /// @remarks
        ///     The values are determined by the values defined in the scripting environment. The scripting environment will map an integer value
        ///     to each string. Each of these integer strings are bitwise-or'd to produce the final group and mask.s
        void UpdateCollisionFilter();



        /// Applies a scale to the collision shape.
        void ApplyScaling(float x, float y, float z);
        void ApplyScaling(const glm::vec3 &scale) { this->ApplyScaling(scale.x, scale.y, scale.z); }

        /// Applies the scaling from the scene node.
        void ApplySceneNodeScaling();



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the collision shapes.
        ///
        /// @remarks
        ///     This will be called from inheritted classes.
        virtual void Serialize(GTLib::Serializer &serializer) const;

        /// Deserializes the collision shapes.
        ///
        /// @remarks
        ///     This will be called from inheritted classes. This will not call OnPreCollisionShapeChanged() or OnPostCollisionShapeChanged().
        virtual void Deserialize(GTLib::Deserializer &deserializer);

        /// Called when the owner scene node has finished deserializing.
        ///
        /// @remarks
        ///     We need to handle this so we can update the model convex hull shapes.
        virtual void OnPostSceneNodeDeserialized();


    private:

        /// Helper method for generically adding a collision shape.
        void AddCollisionShape(btCollisionShape* shape, float offsetX, float offsetY, float offsetZ);

        /// Helper method for attaching the convex hulls of the attached model component to the given compound shape.
        void AddModelConvexHullsToCompoundShape(btCompoundShape* shape, float margin);
        

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


        /// The list of strings representing the collision groups this object is part of.
        GTLib::Vector<GTLib::String> collisionGroupStrings;

        /// The list of strings representing the collision groups this object will collide with.
        GTLib::Vector<GTLib::String> collisionGroupMaskStrings;
    };
}

#endif
