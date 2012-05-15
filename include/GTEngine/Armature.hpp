
#ifndef __GTEngine_Armature_hpp_
#define __GTEngine_Armature_hpp_

#include "Bone.hpp"
#include <GTCore/Vector.hpp>

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
        void AddRootBone(Bone &rootBone);

        /// Removes a root level bone.
        void RemoveRootBone(Bone &rootBone);

        /// Retrieves a direct reference to the internal vector containing the root bones.
              GTCore::Vector<Bone*> & GetRootBones()       { return this->rootBones; }
        const GTCore::Vector<Bone*> & GetRootBones() const { return this->rootBones; }



    private:

        /// The list of root level bones. Whether or not these are simply Bone or BoneWithWeights objects depends on the context in
        /// which the armature is used.
        GTCore::Vector<Bone*> rootBones;
    };
}


#endif
