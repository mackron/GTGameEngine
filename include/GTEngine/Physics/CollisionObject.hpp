// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Physics_CollisionObject_hpp_
#define __GTEngine_Physics_CollisionObject_hpp_

#include "Bullet.hpp"

namespace GT
{
    class BaseCollisionWorld;

    /// GTEngine collision object class.
    ///
    /// The main reason for the existence of this class is so we can keep track of the world it is part of and then remove
    /// it from the world during destruction. This is our prefered way of doing things.
    class CollisionObject : public btCollisionObject
    {
    public:

        /// Constructor.
        CollisionObject();

        /// Destructor.
        virtual ~CollisionObject();


        /// Simple world setter.
        ///
        /// @remarks
        ///     This does not actually add the object to the world, but instead just sets the internal pointer.
        void SetWorld(BaseCollisionWorld* world);

        /// Retrieves a pointer to the world these body is part of.
        BaseCollisionWorld* GetWorld();


    private:

        /// A pointer to the world that owns this object.
        BaseCollisionWorld* world;


    private:
        CollisionObject(const CollisionObject &);
        CollisionObject & operator=(const CollisionObject &);
    };
}

#endif
