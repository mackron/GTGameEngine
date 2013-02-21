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






    private:
    };
}

#endif