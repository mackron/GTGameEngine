// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_MeshBuilder
#define GT_MeshBuilder

#include <GTEngine/Core/Vector.hpp>
#include "Rendering/DrawModes.hpp"
#include "Rendering/VertexFormat.hpp"
#include "Math.hpp"

class btCollisionShape;
class btConvexHullShape;
class btBoxShape;

namespace GT
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


        /// Retrieves the size of each vertex in floats.
        ///
        /// @remarks
        ///     The return value will always be the same as the value passed to the constructor.
        size_t GetVertexSizeInFloats() const { return this->vertexSizeInFloats; }


    protected:

        /// The number of floats making up each vertex.
        size_t vertexSizeInFloats;

        /// The buffer containing the vertex data.
        Vector<float> vertexBuffer;

        /// The buffer containing the indices.
        Vector<unsigned int> indexBuffer;

        /// Whether or not the mesh builder should check for duplicate vertices. Defaults to true.
        bool checkDuplicatesOnEmit;
    };
}


namespace GT
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
        void EmitVertex(const float x, const float y, const float z) { this->EmitVertex(glm::vec3(x, y, z)); }
    };


    /// A special mesh builder for constructing wireframe boxes.
    ///
    /// The mesh is in P3 format.
    class WireframeBoxMeshBuilder : public MeshBuilderP3
    {
    public:

        /// Constructor.
        WireframeBoxMeshBuilder();

        /// Destructor.
        ~WireframeBoxMeshBuilder();


        /// Builds the box.
        ///
        /// @param halfExtents [in] The half extents of the box.
        void Build(const glm::vec3 &halfExtents, const glm::mat4 &transform = glm::mat4());
        void Build(float halfX, float halfY, float halfZ, const glm::mat4 &transform = glm::mat4()) { this->Build(glm::vec3(halfX, halfY, halfZ), transform); }
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
        WireframeSphereMeshBuilder(unsigned int ringSegments = 64);

        /// Destructor.
        ~WireframeSphereMeshBuilder();


        /// Builds the sphere.
        ///
        /// @remarks
        ///     There sphere will be a unit sphere and positioned at the origin. The back-side line segments will not be included.
        void Build(const glm::mat4 &transform = glm::mat4());


        /// Retrieves the mesh builder of the X/Y ring.
        const MeshBuilderP3 & GetXYRing() const { return this->rings[0]; }

        /// Retrieves the mesh builder of the X/Z ring.
        const MeshBuilderP3 & GetXZRing() const { return this->rings[1]; }

        /// Retrieves the mesh builder of the X/Y ring.
        const MeshBuilderP3 & GetYZRing() const { return this->rings[2]; }


        /// Retrieves the number of rings making up the sphere.
        size_t GetRingCount() const { return this->rings.count; }

        /// Retrieves a reference to the mesh builder of the ring at the given index.
              MeshBuilderP3 & GetRing(size_t index)       { return this->rings[index]; }
        const MeshBuilderP3 & GetRing(size_t index) const { return this->rings[index]; }


    private:

        /// Generic method for building a ring with the given transform.
        void BuildRing(const glm::mat4 &transform, const glm::mat4 &ringTransform, MeshBuilderP3 &ringOut);


    private:

        /// The number of segments to use with each ring.
        unsigned int ringSegmentsCount;

        /// The list of mesh builders for each ring.
        Vector<MeshBuilderP3> rings;
    };


    /// A special mesh builder for constructing wireframe cylinders.
    ///
    /// The mesh is in P3 format.
    class WireframeCylinderMeshBuilder : public MeshBuilderP3
    {
    public:

        /// Constructor.
        WireframeCylinderMeshBuilder(unsigned int ringSegmentsCount = 64);

        /// Destructor.
        ~WireframeCylinderMeshBuilder();


        /// Builds the box.
        ///
        /// @param halfExtents [in] The half extents of the box.
        ///
        /// @remarks
        ///     The cylinder will be orientated such that it is running along the Z axis.
        void Build(float radius, float height, const glm::mat4 &transform = glm::mat4());


    private:

        /// Adds a ring segment to the mesh.
        void CreateRing(float radius, const glm::mat4 &transform);


    private:

        /// The number of segments to use with each ring.
        unsigned int ringSegmentsCount;
    };


    /// A special mesh builder for constructing wireframe capsules.
    ///
    /// The mesh is in P3 format.
    class WireframeCapsuleMeshBuilder : public MeshBuilderP3
    {
    public:

        /// Constructor.
        WireframeCapsuleMeshBuilder(unsigned int circleSegmentsCount = 64);

        /// Destructor.
        ~WireframeCapsuleMeshBuilder();


        /// Builds the box.
        ///
        /// @param halfExtents [in] The half extents of the box.
        ///
        /// @remarks
        ///     The cylinder will be orientated such that it is running along the Z axis.
        void Build(float radius, float height, const glm::mat4 &transform = glm::mat4());


    private:

        /// Adds a ring segment to the mesh.
        void CreateRing(float radius, float length, const glm::mat4 &transform);


    private:

        /// The number of segments to use with each ring.
        unsigned int circleSegmentsCount;
    };


    /// A special mesh builder for constructing a wireframe mesh from a collision shape.
    ///
    /// The mesh is in P3 format.
    ///
    /// When constructing the mesh, some wireframe shapes will need the camera view transform in order for the
    /// builder to build the geometry in a way the mesh can read properly in the viewport. Therefore the
    class WireframeCollisionShapeMeshBuilder : public MeshBuilderP3
    {
    public:

        /// Constructor.
        WireframeCollisionShapeMeshBuilder(unsigned int circleSegmentsCount = 64);

        /// Destructor.
        ~WireframeCollisionShapeMeshBuilder();


        /// Builds the mesh.
        ///
        /// @param shape      [in] The collision shape to build the goemtry from.
        /// @param cameraView [in] The camera's view matrix.
        ///
        /// @remarks
        ///     The mesh will be positioned at the origin.
        void Build(const btCollisionShape &shape, const glm::mat4 &cameraView);

        /// Builds and merges a mesh representing the given shape.
        ///
        /// @param shape      [in] The collision shape to build the goemtry from.
        /// @param cameraView [in] The camera's view matrix.
        /// @param transform  [in] The local transform to apply to the collision shape.
        ///
        /// @remarks
        ///     The mesh will be positioned at the origin.
        void BuildAndMerge(const btCollisionShape &shape, const glm::mat4 &cameraView, const glm::mat4 &transform = glm::mat4());





    private:

        /// The number of segments to use with circular geometry.
        unsigned int circleSegmentsCount;
    };





    //////////////////////////////////////////////////////////////////////////////////////
    // Solid triangle-meshes for collision shapes. All of these are in simple P3 format.

    /// Class for constructing a simple P3 mesh from a box collision shape.
    class BoxCollisionShapeMeshBuilder : public MeshBuilderP3
    {
    public:

        /// Constructor.
        BoxCollisionShapeMeshBuilder();

        /// Destructor.
        ~BoxCollisionShapeMeshBuilder();


        /// Builds the mesh.
        ///
        /// @param shape     [in] A reference to the box shape to construct.
        /// @param transform [in] The transform to apply to the box.
        void Build(const btBoxShape &shape, const glm::mat4 &transform);
    };

    /// Class for constructing a simple P3 convex hull mesh.
    class ConvexHullCollisionShapeMeshBuilder : public MeshBuilderP3
    {
    public:

        /// Constructor
        ConvexHullCollisionShapeMeshBuilder();

        /// Destructor.
        ~ConvexHullCollisionShapeMeshBuilder();


        /// Builds the mesh.
        ///
        /// @param shape     [in] A reference to the convex hull to construct.
        /// @param transform [in] The transform to apply to the convex hull.
        void Build(const btConvexHullShape &shape, const glm::mat4 &transform);
    };

    /// Class for constructing a simple P3 mesh representation of the given collision shape.
    class CollisionShapeMeshBuilder : public MeshBuilderP3
    {
    public:

        /// Constructor.
        CollisionShapeMeshBuilder();

        /// Destructor.
        ~CollisionShapeMeshBuilder();


        /// Buildes the mesh.
        ///
        /// @param shape     [in] The collision shape whose mesh representation is being built.
        /// @param transform [in] The transform to apply to the mesh.
        void Build(const btCollisionShape &shape, const glm::mat4 &transform = glm::mat4());

        /// Builds and merges a mesh representing the given shape.
        ///
        /// @param shape     [in] The collision shape to build the goemtry from.
        /// @param transform [in] The local transform to apply to the collision shape.
        void BuildAndMerge(const btCollisionShape &shape, const glm::mat4 &transform = glm::mat4());
    };
}

#endif