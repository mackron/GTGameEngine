// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateFramebuffer.hpp"
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    RCCreateFramebuffer::RCCreateFramebuffer()
        : framebufferState(nullptr)
    {
    }


    void RCCreateFramebuffer::CreateFramebuffer(FramebufferState_OpenGL33* framebufferStateIn)
    {
        this->framebufferState = framebufferStateIn;
    }


    void RCCreateFramebuffer::Execute()
    {
        glGenFramebuffers(1, &this->framebufferState->framebufferObject);
    }
}
