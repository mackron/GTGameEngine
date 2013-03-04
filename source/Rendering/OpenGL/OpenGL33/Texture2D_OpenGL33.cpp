// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Texture2D_OpenGL33.hpp"

namespace GTEngine
{
    Texture2D_OpenGL33::Texture2D_OpenGL33(GLuint* objectGLIn, GLenum targetIn)
        : objectGL(objectGLIn), target(targetIn)
    {
    }

    Texture2D_OpenGL33::~Texture2D_OpenGL33()
    {
    }


    GLuint* Texture2D_OpenGL33::GetOpenGLObjectPtr() const
    {
        return this->objectGL;
    }

    GLenum Texture2D_OpenGL33::GetTarget() const
    {
        return this->target;
    }
}