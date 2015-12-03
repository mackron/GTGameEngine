// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/CPUVertexShader.hpp>

namespace GT
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
            
        }
    }

    void ProcessVertexShader(CPUVertexShader &shader, size_t firstVertexID, size_t lastVertexID)
    {
        // We need to iterate over each vertex and process it.
        for (size_t i = firstVertexID; i <= lastVertexID; ++i)
        {
            // The first step is to copy the vertex data to the output buffer.
            auto vertexOutput = shader.m_output + (i * shader.m_vertexSizeInFloats);
            auto vertexInput  = shader.m_input  + (i * shader.m_vertexSizeInFloats);

            // Now we can process the vertex.
            CPUVertexShader::Vertex vertex(i, vertexOutput, shader.m_format);
            if (shader.m_usingPosition)  vertex.Position  = GetVertexAttribute4(vertexInput, shader.m_positionComponentCount,  shader.m_positionOffset);
            if (shader.m_usingTexCoord)  vertex.TexCoord  = GetVertexAttribute4(vertexInput, shader.m_texCoordComponentCount,  shader.m_texCoordOffset);
            if (shader.m_usingNormal)    vertex.Normal    = GetVertexAttribute4(vertexInput, shader.m_normalComponentCount,    shader.m_normalOffset);
            if (shader.m_usingTangent)   vertex.Tangent   = GetVertexAttribute4(vertexInput, shader.m_tangentComponentCount,   shader.m_tangentOffset);
            if (shader.m_usingBitangent) vertex.Bitangent = GetVertexAttribute4(vertexInput, shader.m_bitangentComponentCount, shader.m_bitangentOffset);

            shader.ProcessVertex(vertex);

            if (shader.m_usingPosition)  SetVertexAttribute4(vertexOutput, shader.m_positionComponentCount,  shader.m_positionOffset,  vertex.Position);
            if (shader.m_usingTexCoord)  SetVertexAttribute4(vertexOutput, shader.m_texCoordComponentCount,  shader.m_texCoordOffset,  vertex.TexCoord);
            if (shader.m_usingNormal)    SetVertexAttribute4(vertexOutput, shader.m_normalComponentCount,    shader.m_normalOffset,    vertex.Normal);
            if (shader.m_usingTangent)   SetVertexAttribute4(vertexOutput, shader.m_tangentComponentCount,   shader.m_tangentOffset,   vertex.Tangent);
            if (shader.m_usingBitangent) SetVertexAttribute4(vertexOutput, shader.m_bitangentComponentCount, shader.m_bitangentOffset, vertex.Bitangent);
        }
    }



    CPUVertexShader::CPUVertexShader()
        : m_input(nullptr), m_vertexCount(0), m_format(), m_vertexSizeInFloats(m_format.GetSize()), m_output(nullptr),
          m_usingPosition(false), m_usingTexCoord(false), m_usingNormal(false), m_usingTangent(false), m_usingBitangent(false),
          m_positionComponentCount(0), m_positionOffset(0),
          m_texCoordComponentCount(0), m_texCoordOffset(0),
          m_normalComponentCount(0), m_normalOffset(0),
          m_tangentComponentCount(0), m_tangentOffset(0),
          m_bitangentComponentCount(0), m_bitangentOffset(0)
    {
    }

    CPUVertexShader::~CPUVertexShader()
    {
    }


    bool CPUVertexShader::Execute(const float* input, size_t vertexCount, const VertexFormat &format, float* output, size_t threadCount)
    {
        (void)threadCount;

        if (input != nullptr && output != nullptr)
        {
            this->OnStartExecute();
            {
                m_input              = input;
                m_vertexCount        = vertexCount;
                m_format             = format;
                m_vertexSizeInFloats = m_format.GetSize();
                m_output             = output;

                m_usingPosition  = m_format.GetAttributeInfo(VertexAttribs::Position,  m_positionComponentCount,  m_positionOffset);
                m_usingTexCoord  = m_format.GetAttributeInfo(VertexAttribs::TexCoord,  m_texCoordComponentCount,  m_texCoordOffset);
                m_usingNormal    = m_format.GetAttributeInfo(VertexAttribs::Normal,    m_normalComponentCount,    m_normalOffset);
                m_usingTangent   = m_format.GetAttributeInfo(VertexAttribs::Tangent,   m_tangentComponentCount,   m_tangentOffset);
                m_usingBitangent = m_format.GetAttributeInfo(VertexAttribs::Bitangent, m_bitangentComponentCount, m_bitangentOffset);

                ProcessVertexShader(*this, 0, m_vertexCount - 1);
            }
            this->OnEndExecute();

            return true;
        }
        
        return false;
    }


    void CPUVertexShader::OnStartExecute()
    {
    }

    void CPUVertexShader::OnEndExecute()
    {
    }




    ///////////////////////////////////////
    // CPUVertexShader::Vertex

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
