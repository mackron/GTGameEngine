// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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


        /// Clears the mesh builder.
        void Clear();


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


    /// Mesh builder optimized for a simple P3 format. Useful for wireframes.
    class MeshBuilderP3 : public MeshBuilder
    {
    public:

        /// Default constructor.
        MeshBuilderP3();

        /// Destructor.
        ~MeshBuilderP3();


        /// Emits a vertex with a position.
        void EmitVertex(const glm::vec3 &position);
    };


    /// A special mesh builder for building wireframe spheres.
    ///
    /// A wireframe sphere is made up of 4 rings. 3 of the rings are in a fixed orientation, but the fourth is orientated based on a
    /// camera orientation. Each ring will have it's own mesh builder object assigned to it, which means you are free to only use the
    /// rings you want.
    ///
    /// In order for the sphere to read properly to the viewer, it is best to hide the back facing part of the applicable rings. This
    /// is enabled by default if a camera transform is specified, but otherwise it can be disabled.
    class WireframeSphereMeshBuilder
    {
    public:

        /// Constructor.
        WireframeSphereMeshBuilder(unsigned int ringSegments = 32);

        /// Destructor.
        ~WireframeSphereMeshBuilder();


        /// Builds the sphere.
        ///
        /// @remarks
        ///     There sphere will be a unit sphere and positioned at the origin. The back-side line segments will not be included.
        void Build(const glm::mat4 &cameraView);



    private:

        /// Generic method for building a ring with the given transform.
        void BuildRing(const glm::mat4 &cameraView, const glm::mat4 &ringTransform, bool cullBackFacingSegments, MeshBuilderP3 &ringOut);


    private:

        /// The number of segments to use with each ring.
        unsigned int ringSegmentsCount;

        /// The ring on the X/Y plane.
        MeshBuilderP3 xyRing;

        /// The ring on the X/Z plane.
        MeshBuilderP3 xzRing;

        /// The ring on the Y/Z plane.
        MeshBuilderP3 yzRing;

        /// The ring orientated towards the camera.
        MeshBuilderP3 cameraRing;
    };
}

#endif