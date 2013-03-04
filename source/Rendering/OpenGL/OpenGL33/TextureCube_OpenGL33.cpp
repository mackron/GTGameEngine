// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "TextureCube_OpenGL33.hpp"

namespace GTEngine
{
    TextureCube_OpenGL33::TextureCube_OpenGL33(GLuint* objectGLIn)
        : objectGL(objectGLIn)
    {
    }

    TextureCube_OpenGL33::~TextureCube_OpenGL33()
    {
    }


    GLuint* TextureCube_OpenGL33::GetOpenGLObjectPtr() const
    {
        return this->objectGL;
    }
}