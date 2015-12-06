// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_VertexArrayLibrary
#define GT_VertexArrayLibrary

#include "Rendering/VertexArray.hpp"
#include "Physics.hpp"
#include "MeshBuilder.hpp"
#include "ConvexHull.hpp"

namespace GT
{
    class Context;

    class VertexArrayLibrary
    {
    public:

        /// Constructor.
        VertexArrayLibrary(Context &context);

        /// Destructor.
        ~VertexArrayLibrary();


        /// Starts up the library.
        bool Startup();

        /// Shuts down the library.
        void Shutdown();




        ////////////////////////////////////////////////////////////////
        // Primitive Shapes

        /// Creates a box shape.
        ///
        /// @param halfWidth  [in] The half width of the box to create.
        /// @param halfHeight [in] The half height of the box to create.
        /// @param halfDepth  [in] The half depth of the box to create.
        ///
        /// @remarks
        ///     The returned vertex array will be in P3T2N3T3B3.
        VertexArray* CreateBox(float halfWidth, float halfHeight, float halfDepth);
        VertexArray* CreateBox(const glm::vec3 &halfExtents) { return CreateBox(halfExtents.x, halfExtents.y, halfExtents.z); }
        VertexArray* CreateBox(const btVector3 &halfExtents) { return CreateBox(halfExtents.x(), halfExtents.y(), halfExtents.z()); }



        ////////////////////////////////////////////////////////////////
        // Wireframe Primitives

        /// Creates a wireframe AABB.
        VertexArray* CreateWireframeFromAABB(const btAABB &aabb);



        ////////////////////////////////////////////////////////////////
        // Convex Hulls

        /// Creates a vertex array from a convex hull.
        ///
        /// @param convexHull [in] A reference to the convex hull object.
        ///
        /// @remarks
        ///     The returned vertex array will be in P3T2N3T3B3, with flat shaded faces.
        VertexArray* CreateFromConvexHull(const ConvexHull &convexHull);


        ////////////////////////////////////////////////////////////////
        // Mesh Builders

        /// Creates a P3T2N3T3B3 vertex array from a MeshBuilderP3T2N3.
        ///
        /// @param builder [in] A reference to the builder to use for creating the vertex array.
        VertexArray* CreateFromBuilder(const MeshBuilderP3T2N3 &builder);



        ////////////////////////////////////////////////////////////////
        // Combining

        /// Combines the given array of vertex arrays to create a new one containing all of the vertex data.
        ///
        /// @param vertexArrays [in] A pointer to the buffer containing the vertex arrays to be combined.
        /// @param count        [in] The number of vertex arrays to combine.
        /// @param format       [in] The format of the new vertex array.
        ///
        /// @remarks
        ///     This function asserts that all input vertex arrays are the same format.
        ///     @par
        ///     Any vertex attributes in <format> that do not appear in the formats of the input arrays will be left undefined.
        VertexArray* CreateCombined(const VertexArray* const* vertexArrays, size_t count, const VertexFormat &format);



        ////////////////////////////////////////////////
        // Deleting

        /// Immediate deletes a vertex array.
        ///
        /// @param va [in] The vertex array to deleted.
        ///
        /// @remarks
        ///     Use MarkForCollection() to use the garbage collector, which will do everything thread-safely.
        void Delete(VertexArray* va);



    public:

        /// Retrieves a P2 vertex array that can used for fullscreen quads.
        VertexArray* GetFullscreenQuadVA();

        /// Retrieves a P2T2 vertex array that can be used for a fullscreen texture write.
        VertexArray* GetFullscreenTriangleVA();

        


    private:

        /// A reference to the main context.
        Context &m_context;

        VertexArray* m_fullscreenQuad;
        VertexArray* m_fullscreenTriangle;
    };
}

#endif