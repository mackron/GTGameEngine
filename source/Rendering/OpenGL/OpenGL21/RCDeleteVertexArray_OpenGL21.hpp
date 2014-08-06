// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_OpenGL21_RCDeleteVertexArray_hpp_
#define __GTEngine_Rendering_OpenGL21_RCDeleteVertexArray_hpp_

#include "../../RenderCommand.hpp"
#include <GTEngine/Rendering/VertexFormat.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
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
