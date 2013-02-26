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
        void Draw(GLuint* vertexArrayObjectIn, GLenum drawModeIn, GLsizei indexCountIn);



        /// RenderCommand::Execute()
        void Execute();


    private:

        /// The vertex array object.
        GLuint* vertexArrayObject;

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
