
#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    VertexArray::VertexArray(VertexArrayUsage usage, const VertexFormat &format)
        : usage(usage), format(format),
          vertices(nullptr), vertexCount(0), indices(nullptr), indexCount(0),
          verticesMapped(false), indicesMapped(false),
          rendererData(nullptr), syncinfo()
    {
        Renderer::OnVertexArrayCreated(*this);
    }

    VertexArray::~VertexArray()
    {
        delete [] this->vertices;
        delete [] this->indices;

        Renderer::OnVertexArrayDeleted(*this);
    }

    void VertexArray::SetData(const float *vertices, size_t vertexCount, const unsigned int *indices, size_t indexCount)
    {
        this->SetVertexData(vertices, vertexCount);
        this->SetIndexData(indices, indexCount);
    }

    void VertexArray::SetVertexData(const float *vertices, size_t vertexCount)
    {
        assert(vertexCount > 0);
        assert(!this->verticesMapped);

        this->syncinfo.verticesChanged = true;
        this->vertexCount = vertexCount;

        delete [] this->vertices;

        size_t vertexDataSize = vertexCount * this->format.GetSize();
        this->vertices        = new float[vertexDataSize];

        if (vertices != nullptr)
        {
            for (size_t i = 0; i < vertexDataSize; ++i)
            {
                this->vertices[i] = vertices[i];
            }
        }


        Renderer::OnVertexArrayVertexDataChanged(*this);
    }

    void VertexArray::SetIndexData(const unsigned int *indices, size_t indexCount)
    {
        assert(indexCount > 0);
        assert(!this->indicesMapped);

        this->syncinfo.indicesChanged  = true;
        this->indexCount = indexCount;

        delete [] this->indices;
        this->indices = new unsigned int[indexCount];

        if (indices != nullptr)
        {
            for (size_t i = 0; i < indexCount; ++i)
            {
                this->indices[i] = indices[i];
            }
        }


        Renderer::OnVertexArrayIndexDataChanged(*this);
    }

    float * VertexArray::MapVertexData()
    {
        if (!this->verticesMapped)
        {
            this->verticesMapped = true;

            return this->vertices;
        }

        return nullptr;
    }
    
    void VertexArray::UnmapVertexData()
    {
        if (this->verticesMapped)
        {
            this->verticesMapped = false;
            this->syncinfo.verticesChanged = true;

            Renderer::OnVertexArrayVertexDataChanged(*this);
        }
    }

    unsigned int * VertexArray::MapIndexData()
    {
        if (!this->indicesMapped)
        {
            this->indicesMapped = true;

            return this->indices;
        }

        return nullptr;
    }
    
    void VertexArray::UnmapIndexData()
    {
        if (this->indicesMapped)
        {
            this->indicesMapped = false;
            this->syncinfo.indicesChanged = true;

            Renderer::OnVertexArrayIndexDataChanged(*this);
        }
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
            auto indexCount = this->GetIndexCount();
            auto indexData  = this->MapIndexData();
            auto vertexData = this->MapVertexData();

            // We need to loop over each triangle, which is every 3 indices.
            for (size_t iVertex = 0; iVertex < indexCount; iVertex += 3)
            {
                auto vertex0Src = vertexData + (indexData[iVertex + 0] * vertexSize);
                auto vertex1Src = vertexData + (indexData[iVertex + 1] * vertexSize);
                auto vertex2Src = vertexData + (indexData[iVertex + 2] * vertexSize);

                auto p0Src = vertex0Src + positionStride;
                auto p1Src = vertex1Src + positionStride;
                auto p2Src = vertex2Src + positionStride;

                auto n0Src = vertex0Src + normalStride;
                auto n1Src = vertex1Src + normalStride;
                auto n2Src = vertex2Src + normalStride;

                auto t0Src = vertex0Src + texCoordStride;
                auto t1Src = vertex1Src + texCoordStride;
                auto t2Src = vertex2Src + texCoordStride;

                glm::vec3 p0(p0Src[0], p0Src[1], p0Src[2]);
                glm::vec3 p1(p1Src[0], p1Src[1], p1Src[2]);
                glm::vec3 p2(p2Src[0], p2Src[1], p2Src[2]);

                glm::vec3 n0(n0Src[0], n0Src[1], n0Src[2]);
                glm::vec3 n1(n1Src[0], n1Src[1], n1Src[2]);
                glm::vec3 n2(n2Src[0], n2Src[1], n2Src[2]);

                glm::vec2 t0(t0Src[0], t0Src[1]);
                glm::vec2 t1(t1Src[0], t1Src[1]);
                glm::vec2 t2(t2Src[0], t2Src[1]);


                glm::vec3 p10 = p1 - p0;
                glm::vec3 p20 = p2 - p0;
                glm::vec2 t10 = t1 - t0;
                glm::vec2 t20 = t2 - t0;

                float r = 1.0F / (t10.x * t20.y - t20.x * t10.y);

                glm::vec3 tangent  = glm::normalize((p10 * t20.y) - (p20 * t10.y)) * r;
                glm::vec3 tangent0 = glm::orthonormalize(tangent, n0);
                glm::vec3 tangent1 = glm::orthonormalize(tangent, n1);
                glm::vec3 tangent2 = glm::orthonormalize(tangent, n2);

                glm::vec3 bitangent0 = glm::normalize(glm::cross(n0, tangent0));
                glm::vec3 bitangent1 = glm::normalize(glm::cross(n1, tangent1));
                glm::vec3 bitangent2 = glm::normalize(glm::cross(n2, tangent2));

                if (glm::dot(glm::cross(n0, tangent0), bitangent0) < 0)
                {
                    bitangent0 *= -1.0f;
                }
                if (glm::dot(glm::cross(n1, tangent1), bitangent1) < 0)
                {
                    bitangent1 *= -1.0f;
                }
                if (glm::dot(glm::cross(n2, tangent2), bitangent2) < 0)
                {
                    bitangent2 *= -1.0f;
                }
                    
                auto tangent0Src = vertex0Src + tangentStride;
                auto tangent1Src = vertex1Src + tangentStride;
                auto tangent2Src = vertex2Src + tangentStride;
                tangent0Src[0] = tangent0.x; tangent0Src[1] = tangent0.y; tangent0Src[2] = tangent0.z;
                tangent1Src[0] = tangent1.x; tangent1Src[1] = tangent1.y; tangent1Src[2] = tangent1.z;
                tangent2Src[0] = tangent2.x; tangent2Src[1] = tangent2.y; tangent2Src[2] = tangent2.z;

                auto bitangent0Src = vertex0Src + bitangentStride;
                auto bitangent1Src = vertex1Src + bitangentStride;
                auto bitangent2Src = vertex2Src + bitangentStride;
                bitangent0Src[0] = bitangent0.x; bitangent0Src[1] = bitangent0.y; bitangent0Src[2] = bitangent0.z;
                bitangent1Src[0] = bitangent1.x; bitangent1Src[1] = bitangent1.y; bitangent1Src[2] = bitangent1.z;
                bitangent2Src[0] = bitangent2.x; bitangent2Src[1] = bitangent2.y; bitangent2Src[2] = bitangent2.z;
            }
                

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