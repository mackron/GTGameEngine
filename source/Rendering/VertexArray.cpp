// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    VertexArray::VertexArray(VertexArrayUsage usage, const VertexFormat &format)
        : usage(usage), format(format),
          m_vertices(nullptr), m_vertexCount(0), m_vertexBufferSize(0), m_indices(nullptr), m_indexCount(0), m_indexBufferSize(0),
          verticesMapped(false), indicesMapped(false)
    {
    }

    VertexArray::~VertexArray()
    {
        free(m_vertices);
        free(m_indices);
    }

    void VertexArray::SetData(const float* verticesIn, unsigned int vertexCountIn, const unsigned int* indicesIn, unsigned int indexCountIn)
    {
        this->SetVertexData(verticesIn, vertexCountIn);
        this->SetIndexData(indicesIn, indexCountIn);
    }

    void VertexArray::SetVertexData(const float* verticesIn, unsigned int vertexCountIn)
    {
        assert(!this->verticesMapped);


        size_t vertexDataSize = vertexCountIn * this->format.GetSize();

        if (m_vertexBufferSize < vertexDataSize || m_vertexBufferSize > vertexDataSize * 2)
        {
            free(m_vertices);

            if (vertexCountIn > 0)
            {
                m_vertices = static_cast<float*>(malloc(vertexDataSize * sizeof(float)));
            }
            else
            {
                m_vertices = nullptr;
            }


            m_vertexBufferSize = vertexDataSize;
        }


        if (m_vertices != nullptr && verticesIn != nullptr)
        {
            for (size_t i = 0; i < vertexDataSize; ++i)
            {
                m_vertices[i] = verticesIn[i];
            }
        }

        
        m_vertexCount = vertexCountIn;
        this->OnVertexDataChanged();
    }

    void VertexArray::SetIndexData(const unsigned int* indicesIn, unsigned int indexCountIn)
    {
        assert(!this->indicesMapped);

        if (m_indexBufferSize < indexCountIn || m_indexBufferSize > indexCountIn * 2)
        {
            free(m_indices);

            if (indexCountIn > 0)
            {
                m_indices = static_cast<unsigned int*>(malloc(indexCountIn * sizeof(unsigned int)));
            }
            else
            {
                m_indices = 0;
            }

            
            m_indexBufferSize = indexCountIn;
        }

        if (m_indices != nullptr && indicesIn != nullptr)
        {
            for (unsigned int i = 0; i < indexCountIn; ++i)
            {
                m_indices[i] = indicesIn[i];
            }
        }


        m_indexCount = indexCountIn;
        this->OnIndexDataChanged();
    }

    float* VertexArray::MapVertexData()
    {
        if (!this->verticesMapped)
        {
            this->verticesMapped = true;

            return m_vertices;
        }

        return nullptr;
    }

    void VertexArray::UnmapVertexData()
    {
        if (this->verticesMapped)
        {
            this->verticesMapped = false;
            this->OnVertexDataChanged();
        }
    }

    unsigned int* VertexArray::MapIndexData()
    {
        if (!this->indicesMapped)
        {
            this->indicesMapped = true;

            return m_indices;
        }

        return nullptr;
    }

    void VertexArray::UnmapIndexData()
    {
        if (this->indicesMapped)
        {
            this->indicesMapped = false;
            this->OnIndexDataChanged();
        }
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void VertexArray::Serialize(GTLib::Serializer &serializer) const
    {
        // The info chunk is always first.
        GTLib::BasicSerializer infoSerializer;
        infoSerializer.Write(static_cast<uint32_t>(this->usage));
        this->format.Serialize(infoSerializer);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_VertexArray_Info;
        header.version     = 1;
        header.sizeInBytes = infoSerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(infoSerializer.GetBuffer(), header.sizeInBytes);



        // Next is the vertex data.
        header.id          = Serialization::ChunkID_VertexArray_Vertices;
        header.version     = 1;
        header.sizeInBytes =
            sizeof(uint32_t) +                                                  // <-- Vertex count.
            sizeof(float) * this->format.GetSize() * m_vertexCount;         // <-- Vertex data.

        serializer.Write(header);
        {
            serializer.Write(static_cast<uint32_t>(m_vertexCount));
            serializer.Write(m_vertices, sizeof(float) * this->format.GetSize() * m_vertexCount);
        }



        // Last of all is the index data.
        header.id          = Serialization::ChunkID_VertexArray_Indices;
        header.version     = 1;
        header.sizeInBytes =
            sizeof(uint32_t) +                               // <-- Index count.
            sizeof(unsigned int) * m_indexCount;         // <-- Index data.

        serializer.Write(header);
        {
            serializer.Write(static_cast<uint32_t>(m_indexCount));
            serializer.Write(m_indices, sizeof(unsigned int) * m_indexCount);
        }
    }

    void VertexArray::Deserialize(GTLib::Deserializer &deserializer)
    {
        assert(this->verticesMapped == false);
        assert(this->indicesMapped  == false);


        // The first chunk should be the info chunk.
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_VertexArray_Info);
        {
            switch (header.version)
            {
            case 1:
                {
                    uint32_t usageIn;
                    deserializer.Read(usageIn);

                    this->usage = static_cast<VertexArrayUsage>(usageIn);
                    this->format.Deserialize(deserializer);

                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing VertexArray. Info chunk is an unsupported version (%d).", header.version);
                    break;
                }
            }
        }


        // The next chunk should be the vertex data.
        deserializer.Read(header);
        assert(header.id == Serialization::ChunkID_VertexArray_Vertices);
        {
            switch (header.version)
            {
            case 1:
                {
                    uint32_t newVertexCount;
                    deserializer.Read(newVertexCount);
                    
                    m_vertexCount = static_cast<size_t>(newVertexCount);

                    free(m_vertices);
                    m_vertices = static_cast<float*>(malloc(this->format.GetSize() * m_vertexCount * sizeof(float)));
                    deserializer.Read(m_vertices, sizeof(float) * this->format.GetSize() * m_vertexCount);
                }

            default:
                {
                    GTEngine::Log("Error deserializing VertexArray. Vertices chunk is an unsupported version (%d).", header.version);
                    break;
                }
            }
        }


        // The last chunk should be the index data.
        deserializer.Read(header);
        assert(header.id == Serialization::ChunkID_VertexArray_Indices);
        {
            switch (header.version)
            {
            case 1:
                {
                    uint32_t newIndexCount;
                    deserializer.Read(newIndexCount);

                    m_indexCount = static_cast<size_t>(newIndexCount);

                    free(m_indices);
                    m_indices = static_cast<unsigned int*>(malloc(m_indexCount * sizeof(unsigned int)));
                    deserializer.Read(m_indices, sizeof(unsigned int) * m_indexCount);
                }

            default:
                {
                    GTEngine::Log("Error deserializing VertexArray. Indices chunk is an unsupported version (%d).", header.version);
                    break;
                }
            }
        }


        this->OnVertexDataChanged();
        this->OnIndexDataChanged();
    }
}


