
#ifndef __GTEngine_NavigationMesh_hpp_
#define __GTEngine_NavigationMesh_hpp_

#include "Recast.h"

namespace GTEngine
{
    /// Class representing a navigation mesh.
    ///
    /// A navigation mesh is used for allowing an actor to know where it is able to move while navigating between two points.
    ///
    /// Each scene will have a navigation mesh object.
    ///
    /// The current implementation of this will require a complete rebuild of the navigation data using a scene object. In this future this
    /// will probably become more adaptive for more dynamic readjustments.
    class NavigationMesh
    {
    public:

        /// Constructor.
        NavigationMesh();

        /// Destructor.
        ~NavigationMesh();


    private:

        /// The configuration for building the Recast navmesh.
        rcConfig config;
    };
}

#endif