// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateVertexArray.hpp"
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


        // 2) Bind the vertex array object.
        glBindVertexArray(*this->vertexArrayObject);


        // 3) Bind the vertex and index arrays.
        glBindBuffer(GL_ARRAY_BUFFER,         *this->vertexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->indexBufferObject);


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
            offset += attribSize;
        }


        // 5) Bind the default vertex array.
        glBindVertexArray(0);
    }
}
