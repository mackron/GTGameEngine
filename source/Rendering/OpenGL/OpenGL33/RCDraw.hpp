// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCDraw_hpp_
#define __GTEngine_Rendering_OpenGL33_RCDraw_hpp_

#include "../../RenderCommand.hpp"
#include <GTEngine/Rendering/VertexFormat.hpp>
#include <GTEngine/Rendering/DrawModes.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCDraw : public RenderCommand
    {
    public:

        /// Constructor.
        RCDraw();

        /// Renderer::Draw()
        void Draw(GLuint* vertexArrayObjectIn, GLuint* vertexBufferObjectIn, GLenum drawModeIn, GLsizei indexCountIn);

        /// Renderer::Draw()
        void Draw(const float* vertices, GLsizei vertexCount, const unsigned int* indices, GLsizei indexCount, const VertexFormat &format, GLenum drawMode);



        /// RenderCommand::Execute()
        void Execute();


    private:

        /// The type of draw command being performed.
        uint32_t drawType;


        /// Structure for drawing from a vertex array.
        struct
        {
            /// The vertex array object.
            GLuint* vertexArrayObject;

            /// The vertex buffer object bound to GL_ARRAY_BUFFER. For some insane reason this is not part of VAO state so it also needs to be bound...
            GLuint* vertexBufferObject;

            /// The draw mode.
            GLenum drawMode;

            /// The index count.
            GLsizei indexCount;

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

        


    private:    // No copying.
        RCDraw(const RCDraw &);
        RCDraw & operator=(const RCDraw &);
    };
}

#endif
