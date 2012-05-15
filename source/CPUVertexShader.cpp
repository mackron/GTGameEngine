
#include <GTEngine/CPUVertexShader.hpp>

// Helpers
namespace GTEngine
{
    inline glm::vec4 GetVertexAttribute4(const float* data, size_t componentCount, size_t offset)
    {
        if (data != nullptr)
        {
            data += offset;

            // We'll try and be clever here and predict the most common scenarios and put them first.
            if (componentCount == 3)
            {
                return glm::vec4(data[0], data[1], data[2], 0.0f);
            }
            else if (componentCount == 2)
            {
                return glm::vec4(data[0], data[1], 0.0f, 0.0f);
            }
            else if (componentCount == 4)
            {
                return glm::vec4(data[0], data[1], data[2], data[3]);
            }
            else if (componentCount == 1)
            {
                return glm::vec4(data[0], 0.0f, 0.0f, 0.0f);
            }
            else
            {
                return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            return glm::vec4();
        }
    }

    inline void SetVertexAttribute4(float* data, size_t componentCount, size_t offset, const glm::vec4 &value)
    {
        if (data != nullptr)
        {
            data += offset;

            // As above, we'll try and be clever here and predict the most common scenarios and put them first.
            if (componentCount == 3)
            {
                data[0] = value.x;
                data[1] = value.y;
                data[2] = value.z;
            }
            else if (componentCount == 2)
            {
                data[0] = value.x;
                data[1] = value.y;
            }
            else if (componentCount == 4)
            {
                data[0] = value.x;
                data[1] = value.y;
                data[2] = value.z;
                data[3] = value.w;
            }
            else if (componentCount == 1)
            {
                data[0] = value.x;
            }

            /*
            if (componentCount == 3)
            {
                data[0] = value.Data.m128_f32[0];
                data[1] = value.Data.m128_f32[1];
                data[2] = value.Data.m128_f32[2];
            }
            else if (componentCount == 2)
            {
                data[0] = value.Data.m128_f32[0];
                data[1] = value.Data.m128_f32[1];
            }
            else if (componentCount == 4)
            {
                data[0] = value.Data.m128_f32[0];
                data[1] = value.Data.m128_f32[1];
                data[2] = value.Data.m128_f32[2];
                data[3] = value.Data.m128_f32[3];
            }
            else if (componentCount == 1)
            {
                data[0] = value.Data.m128_f32[0];
            }
            */
        }
    }
}

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

            size_t positionComponentCount;
            size_t positionOffset;
            bool loadPosition = this->format.GetAttributeInfo(VertexAttribs::Position, positionComponentCount, positionOffset);

            size_t texCoordComponentCount;
            size_t texCoordOffset;
            bool loadTexCoord = this->format.GetAttributeInfo(VertexAttribs::TexCoord, texCoordComponentCount, texCoordOffset);

            size_t normalComponentCount;
            size_t normalOffset;
            bool loadNormal = this->format.GetAttributeInfo(VertexAttribs::Normal, normalComponentCount, normalOffset);

            size_t tangentComponentCount;
            size_t tangentOffset;
            bool loadTangent = this->format.GetAttributeInfo(VertexAttribs::Tangent, tangentComponentCount, tangentOffset);

            size_t bitangentComponentCount;
            size_t bitangentOffset;
            bool loadBitangent = this->format.GetAttributeInfo(VertexAttribs::Bitangent, bitangentComponentCount, bitangentOffset);



            // We need to iterate over each vertex and process it.
            for (size_t i = 0; i < this->vertexCount; ++i)
            {
                // The first step is to copy the vertex data to the output buffer.
                auto vertexOutput = this->output + (i * this->vertexSizeInFloats);
                auto vertexInput  = this->input  + (i * this->vertexSizeInFloats);
                //memcpy(vertexOutput, vertexInput, this->vertexSizeInFloats * sizeof(float));

                // Now we can process the vertex.
                Vertex vertex(i, vertexOutput, this->format);
                if (loadPosition)  vertex.Position  = GetVertexAttribute4(vertexInput, positionComponentCount,  positionOffset);
                if (loadTexCoord)  vertex.TexCoord  = GetVertexAttribute4(vertexInput, texCoordComponentCount,  texCoordOffset);
                if (loadNormal)    vertex.Normal    = GetVertexAttribute4(vertexInput, normalComponentCount,    normalOffset);
                if (loadTangent)   vertex.Tangent   = GetVertexAttribute4(vertexInput, tangentComponentCount,   tangentOffset);
                if (loadBitangent) vertex.Bitangent = GetVertexAttribute4(vertexInput, bitangentComponentCount, bitangentOffset);

                this->ProcessVertex(vertex);

                if (loadPosition)  SetVertexAttribute4(vertexOutput, positionComponentCount,  positionOffset,  vertex.Position);
                if (loadTexCoord)  SetVertexAttribute4(vertexOutput, texCoordComponentCount,  texCoordOffset,  vertex.TexCoord);
                if (loadNormal)    SetVertexAttribute4(vertexOutput, normalComponentCount,    normalOffset,    vertex.Normal);
                if (loadTangent)   SetVertexAttribute4(vertexOutput, tangentComponentCount,   tangentOffset,   vertex.Tangent);
                if (loadBitangent) SetVertexAttribute4(vertexOutput, bitangentComponentCount, bitangentOffset, vertex.Bitangent);
            }

            return true;
        }
        
        return false;
    }
}


// CPUVertexShader::Vertex
namespace GTEngine
{
    CPUVertexShader::Vertex::Vertex(unsigned int id, float* data, const VertexFormat &format)
        : id(id), data(data), format(format),
          Position(0.0f, 0.0f, 0.0f, 1.0f),
          TexCoord(),
          Normal(),
          Tangent(),
          Bitangent()
    {
    }

    CPUVertexShader::Vertex::~Vertex()
    {
    }

    unsigned int CPUVertexShader::Vertex::GetID() const
    {
        return this->id;
    }

    glm::vec4 CPUVertexShader::Vertex::Get(int attribute)
    {
        glm::vec4 value(0.0f, 0.0f, 0.0f, 1.0f);

        size_t componentCount;
        size_t stride;
        if (this->format.GetAttributeInfo(attribute, componentCount, stride))
        {
            value = GetVertexAttribute4(this->data, componentCount, stride);
        }

        return glm::vec4(value.x, value.y, value.z, value.w);
        //return glm::vec4(value.Data.m128_f32[0], value.Data.m128_f32[1], value.Data.m128_f32[2], value.Data.m128_f32[3]);
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
            SetVertexAttribute4(this->data, componentCount, stride, glm::vec4(value));
        }
    }
}