#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>
#include <GTEngine/MeshBuilder.hpp>

namespace GTEngine
{
    bool VertexArray::GenerateTangentsAndBitangents()
    {
        auto positionStride  = this->format.GetAttributeOffset(VertexAttribs::Position);
        auto normalStride    = this->format.GetAttributeOffset(VertexAttribs::Normal);
        auto texCoordStride  = this->format.GetAttributeOffset(VertexAttribs::TexCoord);
        auto tangentStride   = this->format.GetAttributeOffset(VertexAttribs::Tangent);
        auto bitangentStride = this->format.GetAttributeOffset(VertexAttribs::Bitangent);

        if (positionStride != -1 && normalStride != -1 && texCoordStride != -1 && tangentStride != -1 && bitangentStride != -1)
        {
            auto vertexSize = format.GetSize();
            auto indexCount  = this->GetIndexCount();
            auto indexData   = this->MapIndexData();
            auto vertexCount = this->GetVertexCount();
            auto vertexData  = this->MapVertexData();

            glm::vec3* tan = new glm::vec3[vertexCount];

            // We need to loop over each triangle, which is every 3 indices.
            for (size_t iVertex = 0; iVertex < indexCount; iVertex += 3)
            {
                unsigned int i0 = indexData[iVertex + 0];
                unsigned int i1 = indexData[iVertex + 1];
                unsigned int i2 = indexData[iVertex + 2];

                auto vertex0Src = vertexData + (i0 * vertexSize);
                auto vertex1Src = vertexData + (i1 * vertexSize);
                auto vertex2Src = vertexData + (i2 * vertexSize);

                auto p0Src = vertex0Src + positionStride;
                auto p1Src = vertex1Src + positionStride;
                auto p2Src = vertex2Src + positionStride;
                auto t0Src = vertex0Src + texCoordStride;
                auto t1Src = vertex1Src + texCoordStride;
                auto t2Src = vertex2Src + texCoordStride;

                glm::vec3 v1(p0Src[0], p0Src[1], p0Src[2]);
                glm::vec3 v2(p1Src[0], p1Src[1], p1Src[2]);
                glm::vec3 v3(p2Src[0], p2Src[1], p2Src[2]);
                glm::vec2 w1(t0Src[0], t0Src[1]);
                glm::vec2 w2(t1Src[0], t1Src[1]);
                glm::vec2 w3(t2Src[0], t2Src[1]);

                float x1 = v2.x - v1.x;
                float x2 = v3.x - v1.x;
                float y1 = v2.y - v1.y;
                float y2 = v3.y - v1.y;
                float z1 = v2.z - v1.z;
                float z2 = v3.z - v1.z;
        
                float s1 = w2.x - w1.x;
                float s2 = w3.x - w1.x;
                float t1 = w2.y - w1.y;
                float t2 = w3.y - w1.y;


                float r = s1 * t2 - s2 * t1;
                if (r != 0)
                {
                    r = 1.0f / r;
                }

                glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
                glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
        

                tan[i0] += sdir;
                tan[i1] += sdir;
                tan[i2] += sdir;
            }

            for (size_t iVertex = 0; iVertex < vertexCount; ++iVertex)
            {
                auto vertexPtr = vertexData + (iVertex * vertexSize);

                auto normalPtr    = vertexPtr + normalStride;
                auto tangentPtr   = vertexPtr + tangentStride;
                auto bitangentPtr = vertexPtr + bitangentStride;


                glm::vec3 n(normalPtr[0], normalPtr[1], normalPtr[2]);
                glm::vec3 t(tan[iVertex]);
                

                glm::vec3 tangent;
                glm::vec3 bitangent;

                if (glm::length2(t) > 0)
                {
                    // Tangent.
                    tangent = glm::orthonormalize(t, n);


                    // Bitangent.
                    bitangent = glm::normalize(glm::cross(n, tangent));
                    if (glm::dot(glm::cross(n, tangent), bitangent) < 0)
                    {
                        bitangent *= -1.0f;
                    }
                }

                
                tangentPtr[0] = tangent.x;
                tangentPtr[1] = tangent.y; 
                tangentPtr[2] = tangent.z;

                bitangentPtr[0] = bitangent.x;
                bitangentPtr[1] = bitangent.y;
                bitangentPtr[2] = bitangent.z;
            }

            delete [] tan;


            this->UnmapIndexData();
            this->UnmapVertexData();

            return true;
        }
        else
        {
            // We don't have all of the required attributes in the the vertex format.
            return false;
        }
    }


