// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_StaticMeshCollisionShape_hpp_
#define __GTEngine_StaticMeshCollisionShape_hpp_

#include "Bullet.hpp"
#include "../Model.hpp"

namespace GTEngine
{
    /// A special collision shape for making it easier to do triangle mesh collision shapes from GTEngine objects.
    class StaticMeshCollisionShape : public btBvhTriangleMeshShape
    {
    public:

        /// Constructor.
        StaticMeshCollisionShape();

        /// Constructs the collision shape from the mesh information contained in the given model.
        ///
        /// @param model [in] A reference to the model whose triangle information is being 
        StaticMeshCollisionShape(const Model &model);

        /// Destructor.
        ~StaticMeshCollisionShape();


        /// Resets the collision shape.
        void Reset();

        /// Adds to the collision shape the vertex information of the given model.
        ///
        /// @param model [in] The model whose vertex data is being added.
        void AddFromModel(const Model &model);



    private:

        /// Constructor for constructing the shape from a Bullet mesh interface.
        StaticMeshCollisionShape(btTriangleIndexVertexArray* meshInterface, bool buildBvh);
    };
}

#endif