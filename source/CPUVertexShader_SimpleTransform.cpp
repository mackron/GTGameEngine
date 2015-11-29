// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>

namespace GT
{
    CPUVertexShader_SimpleTransform::CPUVertexShader_SimpleTransform(const glm::mat4 &transform)
        : CPUVertexShader(), m_transform(), m_normalTransform()
    {
        this->SetTransform(transform);
    }

    CPUVertexShader_SimpleTransform::~CPUVertexShader_SimpleTransform()
    {
    }

    void CPUVertexShader_SimpleTransform::SetTransform(const glm::mat4 &transform)
    {
        m_transform       = glm::mat4(transform);
        m_normalTransform = glm::inverse(glm::transpose(glm::mat3(transform)));
    }

    void CPUVertexShader_SimpleTransform::ProcessVertex(Vertex &vertex)
    {
        // We only care about the position and normal here.
        vertex.Position.w = 1.0f;

        vertex.Position = m_transform * vertex.Position;
        vertex.Normal   = glm::vec4(m_normalTransform * glm::vec3(vertex.Normal.x, vertex.Normal.y, vertex.Normal.z), 0.0f);
    }
}