    void VertexArray::ApplyTransform(const glm::mat4 &transform)
    {
        // We're going to use a CPU vertex shader here.
        CPUVertexShader_SimpleTransform shader(transform);

        auto vertexData = this->MapVertexData();
        {
            shader.Execute(vertexData, this->GetVertexCount(), this->GetFormat(), vertexData);
        }
        this->UnmapVertexData();
    }


    void VertexArray::Optimize()
    {
        size_t vertexSize = this->format.GetSize();
        MeshBuilder builder(vertexSize);

        auto vertexData = this->MapVertexData();
        auto indexData  = this->MapIndexData();
        {
            for (size_t i = 0; i < this->GetIndexCount(); ++i)
            {
                auto index  = indexData[i];
                auto vertex = vertexData + (index * vertexSize);

                builder.EmitVertex(vertex);
            }
        }
        this->UnmapVertexData();
        this->UnmapIndexData();


        // Now we can set the data again.
        this->SetData(builder.GetVertexData(), builder.GetVertexCount(), builder.GetIndexData(), builder.GetIndexCount());
    }

    void VertexArray::SwapTriangleWinding()
    {
        auto indexData = this->MapIndexData();
        {
            for (size_t i = 0; i < this->GetIndexCount(); i += 3)
            {
                auto temp = indexData[i];
                indexData[i]     = indexData[i + 2];
                indexData[i + 2] = temp;
            }
        }
        this->UnmapIndexData();
    }

    void VertexArray::CalculateAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax)
    {
        aabbMin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
        aabbMax = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);

        auto positionOffset = this->format.GetAttributeOffset(VertexAttribs::Position);
        assert(positionOffset != -1);

        auto vertexData  = this->GetVertexDataPtr();
        auto vertexCount = this->GetVertexCount();
        auto vertexSize  = this->format.GetSize();

        for (size_t i = 0; i < vertexCount; ++i)
        {
            auto position = vertexData + positionOffset;

            if (position[0] < aabbMin.x) aabbMin.x = position[0];
            if (position[0] > aabbMax.x) aabbMax.x = position[0];
            if (position[1] < aabbMin.y) aabbMin.y = position[1];
            if (position[1] > aabbMax.y) aabbMax.y = position[1];
            if (position[2] < aabbMin.z) aabbMin.z = position[2];
            if (position[2] > aabbMax.z) aabbMax.z = position[2];

            vertexData += vertexSize;
        }
    }
}
