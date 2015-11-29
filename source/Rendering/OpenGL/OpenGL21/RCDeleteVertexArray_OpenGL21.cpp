// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCDeleteVertexArray_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GT
{
    RCDeleteVertexArray::RCDeleteVertexArray()
        : vertexBufferObject(nullptr), indexBufferObject(nullptr)
    {
    }

    void RCDeleteVertexArray::DeleteVertexArray(GLuint* vertexBufferObjectIn, GLuint* indexBufferObjectIn)
    {
        this->vertexBufferObject = vertexBufferObjectIn;
        this->indexBufferObject  = indexBufferObjectIn;
    }

    void RCDeleteVertexArray::Execute()
    {
        glDeleteBuffers(1, this->vertexBufferObject);
        glDeleteBuffers(1, this->indexBufferObject);
    }
}
