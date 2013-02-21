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



    protected:

        /// The collision shape. This is always a compound shape.
        btCompoundShape collisionShape;
    };
}

#endif