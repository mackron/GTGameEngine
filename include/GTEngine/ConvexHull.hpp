// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ConvexHull
#define GT_ConvexHull

#include "Math.hpp"
#include "ConvexHullBuildSettings.hpp"
#include "Rendering/VertexArray.hpp"
#include <GTEngine/Core/Vector.hpp>

namespace GT
{
    /// Class representing a convex hull.
    class ConvexHull
    {
    public:

        /// Default constructor.
        ConvexHull();

        /// Copy constructor.
        ConvexHull(const ConvexHull &other);

        /// Constructor.
        ///
        /// @param vertices    [in] A pointer to the buffer containing the vertex data.
        /// @param vertexCount [in] The number of vertices.
        /// @param indices     [in] A pointer to the buffer containing the indices.
        /// @param indexCount  [in] The number of indices.
        ///
        /// @remarks
        ///     <vertices> must be tightly packed in xyz format.
        ConvexHull(const float* vertices, unsigned int vertexCount, const unsigned int* indices, unsigned int indexCount);

        /// Destructor.
        ~ConvexHull();


        /// Retrieves a constant pointer to the vertex data.
        const float* GetVertices() const { return &this->vertices[0][0]; }

        /// Retrieves the number of vertices.
        unsigned int GetVertexCount() const { return static_cast<unsigned int>(this->vertices.count); }


        /// Retrieves a constant pointer to the index data.
        const unsigned int* GetIndices() const { return &this->indices[0]; }

        /// Retrieves the number of indices.
        unsigned int GetIndexCount() const { return static_cast<unsigned int>(this->indices.count); }


        /// Builds the convex hull from input data.
        ///
        /// @param vertices    [in] A pointer to the buffer containing the vertex data.
        /// @param vertexCount [in] The number of vertices.
        /// @param indices     [in] A pointer to the buffer containing the indices.
        /// @param indexCount  [in] The number of indices.
        ///
        /// @remarks
        ///     <vertices> must be tightly packed in xyz format.
        void Build(const float* vertices, unsigned int vertexCount, const unsigned int* indices, unsigned int indexCount);


        /// Assignment operator.
        ConvexHull & operator=(const ConvexHull &other)
        {
            this->vertices = other.vertices;
            this->indices  = other.indices;

            return *this;
        }


    private:

        /// The list of points making up the vertices in the hull.
        Vector<glm::vec3> vertices;

        /// The list of indices making up the triangles in the hull. This is needed for rendering.
        Vector<unsigned int> indices;



    /////////////////////////////////////////////////////
    // Static Functions.
    public:

        /// Builds a list of convex hulls making up the geometry of the the given vertex array.
        ///
        /// @remarks
        ///     Delete each returned convex hull with delete [] or DeleteConvexHulls().
        static void BuildConvexHulls(const VertexArray &va, ConvexHull* &outputArray, unsigned int &outputCount, ConvexHullBuildSettings &settings);

        /// Deletes the convex hulls returned by BuildConvexHulls().
        ///
        /// @param convexHulls [in] A pointer to the array of convex hulls to delete.
        static void DeleteConvexHulls(ConvexHull* convexHulls);
    };
}

#endif
