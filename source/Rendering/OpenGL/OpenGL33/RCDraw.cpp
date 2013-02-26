// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDraw.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDraw::RCDraw()
        : vertexArrayObject(nullptr), drawMode(0), indexCount(0)
    {
    }

    void RCDraw::Draw(GLuint* vertexArrayObjectIn, GLenum drawModeIn, GLsizei indexCountIn)
    {
        this->vertexArrayObject = vertexArrayObjectIn;
        this->drawMode          = drawModeIn;
        this->indexCount        = indexCountIn;
    }

    void RCDraw::Execute()
    {
        // 1) Bind the vertex array.
        glBindVertexArray(*this->vertexArrayObject);

        // 2) Draw.
        glDrawElements(this->drawMode, this->indexCount, GL_UNSIGNED_INT, 0);
    }
}
