// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDraw.hpp"
#include "ServerState_OpenGL33.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDraw::RCDraw()
        : vertexArrayObject(nullptr), vertexBufferObject(nullptr), drawMode(0), indexCount(0)
    {
    }

    void RCDraw::Draw(GLuint* vertexArrayObjectIn, GLuint* vertexBufferObjectIn, GLenum drawModeIn, GLsizei indexCountIn)
    {
        this->vertexArrayObject  = vertexArrayObjectIn;
        this->vertexBufferObject = vertexBufferObjectIn;
        this->drawMode           = drawModeIn;
        this->indexCount         = indexCountIn;
    }

    void RCDraw::Execute()
    {
        // NOTE: It's OK to break global state with this call. It's considered and handled by the renderer.

        // 1) Bind the vertex array.
        glBindVertexArray(*this->vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, *this->vertexBufferObject);


        // 2) Draw.
        glDrawElements(this->drawMode, this->indexCount, GL_UNSIGNED_INT, 0);


        // 3) Set state.
        ServerState_GL_VERTEX_ARRAY_BINDING = *this->vertexArrayObject;
        ServerState_GL_ARRAY_BUFFER_BINDING = *this->vertexArrayObject;
    }
}
