// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Framebuffer_OpenGL33.hpp"

namespace GTEngine
{
    Framebuffer_OpenGL33::Framebuffer_OpenGL33(GLuint* objectGLIn)
        : Framebuffer(), objectGL(objectGLIn)
    {
    }

    Framebuffer_OpenGL33::~Framebuffer_OpenGL33()
    {
    }


    GLuint* Framebuffer_OpenGL33::GetOpenGLObjectPtr()
    {
        return this->objectGL;
    }
}