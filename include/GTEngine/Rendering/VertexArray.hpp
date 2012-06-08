
#ifndef __GTEngine_VertexArray_hpp_
#define __GTEngine_VertexArray_hpp_

#include "VertexFormat.hpp"
#include "../Math.hpp"

namespace GTEngine
{
    enum VertexArrayUsage
    {
        VertexArrayUsage_Static,
        VertexArrayUsage_Dynamic,
        VertexArrayUsage_Stream
    };

    /**
    *   \brief  Class representing a vertex array.
    *
    *   Vertex array data is stored in two locations: on client memory (CPU/RAM) and server memory (GPU). We do this so we can
    *   easily access geometric information from outside the rendering thread in a much easier manner. If we don't do this, getting
    *   access to the vertex data anywhere other than rendering thread is impossible since we would need to call a renderer function.
    *
    *   When data is changed, it is changed on the system side and a flag will be set specifying that the data needs to be
    *   changed on the GPU side. The data will be synced the next time the vertex array is used.
    *
    *   If/when an application will never need access to the client vertex data, it can use VertexArray::DeleteLocalData(). This
    *   will delete the data on the client side. It will, however, render Get*DataPtr() useless and will cause Map*Data() and 
    *   Unmap*Data() to become server-side GPU calls, which can not be called from anywhere other than the rendering thread.
    */
    class VertexArray
    {
    public:

        /**
        *   \brief  Constructor.
        */
        VertexArray(VertexArrayUsage usage, const VertexFormat &format);

        /**
        *   \brief  Destructor.
        */
        ~VertexArray();

        /**
        *   \brief                   Sets the data of the vertex array.
        *   \param  vertices    [in] A pointer to the buffer containing the vertex data.
        *   \param  vertexCount [in] The number of vertices. This is NOT the number of floats in 'vertices', but the number of vertices in the array. Must be larger than 0.
        *   \param  indices     [in] A pointer to the buffer containing the indices.
        *   \param  indexCount  [in] The number of indices in the vertex array. Must be larger than 0.
        *
        *   \remarks
        *       This method asserts that vertex and index data pointers are not mapped.
        */
        void SetData(const float *vertices, size_t vertexCount, const unsigned int *indices, size_t indexCount);
        void SetVertexData(const float *vertices, size_t vertexCount);
        void SetIndexData(const unsigned int *indices, size_t indexCount);


        /**
        *   \brief  Retrieves a constant pointer to the vertex data.
        *
        *   \remarks
        *       Use this method for read-only access to the vertex data. If the data needs to be modified, use MapVertexData()/UnmapVertexData() or SetVertexData().
        */
        inline const float * GetVertexDataPtr() const { return this->vertices; }

        /**
        *   \brief  Retrieves the number of vertices in the vertex array.
        */
        inline size_t GetVertexCount() const { return this->vertexCount; }

        /**
        *   \brief  Retrieves a constant pointer to the index data.
        *
        *   \remarks
        *       Use this method for read-only access to the index data. If the data needs to be modified, use MapIndexData()/UnmapIndexData() or SetIndexData().
        */
        inline const unsigned int * GetIndexDataPtr() const { return this->indices; }

        /**
        *   \brief  Retrieves the number of indices in the vertex array.
        */
        inline size_t GetIndexCount() const { return this->indexCount; }


        /**
        *   \brief  Returns a writable pointer to the vertex data. Returns nullptr if the data is already mapped.
        *
        *   \remarks
        *       The data must be unmapped with UnmapVertexData() once the application is finished with the pointer.
        *       \par
        *       An application can first allocate the memory using SetVertexData() with a null pointer. Then it can use MapVertexData() if appropriate.
        */
        float * MapVertexData();

        /**
        *   \brief  Unmaps the vertex data.
        */
        void UnmapVertexData();

        /**
        *   \brief  Returns a writable pointer to the index data. Returns nullptr if the data is already mapped.
        *
        *   \remarks
        *       The data must be unmapped with UnmapIndexData() once the application is finished with the pointer.
        *       \par
        *       An application can first allocate the memory using SetIndexData() with a null pointer. Then it can use MapIndexData() if appropriate.
        */
        unsigned int * MapIndexData();

        /**
        *   \brief  Unmaps the vertex data.
        */
        void UnmapIndexData();


        



        /**
        *   \brief  Retrieves the vertex format of this array.
        */
        inline const VertexFormat & GetFormat() const { return this->format; }

        /**
        *   \brief  Retrieves the usage of this array.
        */
        inline VertexArrayUsage GetUsage() const { return this->usage; }

        /**
        *   \brief  Retrieves a pointer to the internal renderer data.
        *
        *   \remarks
        *       This should only be used by the renderer.
        */
        inline const void * GetRendererData() const { return this->rendererData; }
        inline       void * GetRendererData()       { return this->rendererData; }

        /**
        *   \brief  Sets the pointer to the internal renderer data.
        *
        *   \remarks
        *       This does not deallocate the previous renderer data. That is the responsibility of the renderer itself.
        */
        inline void SetRendererData(void *rendererData) const { this->rendererData = rendererData; }



    // Transforms.
    //
    // TODO: Move this function. VertexArray should be relatively low level. Maybe create a MeshUtils static class?
    public:

        /// Generates tangents and bitanges if the format is suitable.
        bool GenerateTangentsAndBitangents();

        /// Applies the given transformation to the vertex array.
        void ApplyTransform(const glm::mat4 &transform);

        /// Optimizes the vertex array to remove duplicate vertices.
        void Optimize();

        /// Swaps the triangle winding of the array. Assumes the array is made up of triangles. This only modifies the indices.
        void SwapTriangleWinding();


    private:

        /// The usage of the vertex array.
        VertexArrayUsage usage;

        /// The format of the vertex data.
        VertexFormat format;

        /// A pointer to the the vertex information. The format of the data in this array is determined by 'format'.
        float *vertices;

        /// The number of vertices. This is NOT the number of float's in 'vertices', but rather the number of vertices that the data
        /// in 'vertices' defines.
        size_t vertexCount;

        /// A pointer to the indices that determine the vertices that make up each polygon in the array.
        unsigned int *indices;

        /// The number of indices making up the vertex array.
        size_t indexCount;

        /// Keeps track of whether or not the vertex data is currently mapped.
        bool verticesMapped;

        /// Keeps track of whether or not the index data is currently mapped.
        bool indicesMapped;

        /// The renderer will need to store it's own properties about the vertex array. This pointer can be used by the renderer
        /// to hold a pointer to some renderer-specific data.
        mutable void *rendererData;


    public:

        struct _syncinfo
        {
            _syncinfo()
                : verticesChanged(true), indicesChanged(true)
            {
            }
            ~_syncinfo()
            {
            }

            bool verticesChanged;       //< Whether or not the vertex data has changed.
            bool indicesChanged;        //< Whether or not the texture data has been updated.

        private:    // No copying.
            _syncinfo(const _syncinfo &);
            _syncinfo & operator=(const _syncinfo &);

        }mutable syncinfo;


    private:    // No copying.
        VertexArray(const VertexArray &);
        VertexArray & operator=(const VertexArray &);


    friend class Renderer;
    };
}

#endif