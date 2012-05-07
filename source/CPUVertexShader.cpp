
#include <GTEngine/CPUVertexShader.hpp>

namespace GTEngine
{
    CPUVertexShader::CPUVertexShader()
        : input(nullptr), vertexCount(0), format(), vertexSizeInFloats(format.GetSize()), output(nullptr),
          currentVertexIndex(0)
    {
    }

    CPUVertexShader::CPUVertexShader(const float* input, size_t vertexCount, const VertexFormat &format, float* output)
        : input(input), vertexCount(vertexCount), format(format), vertexSizeInFloats(format.GetSize()), output(output),
          currentVertexIndex(0)
    {
    }

    CPUVertexShader::~CPUVertexShader()
    {
    }

    bool CPUVertexShader::Initialise(const float* input, size_t vertexCount, const VertexFormat &format, float* output)
    {
        this->input              = input;
        this->vertexCount        = vertexCount;
        this->format             = format;
        this->vertexSizeInFloats = this->format.GetSize();
        this->output             = output;
        this->currentVertexIndex = 0;

        return true;
    }

    bool CPUVertexShader::Execute()
    {
        if (this->input != nullptr && this->output != nullptr)
        {
            // We need to iterate over each vertex and process it.
            for (this->currentVertexIndex = 0; this->currentVertexIndex < this->vertexCount; ++this->currentVertexIndex)
            {
                // The first step is to copy the vertex data to the output buffer.
                auto vertexOutput = this->output + (this->currentVertexIndex * this->vertexSizeInFloats);
                auto vertexInput  = this->input  + (this->currentVertexIndex * this->vertexSizeInFloats);
                memcpy(vertexOutput, vertexInput, this->vertexSizeInFloats * sizeof(float));

                // Now we can process the vertex.
                this->ProcessVertex();
            }

            return true;
        }
        
        return false;
    }


    glm::vec4 CPUVertexShader::Get(int attribute)
    {
        size_t componentCount = this->format.GetAttributeComponentCount(attribute);
        size_t stride         = this->format.GetAttributeOffset(attribute);

        auto data = this->input + ((this->currentVertexIndex * this->vertexSizeInFloats) + stride);

        // We get the value from the input vertex.
        glm::vec4 value(0.0f, 0.0f, 0.0f, 1.0f);

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

        return value;
    }

    void CPUVertexShader::Set(int attribute, float value)
    {
        this->Set(attribute, glm::vec4(value, 0.0f, 0.0f, 1.0f));
    }

    void CPUVertexShader::Set(int attribute, const glm::vec2 &value)
    {
        this->Set(attribute, glm::vec4(value, 0.0f, 1.0f));
    }

    void CPUVertexShader::Set(int attribute, const glm::vec3 &value)
    {
        this->Set(attribute, glm::vec4(value, 1.0f));
    }

    void CPUVertexShader::Set(int attribute, const glm::vec4 &value)
    {
        size_t componentCount = this->format.GetAttributeComponentCount(attribute);
        size_t stride         = this->format.GetAttributeOffset(attribute);

        auto data = this->output + ((this->currentVertexIndex * this->vertexSizeInFloats) + stride);

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
