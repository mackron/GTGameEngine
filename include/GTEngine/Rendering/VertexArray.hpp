// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_VertexArray_hpp_
#define __GTEngine_VertexArray_hpp_

#include "VertexFormat.hpp"
#include "../Math.hpp"
#include "../Serialization.hpp"

namespace GTEngine
{
    enum VertexArrayUsage
    {
        VertexArrayUsage_Static,
        VertexArrayUsage_Dynamic,
        VertexArrayUsage_Stream
    };

    /// Class representing a vertex array.
    ///
    /// Currently, vertex and index data will be stored on both the client and server (GPU).
    class VertexArray
    {
    protected:

        /// Constructor.
        VertexArray(VertexArrayUsage usage, const VertexFormat &format);


    public:

        /// Destructor.
        virtual ~VertexArray();

        /// Sets the data of the vertex array.
        ///
        /// @param  vertices    [in] A pointer to the buffer containing the vertex data.
        /// @param  vertexCount [in] The number of vertices. This is NOT the number of floats in 'vertices', but the number of vertices in the array. Must be larger than 0.
        /// @param  indices     [in] A pointer to the buffer containing the indices.
        /// @param  indexCount  [in] The number of indices in the vertex array. Must be larger than 0.
        ///
        /// @remarks
        ///     This method asserts that vertex and index data pointers are not mapped.
        void SetData(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount);

        /// Sets the vertex data of the vertex array.
        ///
        /// @param  vertices    [in] A pointer to the buffer containing the vertex data.
        /// @param  vertexCount [in] The number of vertices. This is NOT the number of floats in 'vertices', but the number of vertices in the array. Must be larger than 0.
        ///
        /// @remarks
        ///     This method asserts that the existing vertex data is not mapped.
        ///     @par
        ///     Use SetIndexData() to set the indices.
        void SetVertexData(const float* vertices, size_t vertexCount);

        /// Sets the index data of the vertex array.
        ///
        /// @param  indices     [in] A pointer to the buffer containing the indices.
        /// @param  indexCount  [in] The number of indices in the vertex array. Must be larger than 0.
        ///
        /// @remarks
        ///     This method asserts that the existing index data is not mapped.
        ///     @par
        ///     Use SetVertexData() to set the vertices.
        void SetIndexData(const unsigned int* indices, size_t indexCount);


        /// Retrieves a constant pointer to the vertex data.
        ///
        /// remarks
        ///     Use this method for read-only access to the vertex data. If the data needs to be modified, use MapVertexData()/UnmapVertexData() or SetVertexData().
        inline const float* GetVertexDataPtr() const { return this->vertices; }

        /// Retrieves the number of vertices in the vertex array.
        inline size_t GetVertexCount() const { return this->vertexCount; }

        /// Retrieves a constant pointer to the index data.
        ///
        /// @remarks
        ///     Use this method for read-only access to the index data. If the data needs to be modified, use MapIndexData()/UnmapIndexData() or SetIndexData().
        inline const unsigned int* GetIndexDataPtr() const { return this->indices; }

        /// Retrieves the number of indices in the vertex array.
        inline size_t GetIndexCount() const { return this->indexCount; }


        /// Returns a writable pointer to the vertex data. Returns nullptr if the data is already mapped.
        ///
        /// @remarks
        ///     The data must be unmapped with UnmapVertexData() once the application is finished with the pointer.
        ///     @par
        ///     An application can first allocate the memory using SetVertexData() with a null pointer. Then it can use MapVertexData() if appropriate.
        float* MapVertexData();

        /// Unmaps the vertex data.
        void UnmapVertexData();

        /// Returns a writable pointer to the index data. Returns nullptr if the data is already mapped.
        ///
        /// @remarks
        ///     The data must be unmapped with UnmapIndexData() once the application is finished with the pointer.
        ///     @par
        ///     An application can first allocate the memory using SetIndexData() with a null pointer. Then it can use MapIndexData() if appropriate.
        unsigned int* MapIndexData();

        /// Unmaps the vertex data.
        void UnmapIndexData();



        /// Retrieves the vertex format of this array.
        inline const VertexFormat & GetFormat() const { return this->format; }

        /// Retrieves the usage of this array.
        inline VertexArrayUsage GetUsage() const { return this->usage; }



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the vertex array.
        ///
        /// @param serializer [in] A reference to the serializer to write to.
        void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the vertex array.
        ///
        /// @param deserializer [in] A reference to the deserializer to read from.
        void Deserialize(GTCore::Deserializer &deserializer);



    protected:

        ///////////////////////////////////////////////////////
        // Events.
        //
        // These events are called so that derived classes can know when data has changed. Typically, a function on the renderer will need to be called
        // that updates the renderer-side object.

        /// Called when vertex data has changed.
        virtual void OnVertexDataChanged() {};

        /// Called when the index data has changed.
        virtual void OnIndexDataChanged() {};



    // Transforms.
    //
    // TODO: Move these functions. VertexArray should be relatively low level. Maybe create a MeshUtils static class?
    public:

        /// Generates tangents and bitanges if the format is suitable.
        bool GenerateTangentsAndBitangents();

        /// Applies the given transformation to the vertex array.
        void ApplyTransform(const glm::mat4 &transform);

        /// Optimizes the vertex array to remove duplicate vertices.
        void Optimize();

        /// Swaps the triangle winding of the array. Assumes the array is made up of triangles. This only modifies the indices.
        void SwapTriangleWinding();

        /// Calculates the AABB of the vertex array.
        void CalculateAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax);


    protected:

        /// The usage of the vertex array.
        VertexArrayUsage usage;

        /// The format of the vertex data.
        VertexFormat format;


        /// A pointer to the the vertex information. The format of the data in this array is determined by 'format'.
        float* vertices;

        /// The number of vertices. This is NOT the number of float's in 'vertices', but rather the number of vertices that the data
        /// in 'vertices' defines.
        size_t vertexCount;

        /// The size in floats of the vertex buffer.
        size_t vertexBufferSize;



        /// A pointer to the indices that determine the vertices that make up each polygon in the array.
        unsigned int* indices;

        /// The number of indices making up the vertex array.
        size_t indexCount;

        /// The size in unsigned ints of the index buffer.
        size_t indexBufferSize;



        /// Keeps track of whether or not the vertex data is currently mapped.
        bool verticesMapped;

        /// Keeps track of whether or not the index data is currently mapped.
        bool indicesMapped;



    private:    // No copying.
        VertexArray(const VertexArray &);
        VertexArray & operator=(const VertexArray &);
    };
}

#endif