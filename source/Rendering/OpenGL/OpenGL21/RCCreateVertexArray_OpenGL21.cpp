// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCCreateVertexArray_OpenGL21.hpp"
#include "ServerState_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCCreateVertexArray::RCCreateVertexArray()
        : vertexBufferObject(nullptr), indexBufferObject(nullptr), format()
    {
    }

    void RCCreateVertexArray::CreateVertexArray(GLuint* vertexBufferObjectIn, GLuint* indexBufferObjectIn, const VertexFormat &formatIn)
    {
        this->vertexBufferObject = vertexBufferObjectIn;
        this->indexBufferObject  = indexBufferObjectIn;
        this->format             = formatIn;
    }

    void RCCreateVertexArray::Execute()
    {
        glGenBuffers(1, this->vertexBufferObject);
        glGenBuffers(1, this->indexBufferObject);
    }
}
