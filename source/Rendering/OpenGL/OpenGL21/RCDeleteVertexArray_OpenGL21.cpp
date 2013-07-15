// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteVertexArray_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDeleteVertexArray::RCDeleteVertexArray()
        : vertexArrayObject(nullptr), vertexBufferObject(nullptr), indexBufferObject(nullptr)
    {
    }

    void RCDeleteVertexArray::DeleteVertexArray(GLuint* vertexArrayObjectIn, GLuint* vertexBufferObjectIn, GLuint* indexBufferObjectIn)
    {
        this->vertexArrayObject  = vertexArrayObjectIn;
        this->vertexBufferObject = vertexBufferObjectIn;
        this->indexBufferObject  = indexBufferObjectIn;
    }

    void RCDeleteVertexArray::Execute()
    {
        glDeleteVertexArrays(1, this->vertexArrayObject);
        glDeleteBuffers(1, this->vertexBufferObject);
        glDeleteBuffers(1, this->indexBufferObject);
    }
}
