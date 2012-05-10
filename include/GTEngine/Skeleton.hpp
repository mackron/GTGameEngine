
#ifndef __GTEngine_Skeleton_hpp_
#define __GTEngine_Skeleton_hpp_

#include "Bone.hpp"

namespace GTEngine
{
    /// Class representing the skeleton of a mesh.
    class Skeleton
    {
    public:

        /// Constructor.
        Skeleton();

        /// Destructor.
        ~Skeleton();


        /// Sets the root bone.


    private:

        /// The root bone.
        Bone* root;
    };
}

#endif
