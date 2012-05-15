
#include <GTEngine/CPUVertexShader_Skinning.hpp>

namespace GTEngine
{
    CPUVertexShader_Skinning::CPUVertexShader_Skinning()
        : CPUVertexShader(),
          skinningVertexAttributes(nullptr)
    {
    }

    CPUVertexShader_Skinning::~CPUVertexShader_Skinning()
    {
    }

    void CPUVertexShader_Skinning::SetSkinningVertexAttributes(const SkinningVertexAttribute* attributes)
    {
        this->skinningVertexAttributes = attributes;
    }


    void CPUVertexShader_Skinning::ProcessVertex(Vertex &vertex)
    {
        assert(this->skinningVertexAttributes != nullptr);

        auto &skinningData = this->skinningVertexAttributes[vertex.GetID()];

        glm::vec4 position  = vertex.Get(VertexAttribs::Position);
        glm::vec4 normal    = vertex.Get(VertexAttribs::Normal);
        glm::vec4 tangent   = vertex.Get(VertexAttribs::Tangent);
        glm::vec4 bitangent = vertex.Get(VertexAttribs::Bitangent);

        glm::vec4 newPosition(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 newNormal(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 newTangent(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 newBitangent(0.0f, 0.0f, 0.0f, 0.0f);
        
        // For each bone...
        for (size_t i = 0; i < skinningData.bones.count; ++i)
        {
            auto bone   = skinningData.bones[i].bone;
            auto weight = skinningData.bones[i].weight;

            assert(bone != nullptr);

            glm::mat4 skinningTransform       = bone->GetSkinningTransform();
            glm::mat3 skinningNormalTransform = glm::mat3(skinningTransform);

            newPosition  += weight * (skinningTransform       * position);
            newNormal    += glm::vec4(weight * (skinningNormalTransform * glm::vec3(normal)),    0.0f);
            newTangent   += glm::vec4(weight * (skinningNormalTransform * glm::vec3(tangent)),   0.0f);
            newBitangent += glm::vec4(weight * (skinningNormalTransform * glm::vec3(bitangent)), 0.0f);
        }

        vertex.Set(VertexAttribs::Position,  position);
        vertex.Set(VertexAttribs::Normal,    normal);
        vertex.Set(VertexAttribs::Tangent,   tangent);
        vertex.Set(VertexAttribs::Bitangent, bitangent);
    }
}
