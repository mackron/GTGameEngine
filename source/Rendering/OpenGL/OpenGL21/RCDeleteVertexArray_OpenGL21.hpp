// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_OpenGL21_RCDeleteVertexArray
#define GT_Rendering_OpenGL21_RCDeleteVertexArray

#include "../../RenderCommand.hpp"
#include <GTGE/Rendering/VertexFormat.hpp>
#include <gtgl/gtgl.h>

namespace GT
{
    /// Render command for clearing the buffers/
    class RCDeleteVertexArray : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteVertexArray();


        /// Renderer::CreateVertexArray()
        void DeleteVertexArray(GLuint* vertexBufferObject, GLuint* indexBufferObjectIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL vertex buffer object.
        GLuint* vertexBufferObject;

        /// A pointer to the OpenGL index buffer object.
        GLuint* indexBufferObject;



    private:    // No copying.
        RCDeleteVertexArray(const RCDeleteVertexArray &);
        RCDeleteVertexArray & operator=(const RCDeleteVertexArray &);
    };
}

#endif
