// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/TextMesh.hpp>

namespace GT
{
    TextMesh::TextMesh()
        : m_vertices(), m_indices(), m_glyphMapHandle(0)
    {
    }

    TextMesh::TextMesh(const TextMeshVertex* verticesIn, size_t vertexCount, const unsigned int* indicesIn, size_t indexCount, GlyphMapHandle glyphMapHandle)
        : m_vertices(vertexCount), m_indices(indexCount), m_glyphMapHandle(glyphMapHandle)
    {
        for (size_t i = 0; i < vertexCount; ++i)
        {
            m_vertices.PushBack(verticesIn[i]);
        }

        for (size_t i = 0; i < indexCount; ++i)
        {
            m_indices.PushBack(indicesIn[i]);
        }
    }

    TextMesh::~TextMesh()
    {
    }


    void TextMesh::SetVertexColours(float r, float g, float b)
    {
        for (size_t i = 0; i < m_vertices.count; ++i)
        {
            auto &vertex = m_vertices[i];
            vertex.colourR = r;
            vertex.colourG = g;
            vertex.colourB = b;
        }
    }
}
