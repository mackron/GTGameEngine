// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/GUI/Rendering/GUIMesh.hpp>

namespace GT
{
    GUIMesh::GUIMesh()
        : vertices(), indices(), texture(0)
    {
    }

    GUIMesh::GUIMesh(const GUIMeshVertex* verticesIn, size_t vertexCount, const unsigned int* indicesIn, size_t indexCount, GUIImageHandle textureIn)
        : vertices(vertexCount), indices(indexCount), texture(textureIn)
    {
        for (size_t i = 0; i < vertexCount; ++i)
        {
            this->vertices.PushBack(verticesIn[i]);
        }

        for (size_t i = 0; i < indexCount; ++i)
        {
            this->indices.PushBack(indicesIn[i]);
        }
    }

    GUIMesh::~GUIMesh()
    {
    }


    void GUIMesh::SetVertexColours(float r, float g, float b)
    {
        for (size_t i = 0; i < this->vertices.count; ++i)
        {
            auto &vertex = this->vertices[i];
            vertex.colourR = r;
            vertex.colourG = g;
            vertex.colourB = b;
        }
    }

    void GUIMesh::SetVertexColours(float r, float g, float b, float a)
    {
        for (size_t i = 0; i < this->vertices.count; ++i)
        {
            auto &vertex = this->vertices[i];
            vertex.colourR = r;
            vertex.colourG = g;
            vertex.colourB = b;
            vertex.colourA = a;
        }
    }

    void GUIMesh::SetVertexAlpha(float a)
    {
        for (size_t i = 0; i < this->vertices.count; ++i)
        {
            auto &vertex = this->vertices[i];
            vertex.colourA = a;
        }
    }


    void GUIMesh::SetFromP2T2Colour(const float* verticesIn, size_t vertexCount, const unsigned int* indicesIn, size_t indexCount, const GT::ColorF &colour)
    {
        const size_t vertexComponentCount = 4;

        this->vertices.Clear();
        this->indices.Clear();

        this->vertices.Reserve(vertexCount);
        for (size_t i = 0; i < vertexCount; ++i)
        {
            auto vertexIn = verticesIn + (i * vertexComponentCount);

            GUIMeshVertex vertex;
            vertex.positionX = vertexIn[0];
            vertex.positionY = vertexIn[1];
            vertex.texCoordX = vertexIn[2];
            vertex.texCoordY = vertexIn[3];

            vertex.colourR = colour.r;
            vertex.colourG = colour.g;
            vertex.colourB = colour.b;
            vertex.colourA = colour.a;

            this->vertices.PushBack(vertex);
        }

        
        this->indices.Reserve(indexCount);
        for (size_t i = 0; i < indexCount; ++i)
        {
            this->indices.PushBack(indicesIn[i]);
        }
    }
}
