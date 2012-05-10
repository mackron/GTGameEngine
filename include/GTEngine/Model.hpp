
#ifndef __GTEngine_Model_hpp_
#define __GTEngine_Model_hpp_

#include "Mesh.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /// Class representing a model.
    ///
    /// A model contains an arbitrary number of meshes, materials and animation data. 
    class Model
    {
    public:

        Model();
        ~Model();

        /// Attaches a mesh to the model.
        void AttachMesh(VertexArray* mesh, Material* material);

        /// Applies a transformation to the model's geometric data.
        void ApplyTransformation(const glm::mat4 &transform);

        /// Generates tangents and bitangents for every mesh.
        void GenerateTangentsAndBitangents();


    public:

        // The list of vertex array meshes making up the model.
        GTCore::Vector<Mesh*> meshes;
    };    
}

#endif
