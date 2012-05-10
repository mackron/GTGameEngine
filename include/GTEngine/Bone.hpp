
#ifndef __GTEngine_Bone_hpp_
#define __GTEngine_Bone_hpp_

#include <GTCore/Vector.hpp>
#include "Math.hpp"

namespace GTEngine
{
    /// Class representing a bone in a skeleton.
    class Bone
    {
    public:

        /// Constructor.
        Bone();

        /// Destructor.
        ~Bone();


    private:

        /// The parent bone. If this is null then the bone is the root bone in the skeleton.
        Bone* parent;

        /// The children.
        GTCore::Vector<Bone*> children;

        /// The bones transformation.
        glm::mat4 transform;

        /// The offset matrix for this bone.
        glm::mat4 offsetMatrix;


        /// Structure reprsenting a vertex weight.
        struct VertexWeight
        {
            unsigned int vertexID;
            float        weight;
        };

        /// The list of vertex weights.
        GTCore::Vector<VertexWeight> weights;
    };
}


#endif
