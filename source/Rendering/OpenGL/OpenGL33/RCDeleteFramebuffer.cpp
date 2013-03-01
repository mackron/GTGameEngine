// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteFramebuffer.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDeleteFramebuffer::RCDeleteFramebuffer()
        : framebufferObject(nullptr)
    {
    }

    void RCDeleteFramebuffer::DeleteFramebuffer(GLuint* framebufferObjectIn)
    {
        this->framebufferObject = framebufferObjectIn;
    }

    void RCDeleteFramebuffer::Execute()
    {
        glDeleteFramebuffers(1, this->framebufferObject);
    }
}
