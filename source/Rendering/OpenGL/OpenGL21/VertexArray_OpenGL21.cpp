// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "VertexArray_OpenGL21.hpp"

namespace GT
{
    VertexArray_OpenGL21::VertexArray_OpenGL21(VertexArrayUsage usage, const VertexFormat &format, GLuint* vertexObjectIn, GLuint* indexObjectIn)
        : VertexArray(usage, format),
          vertexObjectGL(vertexObjectIn), indexObjectGL(indexObjectIn),
          vertexDataNeedsUpdating(false), indexDataNeedsUpdating(false)
    {
    }

    VertexArray_OpenGL21::~VertexArray_OpenGL21()
    {
    }




    ///////////////////////////////////////////
    // Events.

    void VertexArray_OpenGL21::OnVertexDataChanged()
    {
        this->vertexDataNeedsUpdating = true;
    }

    void VertexArray_OpenGL21::OnIndexDataChanged()
    {
        this->indexDataNeedsUpdating = true;
    }
}