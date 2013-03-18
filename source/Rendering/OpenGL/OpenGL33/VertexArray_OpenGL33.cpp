// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "VertexArray_OpenGL33.hpp"

namespace GTEngine
{
    VertexArray_OpenGL33::VertexArray_OpenGL33(VertexArrayUsage usage, const VertexFormat &format, GLuint* objectGLIn, GLuint* vertexObjectIn, GLuint* indexObjectIn)
        : VertexArray(usage, format),
          objectGL(objectGLIn), vertexObjectGL(vertexObjectIn), indexObjectGL(indexObjectIn),
          vertexDataNeedsUpdating(false), indexDataNeedsUpdating(false)
    {
    }

    VertexArray_OpenGL33::~VertexArray_OpenGL33()
    {
    }




    ///////////////////////////////////////////
    // Events.

    void VertexArray_OpenGL33::OnVertexDataChanged()
    {
        this->vertexDataNeedsUpdating = true;
    }

    void VertexArray_OpenGL33::OnIndexDataChanged()
    {
        this->indexDataNeedsUpdating = true;
    }
}