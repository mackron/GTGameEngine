
#ifndef __GTEngine_Armature_hpp_
#define __GTEngine_Armature_hpp_

#include "Bone.hpp"
#include <GTCore/Vector.hpp>

// Just for temporary testing.
#include "Logging.hpp"

namespace GTEngine
{
    /// Class representing the armature of a mesh.
    ///
    /// An armature is simply made up of a series of bones. Each bone can be compared to a scene node - they have a parent and children,
    /// which each bone being positioned relative to it's parent.
    ///
    /// Not all bones affect the meshes vertices, but instead are used solely for the transformations of children.
    ///
    /// There can be any number of root level bones.
    ///
    /// This class does not allocate or deallocate bones. This needs to be done at a higher level.
    class Armature
    {
    public:

        /// Constructor.
        Armature();

        /// Destructor.
        ~Armature();


        /// Adds a root level bone.
        void AddRootBone(BoneWithWeights &rootBone);

        /// Removes a root level bone.
        void RemoveRootBone(BoneWithWeights &rootBone);

        /// Retrieves a direct reference to the internal vector containing the root bones.
              GTCore::Vector<BoneWithWeights*> & GetRootBones()       { return this->rootBones; }
        const GTCore::Vector<BoneWithWeights*> & GetRootBones() const { return this->rootBones; }



        // Testing for debugging.
        void LogInfo()
        {
            for (size_t i = 0; i < this->rootBones.count; ++i)
            {
                this->rootBones[i]->LogInfo();
            }
        }


    private:

        /// The list of root level bones.
        GTCore::Vector<BoneWithWeights*> rootBones;
    };
}


#endif
