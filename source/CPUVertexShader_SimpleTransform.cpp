
#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>

namespace GTEngine
{
    CPUVertexShader_SimpleTransform::CPUVertexShader_SimpleTransform(const glm::mat4 &transform)
        : CPUVertexShader(), transform()
    {
        this->SetTransform(transform);
    }

    CPUVertexShader_SimpleTransform::~CPUVertexShader_SimpleTransform()
    {
    }

    void CPUVertexShader_SimpleTransform::SetTransform(const glm::mat4 &transform)
    {
        this->transform       = transform;
        this->normalTransform = glm::inverse(glm::transpose(glm::mat3(transform)));
    }

    void CPUVertexShader_SimpleTransform::ProcessVertex(Vertex &vertex)
    {
        // We only care about the position and normal here.
        glm::vec4 positionIn = vertex.Get(VertexAttribs::Position);
        glm::vec4 normalIn   = vertex.Get(VertexAttribs::Normal);

        vertex.Set(VertexAttribs::Position, this->transform       * positionIn);
        vertex.Set(VertexAttribs::Normal,   this->normalTransform * glm::vec3(normalIn));
    }
}
