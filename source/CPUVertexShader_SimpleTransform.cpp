// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>

namespace GTEngine
{
    CPUVertexShader_SimpleTransform::CPUVertexShader_SimpleTransform(const glm::mat4 &transform)
        : CPUVertexShader(), transform(), normalTransform()
    {
        this->SetTransform(transform);
    }

    CPUVertexShader_SimpleTransform::~CPUVertexShader_SimpleTransform()
    {
    }

    void CPUVertexShader_SimpleTransform::SetTransform(const glm::mat4 &transform)
    {
        this->transform       = glm::simdMat4(transform);
        this->normalTransform = glm::inverse(glm::transpose(glm::mat3(transform)));
    }

    void CPUVertexShader_SimpleTransform::ProcessVertex(Vertex &vertex)
    {
        // We only care about the position and normal here.
        vertex.Position.w = 1.0f;

        vertex.Position = this->transform * vertex.Position;
        vertex.Normal   = glm::simdVec4(this->normalTransform * glm::vec3(vertex.Normal.x, vertex.Normal.y, vertex.Normal.z), 0.0f);
    }
}
