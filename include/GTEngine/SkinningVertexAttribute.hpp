// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SkinningVertexAttribute_hpp_
#define __GTEngine_SkinningVertexAttribute_hpp_

#include <GTEngine/Bone.hpp>
#include <GTEngine/Core/Vector.hpp>

namespace GT
{
    /// Structure representing a bone/weight pair that should be applied to a vertex.
    struct BoneWeightPair
    {
        BoneWeightPair(int boneIndexIn, float weightIn)
            : boneIndex(static_cast<uint32_t>(boneIndexIn)), weight(weightIn)
        {
        }

        BoneWeightPair(const BoneWeightPair &other)
            : boneIndex(other.boneIndex), weight(other.weight)
        {
        }

        BoneWeightPair & operator=(const BoneWeightPair &other)
        {
            this->boneIndex = other.boneIndex;
            this->weight    = other.weight;

            return *this;
        }


        uint32_t boneIndex;
        float    weight;
    };

    /// Structure representing a vertex attribute used for the CPU skinning vertex shader.
    ///
    /// This simply contains an array of bone/weight pairs that should be applied to the vertex.
    struct SkinningVertexAttribute
    {
        SkinningVertexAttribute()
            : bones()
        {
        }

        void AddBoneWeightPair(int boneIndex, float weight)
        {
            this->bones.PushBack(BoneWeightPair(boneIndex, weight));
            this->bones.OptimizeBuffer();                               // <-- we do this to ensure we keep the buffer tightly bound to the data. Avoids wastage since there will be a lot of instantiations here (one for each vertex).
        }

        Vector<BoneWeightPair> bones;
    };
}

#endif
