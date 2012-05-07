
#include <GTEngine/CPUVertexShader.hpp>

namespace GTEngine
{
    CPUVertexShader::CPUVertexShader()
        : input(nullptr), vertexCount(0), format(), vertexSizeInFloats(format.GetSize()), output(nullptr)
    {
    }

    CPUVertexShader::~CPUVertexShader()
    {
    }

    bool CPUVertexShader::Execute(const float* input, size_t vertexCount, const VertexFormat &format, float* output)
    {
        if (input != nullptr && output != nullptr)
        {
            this->input              = input;
            this->vertexCount        = vertexCount;
            this->format             = format;
            this->vertexSizeInFloats = this->format.GetSize();
            this->output             = output;

            // We need to iterate over each vertex and process it.
            for (size_t i = 0; i < this->vertexCount; ++i)
            {
                // The first step is to copy the vertex data to the output buffer.
                auto vertexOutput = this->output + (i * this->vertexSizeInFloats);
                auto vertexInput  = this->input  + (i * this->vertexSizeInFloats);
                memcpy(vertexOutput, vertexInput, this->vertexSizeInFloats * sizeof(float));

                // Now we can process the vertex.
                Vertex vertex(vertexOutput, this->format);
                this->ProcessVertex(vertex);
            }

            return true;
        }
        
        return false;
    }
}


// CPUVertexShader::Vertex
namespace GTEngine
{
    CPUVertexShader::Vertex::Vertex(float* data, const VertexFormat &format)
        : data(data), format(format)
    {
    }

    CPUVertexShader::Vertex::~Vertex()
    {
    }

    glm::vec4 CPUVertexShader::Vertex::Get(int attribute)
    {
        glm::vec4 value(0.0f, 0.0f, 0.0f, 1.0f);

        size_t componentCount;
        size_t stride;
        if (this->format.GetAttributeInfo(attribute, componentCount, stride))
        {
            auto data = this->data + stride;

            if (componentCount > 0)
            {
                value.x = *(data + 0);

                if (componentCount > 1)
                {
                    value.y = *(data + 1);

                    if (componentCount > 2)
                    {
                        value.z = *(data + 2);

                        if (componentCount > 3)
                        {
                            value.w = *(data + 3);
                        }
                    }
                }
            }
        }

        return value;
    }

    void CPUVertexShader::Vertex::Set(int attribute, float value)
    {
        this->Set(attribute, glm::vec4(value, 0.0f, 0.0f, 1.0f));
    }

    void CPUVertexShader::Vertex::Set(int attribute, const glm::vec2 &value)
    {
        this->Set(attribute, glm::vec4(value, 0.0f, 1.0f));
    }

    void CPUVertexShader::Vertex::Set(int attribute, const glm::vec3 &value)
    {
        this->Set(attribute, glm::vec4(value, 1.0f));
    }

    void CPUVertexShader::Vertex::Set(int attribute, const glm::vec4 &value)
    {
        size_t componentCount;
        size_t stride;
        if (this->format.GetAttributeInfo(attribute, componentCount, stride))
        {
            auto data = this->data + stride;

            if (componentCount > 0)
            {
                *(data + 0) = value.x;

                if (componentCount > 1)
                {
                    *(data + 1) = value.y;

                    if (componentCount > 2)
                    {
                        *(data + 2) = value.z;

                        if (componentCount > 3)
                        {
                            *(data + 3) = value.w;
                        }
                    }
                }
            }
        }
    }
}
