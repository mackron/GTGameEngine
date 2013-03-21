// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateVertexArray.hpp"
#include "ServerState_OpenGL33.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCCreateVertexArray::RCCreateVertexArray()
        : vertexArrayObject(nullptr), vertexBufferObject(nullptr), indexBufferObject(nullptr), format()
    {
    }

    void RCCreateVertexArray::CreateVertexArray(GLuint* vertexArrayObjectIn, GLuint* vertexBufferObjectIn, GLuint* indexBufferObjectIn, const VertexFormat &formatIn)
    {
        this->vertexArrayObject  = vertexArrayObjectIn;
        this->vertexBufferObject = vertexBufferObjectIn;
        this->indexBufferObject  = indexBufferObjectIn;
        this->format             = formatIn;
    }

    void RCCreateVertexArray::Execute()
    {
        // 1) Create the objects.
        glGenVertexArrays(1, this->vertexArrayObject);
        glGenBuffers(1, this->vertexBufferObject);
        glGenBuffers(1, this->indexBufferObject);


        // 2) Bind the vertex array object. This will be restored later.
        if (ServerState_GL_VERTEX_ARRAY_BINDING != *this->vertexArrayObject)
        {
            glBindVertexArray(*this->vertexArrayObject);
        }


        // 3) Bind the vertex and index arrays.
        //
        // GL_ARRAY_BUFFER is not stored in the VAO state. This needs to be bound alongside the VAO whenever it's bound. As usual, this will be restored afterwards so
        // that the global state can keep it's integrity.
        if (ServerState_GL_ARRAY_BUFFER_BINDING != *this->vertexBufferObject)
        {
            glBindBuffer(GL_ARRAY_BUFFER, *this->vertexBufferObject);
        }


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->indexBufferObject);        // <-- This one is part of the VAO state, so no need to retrieve the previous binding for it.



        // 4) Enable the vertex attributes supplied by 'format'.
        size_t  formatArraySize   = format.GetAttributeCount();
        GLsizei formatSizeInBytes = static_cast<GLsizei>(format.GetSizeInBytes());
        int     offset = 0;

        for (size_t i = 0; i < formatArraySize; i += 2)
        {
            GLuint  attribIndex = static_cast<GLuint>(format[i]);
            GLint   attribSize  = static_cast<GLint>( format[i + 1]);

            glEnableVertexAttribArray(attribIndex);
            glVertexAttribPointer(attribIndex, attribSize, GL_FLOAT, GL_FALSE, formatSizeInBytes, reinterpret_cast<const GLvoid*>(offset));

            // The offset must be set AFTER glVertexAttribPointer().
            offset += attribSize * sizeof(float);
        }


        // 5) Bind the previous vertex array so we can restore global state..
        if (ServerState_GL_VERTEX_ARRAY_BINDING != *this->vertexArrayObject)
        {
            glBindVertexArray(ServerState_GL_VERTEX_ARRAY_BINDING);
        }

        if (ServerState_GL_ARRAY_BUFFER_BINDING != *this->vertexBufferObject)
        {
            glBindBuffer(GL_ARRAY_BUFFER, ServerState_GL_ARRAY_BUFFER_BINDING);
        }
    }
}
