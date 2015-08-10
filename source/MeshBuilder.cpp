// Copyright (C) 2011 - 2015 David Reid. See included LICENCE.

#include <GTGameEngine/MeshBuilder.hpp>

namespace GT
{
    MeshBuilder::MeshBuilder(unsigned int vertexSizeInFloats, bool checkDuplicatesOnEmit)
        : m_vertexSizeInFloats(vertexSizeInFloats), m_vertexBuffer(), m_indexBuffer(), m_checkDuplicatesOnEmit(checkDuplicatesOnEmit)
    {
    }

    MeshBuilder::~MeshBuilder()
    {
    }

    void MeshBuilder::EmitVertex(const float* vertexData)
    {
        unsigned int index;
        if (!m_checkDuplicatesOnEmit || !this->Find(vertexData, index))
        {
            index = static_cast<unsigned int>(m_vertexBuffer.count) / static_cast<unsigned int>(m_vertexSizeInFloats);

            // Here is where we append the input vertex data to the mesh builder's vertex data buffer.
            for (auto i = 0U; i < m_vertexSizeInFloats; ++i)
            {
                m_vertexBuffer.PushBack(vertexData[i]);
            }
        }

        // Now we just add the index to the end.
        m_indexBuffer.PushBack(index);
    }

    void MeshBuilder::Merge(const MeshBuilder &other)
    {
        for (size_t iIndex = 0; iIndex < other.GetIndexCount(); ++iIndex)
        {
            unsigned int index = other.GetIndexData()[iIndex];
            auto vertex        = other.GetVertexData() + (other.GetVertexSizeInFloats() * index);

            this->EmitVertex(vertex);
        }
    }

    bool MeshBuilder::Find(const float* vertexData, unsigned int &indexOut)
    {
        size_t vertexCount = m_vertexBuffer.count / m_vertexSizeInFloats;

        for (size_t i = 0; i < vertexCount; ++i)
        {
            if (this->Equal(m_vertexBuffer.buffer + (i * m_vertexSizeInFloats), vertexData))
            {
                indexOut = static_cast<unsigned int>(i);
                return true;
            }
        }

        return false;
    }

    bool MeshBuilder::Equal(const float* vertexData0, const float* vertexData1)
    {
        for (size_t i = 0; i < m_vertexSizeInFloats; ++i)
        {
            if (vertexData0[i] != vertexData1[i])
            {
                return false;
            }
        }

        // If we make it here, it means we are equal.
        return true;
    }

    void MeshBuilder::Clear()
    {
        m_vertexBuffer.Clear();
        m_indexBuffer.Clear();
    }




    /// MeshBuilderP3T2N3
    MeshBuilderP3T2N3::MeshBuilderP3T2N3(bool checkDuplicatesOnEmit)
        : MeshBuilder(8, checkDuplicatesOnEmit)
    {
    }

    MeshBuilderP3T2N3::~MeshBuilderP3T2N3()
    {
    }

    void MeshBuilderP3T2N3::EmitVertex(float positionX, float positionY, float positionZ, float texCoordX, float texCoordY, float normalX, float normalY, float normalZ)
    {
        float data[8] =
        {
            positionX, positionY, positionZ,
            texCoordX, texCoordY,
            normalX,   normalY,   normalZ
        };

        MeshBuilder::EmitVertex(data);
    }
}
