// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Framebuffer_OpenGL21.hpp"

namespace GTEngine
{
    Framebuffer_OpenGL21::Framebuffer_OpenGL21(FramebufferState_OpenGL21* stateGLIn)
        : Framebuffer(), stateGL(stateGLIn)
    {
    }

    Framebuffer_OpenGL21::~Framebuffer_OpenGL21()
    {
    }


    FramebufferState_OpenGL21* Framebuffer_OpenGL21::GetOpenGLState() const
    {
        return this->stateGL;
    }
}