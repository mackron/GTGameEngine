// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_OpenGL21_RCDraw
#define GT_Rendering_OpenGL21_RCDraw

#include "../../RenderCommand.hpp"
#include <GTGE/Rendering/VertexFormat.hpp>
#include <GTGE/Rendering/DrawModes.hpp>
#include <gtgl/gtgl.h>

namespace GT
{
    /// Render command for clearing the buffers/
    class RCDraw : public RenderCommand
    {
    public:

        /// Constructor.
        RCDraw();

        /// Destructor.
        ~RCDraw();

        /// Renderer::Draw()
        void Draw(GLuint* vertexBufferObjectIn, GLuint* indexBufferObjectIn, GLsizei indexCount, const VertexFormat &format, GLenum drawMode);

        /// Renderer::Draw()
        void Draw(const float* vertices, GLsizei vertexCount, const unsigned int* indices, GLsizei indexCount, const VertexFormat &format, GLenum drawMode);



        /// RenderCommand::Execute()
        void Execute();


    private:

        /// Sets the vertex attribute state from the given format and vertex pointer.
        ///
        /// @param format   [in] The vertex format.
        /// @param vertices [in] A pointer to the buffer containing the vertices. Can be null.
        void SetVertexAttribState(const VertexFormat &format, const float* vertices);


    private:

        /// The type of draw command being performed.
        uint32_t drawType;

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
#endif

        /// Structure for drawing from a vertex array.
        struct
        {
            /// The vertex buffer object bound to GL_ARRAY_BUFFER. For some insane reason this is not part of VAO state so it also needs to be bound...
            GLuint* vertexBufferObject;

            /// The vertex buffer object bound to GL_ELEMENT_ARRAY_BUFFER.
            GLuint* indexBufferObject;

            /// The index count.
            GLsizei indexCount;

            /// The vertex format.
            VertexFormat format;

            /// The draw mode.
            GLenum drawMode;


        }drawVA;


        /// Structure for drawing from raw input.
        struct
        {
            /// The buffer containing the vertices.
            float* vertices;

            /// The size in bytes of the vertex buffer.
            GLsizei vertexBufferSize;

            /// The buffer containing the indices.
            unsigned int* indices;

            /// The index count.
            GLsizei indexCount;

            /// The vertex format.
            VertexFormat format;

            /// The draw mode (triangles or lines).
            GLenum drawMode;


        }drawRaw;

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


    private:    // No copying.
        RCDraw(const RCDraw &);
        RCDraw & operator=(const RCDraw &);
    };
}

#endif
