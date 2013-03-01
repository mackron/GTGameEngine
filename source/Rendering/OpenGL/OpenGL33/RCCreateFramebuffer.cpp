// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateFramebuffer.hpp"
#include <GTEngine/Logging.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCCreateFramebuffer::RCCreateFramebuffer()
        : framebufferObject(nullptr)
    {
    }


    void RCCreateFramebuffer::CreateFramebuffer(GLuint* framebufferObjectIn)
    {
        this->framebufferObject = framebufferObjectIn;
    }


    void RCCreateFramebuffer::Execute()
    {
        glGenFramebuffers(1, this->framebufferObject);
    }
}
