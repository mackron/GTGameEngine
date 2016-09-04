// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTType_TextMesh
#define __GTType_TextMesh

namespace GT
{
    /// Structure representing a vertex in a mesh.
    struct TextMeshVertex
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


    /// Class representing a mesh containing the geometric information for rendering.
    ///
    /// The vertex data will be in XY/UV/RGBA format, i.e. 8 floating point values for each vertex.
    ///
    /// Each mesh can only have a single texture applied to it. This makes it much, much easier to integrate into renderers.
    class TextMesh
    {
    public:

        /// Constructor.
        TextMesh();
        TextMesh(const TextMeshVertex* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, GlyphMapHandle = 0);

        /// Destructor.
        ~TextMesh();


        /// Retrieves a pointer to the buffer containing the vertex data.
        const float* GetVertices() const { return &m_vertices[0].positionX; }

        /// Retrieves the vertex count.
        size_t GetVertexCount() const { return m_vertices.count; }


        /// Retrieves a pointer to the buffer containing the index data.
        const unsigned int* GetIndices() const { return &m_indices[0]; }

        /// Retrieves the index count.
        size_t GetIndexCount() const { return m_indices.count; }



        /// Retrieves the glyph map handle that is being used when rendering the mesh.
        GlyphMapHandle GetGlyphMapHandle() const { return m_glyphMapHandle; }
        
        /// Sets the handle to the glyph map to use when rendering the mesh.
        void SetGlyphMapHandle(GlyphMapHandle glyphMapHandle) { m_glyphMapHandle = glyphMapHandle; }



        /// Sets the colour of every vertex
        void SetVertexColours(float r, float g, float b);



    private:

        /// The list of vertices in the mesh.
        Vector<TextMeshVertex> m_vertices;

        /// The list of indices in the mesh.
        Vector<unsigned int> m_indices;
        
        /// A handle to the glyph map to apply to the mesh at render time. This can be null.
        GlyphMapHandle m_glyphMapHandle;


    private:    // No copying, for now.
        TextMesh(const TextMesh &);
        TextMesh & operator=(const TextMesh &);
    };
}

#endif
