// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Framebuffer_OpenGL33.hpp"

namespace GTEngine
{
    Framebuffer_OpenGL33::Framebuffer_OpenGL33(FramebufferState_OpenGL33* stateGLIn)
        : Framebuffer(), stateGL(stateGLIn)
    {
    }

    Framebuffer_OpenGL33::~Framebuffer_OpenGL33()
    {
    }


    FramebufferState_OpenGL33* Framebuffer_OpenGL33::GetOpenGLState() const
    {
        return this->stateGL;
    }
}