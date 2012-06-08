
#ifndef __PAG1_MeshBuilder_hpp_
#define __PAG1_MeshBuilder_hpp_

#include <GTCore/Vector.hpp>
#include "Rendering/DrawModes.hpp"
#include "Rendering/VertexFormat.hpp"
#include "Math.hpp"

namespace GTEngine
{
    /// Class used for building meshes.
    ///
    /// The way this class works is it emits primitives. In triangle mode, every 3 emits will make up the triangle.
    class MeshBuilder
    {
    public:

        /// Constructor.
        MeshBuilder(size_t vertexSizeInFloats);

        /// Destructor.
        virtual ~MeshBuilder();


        /// Emits a vertex.
        ///
        /// @param vertexData [in] A pointer to the vertex data. The size of this buffer is determined by the format specified in the constructor.
        ///
        /// @remarks
        ///     If an identical vertex has already been emitted, that one will be used instead of adding a duplicate vertex.
        void EmitVertex(const float* vertexData);


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
              float* GetVertexData() { return this->vertexBuffer.buffer; }
        const float* GetVertexData() const { return this->vertexBuffer.buffer; }

        /// Retrieves the number of vertices in the vertex buffer.
        size_t GetVertexCount() const { return this->vertexBuffer.count / vertexSizeInFloats; }


        /// Retrieves a pointer to the buffer containing the indices.
              unsigned int* GetIndexData() { return this->indexBuffer.buffer; }
        const unsigned int* GetIndexData() const { return this->indexBuffer.buffer; }

        /// Retrieves the number of indices in the vertex buffer.
        size_t GetIndexCount() const { return this->indexBuffer.count; }


    protected:

        /// The number of floats making up each vertex.
        size_t vertexSizeInFloats;

        /// The buffer containing the vertex data.
        GTCore::Vector<float> vertexBuffer;

        /// The buffer containing the indices.
        GTCore::Vector<unsigned int> indexBuffer;
    };
}


namespace GTEngine
{
    /// Mesh builder class optimized for the common P3T2N3 texture format.
    class MeshBuilderP3T2N3 : public MeshBuilder
    {
    public:

        /// Default constructor.
        MeshBuilderP3T2N3();

        /// Destructor.
        ~MeshBuilderP3T2N3();


        /// Emits a vertex with a position, texture coordinate and normal.
        void EmitVertex(const glm::vec3 &position, const glm::vec2 &texCoord, const glm::vec3 &normal);
    };
}

#endif