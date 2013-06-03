// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/CPUVertexShader_Skinning.hpp>

namespace GTEngine
{
    /*
    glm::simdVec4 mul(const glm::simdMat4 &m, const glm::simdVec4 &v)
    {
        __m128 v0 = _mm_mul_ps(m[0].Data, _mm_shuffle_ps(v.Data, v.Data, _MM_SHUFFLE(0,0,0,0)));
        __m128 v1 = _mm_mul_ps(m[1].Data, _mm_shuffle_ps(v.Data, v.Data, _MM_SHUFFLE(1,1,1,1)));
        __m128 v2 = _mm_mul_ps(m[2].Data, _mm_shuffle_ps(v.Data, v.Data, _MM_SHUFFLE(2,2,2,2)));
        __m128 v3 = _mm_mul_ps(m[3].Data, _mm_shuffle_ps(v.Data, v.Data, _MM_SHUFFLE(3,3,3,3)));

        return glm::simdVec4(_mm_add_ps(_mm_add_ps(v0, v1), _mm_add_ps(v2, v3)));
    }
    */


    CPUVertexShader_Skinning::CPUVertexShader_Skinning()
        : CPUVertexShader(),
          bones(nullptr), skinningVertexAttributes(nullptr),
          aabbMin(FLT_MAX), aabbMax(-FLT_MAX)
    {
    }

    CPUVertexShader_Skinning::~CPUVertexShader_Skinning()
    {
    }


    void CPUVertexShader_Skinning::SetBoneBuffer(const Bone* const* bonesIn)
    {
        this->bones = bonesIn;
    }

    void CPUVertexShader_Skinning::SetSkinningVertexAttributes(const SkinningVertexAttribute* attributes)
    {
        this->skinningVertexAttributes = attributes;
    }

    void CPUVertexShader_Skinning::GetAABB(glm::vec3 &aabbMinOut, glm::vec3 &aabbMaxOut)
    {
        aabbMinOut.x = this->aabbMin.x;
        aabbMinOut.y = this->aabbMin.y;
        aabbMinOut.z = this->aabbMin.z;

        aabbMaxOut.x = this->aabbMax.x;
        aabbMaxOut.y = this->aabbMax.y;
        aabbMaxOut.z = this->aabbMax.z;
    }



    void CPUVertexShader_Skinning::OnStartExecute()
    {
        this->aabbMin = glm::simdVec4( FLT_MAX);
        this->aabbMax = glm::simdVec4(-FLT_MAX);
    }


    void CPUVertexShader_Skinning::ProcessVertex(Vertex &vertex)
    {
        assert(this->skinningVertexAttributes != nullptr);

        auto &skinningData = this->skinningVertexAttributes[vertex.GetID()];


        vertex.Position.w  = 1.0f;
        vertex.Normal.w    = 0.0f;
        vertex.Tangent.w   = 0.0f;
        vertex.Bitangent.w = 0.0f;

        /*
        glm::vec4 newPosition(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 newNormal(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 newTangent(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 newBitangent(0.0f, 0.0f, 0.0f, 0.0f);
        */

        glm::simdVec4 newPosition(0.0f, 0.0f, 0.0f, 0.0f);
        glm::simdVec4 newNormal(0.0f, 0.0f, 0.0f, 0.0f);
        glm::simdVec4 newTangent(0.0f, 0.0f, 0.0f, 0.0f);
        glm::simdVec4 newBitangent(0.0f, 0.0f, 0.0f, 0.0f);
        

        // For each bone...
        for (size_t i = 0; i < skinningData.bones.count; ++i)
        {
            auto bone   = this->bones[skinningData.bones[i].boneIndex];
            auto weight = skinningData.bones[i].weight;

            assert(bone != nullptr);

            
            //const glm::mat4 &skinningTransform = bone->GetSkinningTransform();
            glm::simdMat4 skinningTransform(bone->GetSkinningTransform());
            
            newPosition  += weight * (skinningTransform * vertex.Position);
            newNormal    += weight * (skinningTransform * vertex.Normal);
            newTangent   += weight * (skinningTransform * vertex.Tangent);
            newBitangent += weight * (skinningTransform * vertex.Bitangent);
        }

        vertex.Position  = newPosition;
        vertex.Normal    = newNormal;
        vertex.Tangent   = newTangent;
        vertex.Bitangent = newBitangent;

        this->aabbMin = glm::min(vertex.Position, this->aabbMin);
        this->aabbMax = glm::max(vertex.Position, this->aabbMax);
    }
}
