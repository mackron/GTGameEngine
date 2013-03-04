// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCDraw_hpp_
#define __GTEngine_Rendering_OpenGL33_RCDraw_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCDraw : public RenderCommand
    {
    public:

        /// Constructor.
        RCDraw();

        /// Renderer::Clear()
        void Draw(GLuint* vertexArrayObjectIn, GLuint* vertexBufferObjectIn, GLenum drawModeIn, GLsizei indexCountIn);



        /// RenderCommand::Execute()
        void Execute();


    private:

        /// The vertex array object.
        GLuint* vertexArrayObject;

        /// The vertex buffer object bound to GL_ARRAY_BUFFER. For some insane reason this is not part of VAO state so it also needs to be bound...
        GLuint* vertexBufferObject;

        /// The draw mode.
        GLenum drawMode;

        /// The index count.
        GLsizei indexCount;


    private:    // No copying.
        RCDraw(const RCDraw &);
        RCDraw & operator=(const RCDraw &);
    };
}

#endif
