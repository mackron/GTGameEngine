// Copyright (C) 2011 - 2015 David Reid. See included LICENCE.

#ifndef __GT_MeshBuilder_hpp_
#define __GT_MeshBuilder_hpp_

#include <GTLib/Vector.hpp>

namespace GT
{
    /// Class used for building meshes.
    ///
    /// The way this class works is it emits primitives. In triangle mode, every 3 emits will make up the triangle.
    class MeshBuilder
    {
    public:

        /// Constructor.
        MeshBuilder(unsigned int vertexSizeInFloats, bool checkDuplicatesOnEmit = true);

        /// Destructor.
        virtual ~MeshBuilder();


        /// Emits a vertex.
        ///
        /// @param vertexData [in] A pointer to the vertex data. The size of this buffer is determined by the format specified in the constructor.
        ///
        /// @remarks
        ///     If an identical vertex has already been emitted, that one will be used instead of adding a duplicate vertex.
        void EmitVertex(const float* vertexData);

        /// Merges another mesh builder into this one.
        ///
        /// @param other [in] The other mesh builder to add to this one.
        ///
        /// @remarks
        ///     Both mesh builders MUST be in the same format.
        void Merge(const MeshBuilder &other);


        /// Finds the index of the given vertex.
        ///
        /// @param vertexData [in ] A pointer to the vertex data to search for.
        /// @param indexOut   [out] A reference to the variable that will receive the index, if found.
        ///
        /// @return
        ///     True if the vertex was found; false otherwise.
        bool Find(const float* vertexData, unsigned int &indexOut);


        /// Compares two vertex data buffers for equality.
        ///
        /// @param vertexData0 [in] The first buffer to compare against.
        /// @param vertexData1 [in] The second buffer to compare against.
        ///
        /// @return True if the vertices are equal; false otherwise.
        bool Equal(const float* vertexData0, const float* vertexData1);


        /// Retrieves a pointer to the buffer containing the vertex data.
              float* GetVertexData()       { return m_vertexBuffer.buffer; }
        const float* GetVertexData() const { return m_vertexBuffer.buffer; }

        /// Retrieves the number of vertices in the vertex buffer.
        unsigned int GetVertexCount() const { return static_cast<unsigned int>(m_vertexBuffer.count) / m_vertexSizeInFloats; }


        /// Retrieves a pointer to the buffer containing the indices.
              unsigned int* GetIndexData()       { return m_indexBuffer.buffer; }
        const unsigned int* GetIndexData() const { return m_indexBuffer.buffer; }

        /// Retrieves the number of indices in the vertex buffer.
        unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_indexBuffer.count); }


        /// Clears the mesh builder.
        void Clear();


        /// Retrieves the size of each vertex in floats.
        ///
        /// @remarks
        ///     The return value will always be the same as the value passed to the constructor.
        unsigned int GetVertexSizeInFloats() const { return m_vertexSizeInFloats; }


    protected:

        /// The number of floats making up each vertex.
        unsigned int m_vertexSizeInFloats;

        /// The buffer containing the vertex data.
        GTLib::Vector<float> m_vertexBuffer;

        /// The buffer containing the indices.
        GTLib::Vector<unsigned int> m_indexBuffer;

        /// Whether or not the mesh builder should check for duplicate vertices. Defaults to true.
        bool m_checkDuplicatesOnEmit;
    };



    /// Mesh builder class optimized for the common P3T2N3 texture format.
    class MeshBuilderP3T2N3 : public MeshBuilder
    {
    public:

        /// Default constructor.
        MeshBuilderP3T2N3(bool checkDuplicatesOnEmit = true);

        /// Destructor.
        ~MeshBuilderP3T2N3();


        /// Emits a vertex with a position, texture coordinate and normal.
        void EmitVertex(float positionX, float positionY, float positionZ, float texCoordX, float texCoordY, float normalX, float normalY, float normalZ);
    };
}

#endif
