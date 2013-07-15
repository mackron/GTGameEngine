// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL21_RCSetVertexArrayState_hpp_
#define __GTEngine_Rendering_OpenGL21_RCSetVertexArrayState_hpp_

#include "../../RenderCommand.hpp"
#include <GTEngine/Rendering/VertexFormat.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetVertexArrayState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetVertexArrayState();

        /// Renderer::SetVertexArrayVertexData()
        void SetVertexData(GLuint* vertexArrayObject, GLuint* vertexBufferObject, const float* vertices, size_t vertexCount, size_t vertexSizeInBytes, GLenum usage);

        /// Renderer::SetVertexArrayIndexData()
        void SetIndexData(GLuint* vertexArrayObject, const unsigned int* indices, size_t indexCount, GLenum usage);


        /// Retrieves the vertex array object whose state is being set.
        GLuint* GetVertexArrayObject();


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// The vertex array object whose state is being modified.
        GLuint* vertexArrayObject;

        /// The vertex buffer object that would usually be bound to GL_ARRAY_BUFFER.
        GLuint* vertexBufferObject;


        /// The vertex data to set.
        struct
        {
            float* vertices;
            size_t count;
            size_t vertexSizeInBytes;
            GLenum usage;

        }vertexData;

        /// The index data to set.
        struct
        {
            unsigned int* indices;
            size_t        count;
            GLenum        usage;

        }indexData;


    private:    // No copying.
        RCSetVertexArrayState(const RCSetVertexArrayState &);
        RCSetVertexArrayState & operator=(const RCSetVertexArrayState &);
    };
}

#endif
