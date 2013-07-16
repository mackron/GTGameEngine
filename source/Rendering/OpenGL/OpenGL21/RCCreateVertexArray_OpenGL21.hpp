// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL21_RCCreateVertexArray_hpp_
#define __GTEngine_Rendering_OpenGL21_RCCreateVertexArray_hpp_

#include "../../RenderCommand.hpp"
#include <GTEngine/Rendering/VertexFormat.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCCreateVertexArray : public RenderCommand
    {
    public:

        /// Constructor.
        RCCreateVertexArray();


        /// Renderer::CreateVertexArray()
        void CreateVertexArray(GLuint* vertexBufferObject, GLuint* indexBufferObjectIn, const VertexFormat &formatIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL vertex buffer object.
        GLuint* vertexBufferObject;

        /// A pointer to the OpenGL index buffer object.
        GLuint* indexBufferObject;

        /// The format of the vertex data.
        VertexFormat format;



    private:    // No copying.
        RCCreateVertexArray(const RCCreateVertexArray &);
        RCCreateVertexArray & operator=(const RCCreateVertexArray &);
    };
}

#endif
