// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIMesh
#define GT_GUIMesh

#include "../ImageManager.hpp"
#include <GTEngine/Core/Colour.hpp>
#include <GTEngine/Core/Vector.hpp>

namespace GTGUI
{
    /// Structure representing a vertex in a mesh.
    struct GUIMeshVertex
    {
        float positionX;
        float positionY;

        float texCoordX;
        float texCoordY;

        float colourR;
        float colourG;
        float colourB;
        float colourA;
    };

    /// Class containing geometry data that will be send to the renderer for drawing.
    ///
    /// The vertex data will be in XY/UV/RGBA format, i.e. 8 floating point values for each vertex.
    ///
    /// Each mesh can only have a single texture applied to it. This makes it much, much easier to integrate into renderers.
    class GUIMesh
    {
    public:

        /// Constructor.
        GUIMesh();
        GUIMesh(const GUIMeshVertex* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, GUIImageHandle texture = 0);

        /// Destructor.
        ~GUIMesh();


        /// Retrieves a pointer to the buffer containing the vertex data.
        const float* GetVertices() const { return &this->vertices[0].positionX; }

        /// Retrieves the vertex count.
        size_t GetVertexCount() const { return this->vertices.count; }


        /// Retrieves a pointer to the buffer containing the index data.
        const unsigned int* GetIndices() const { return &this->indices[0]; }

        /// Retrieves the index count.
        size_t GetIndexCount() const { return this->indices.count; }




        /// Retrieves the texture being used by this mesh.
        GUIImageHandle GetTexture() const { return this->texture; }

        /// Sets the texture.
        void SetTexture(GUIImageHandle newTexture) { this->texture = newTexture; }


        /// Sets the colour of every vertex
        void SetVertexColours(float r, float g, float b);
        void SetVertexColours(float r, float g, float b, float a);
        void SetVertexColours(const GT::Colour &colour) { this->SetVertexColours(colour.r, colour.g, colour.b, colour.a); }

        /// Sets the alpha component of every vertex.
        void SetVertexAlpha(float a);



        /// Sets the vertex data from P2T2 format, which will commonly be used by fonts.
        ///
        /// @param verticesIn   [in] A pointer to the buffer containing the new vertices.
        /// @param vertexCount  [in] The number of vertices (not floats).
        /// @param indicesIn    [in] A pointer to the buffer containing the new indices.
        /// @param indicesCount [in] The number of indices.
        /// @param colour       [in] The colour of the vertices.
        void SetFromP2T2Colour(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, const GT::Colour &colour);



    private:

        /// The list of vertices in the mesh.
        GT::Vector<GUIMeshVertex> vertices;

        /// The list of indices in the mesh.
        GT::Vector<unsigned int> indices;

        /// A handle to the texture to apply to the mesh at render time.
        GUIImageHandle texture;


    private:    // No copying for now.
        GUIMesh(const GUIMesh &);
        GUIMesh & operator=(const GUIMesh &);
    };
}

#endif
