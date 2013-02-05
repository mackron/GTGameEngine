// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_MeshCollisionShape_hpp_
#define __GTEngine_MeshCollisionShape_hpp_

#include "Bullet.hpp"
#include "../Model.hpp"

namespace GTEngine
{
    /// A special collision shape for making it easier to do triangle mesh collision shapes from GTEngine objects.
    class MeshCollisionShape : public btGImpactMeshShape
    {
    public:

        /// Constructor.
        MeshCollisionShape();

        /// Constructs the collision shape from the mesh information contained in the given model.
        ///
        /// @param model [in] A reference to the model whose triangle information is being 
        MeshCollisionShape(const Model &model);

        /// Destructor.
        ~MeshCollisionShape();


        /// Resets the collision shape.
        void Reset();

        /// Adds to the collision shape the vertex information of the given model.
        ///
        /// @param model [in] The model whose vertex data is being added.
        void AddFromModel(const Model &model);



    private:

        /// Constructor for constructing the shape from a Bullet mesh interface.
        MeshCollisionShape(btTriangleIndexVertexArray* meshInterface);
    };
}

#endif