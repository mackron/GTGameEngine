
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
    }

    VertexArray::~VertexArray()
    {
        Renderer::MarkForCollection(this);

        delete [] this->vertices;
        delete [] this->indices;
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
        }
    }
}