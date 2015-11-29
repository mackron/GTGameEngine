// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "Framebuffer_OpenGL21.hpp"

namespace GT
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