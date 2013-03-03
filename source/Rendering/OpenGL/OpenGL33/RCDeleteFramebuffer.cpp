// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteFramebuffer.hpp"

namespace GTEngine
{
    RCDeleteFramebuffer::RCDeleteFramebuffer()
        : framebufferState(nullptr)
    {
    }

    void RCDeleteFramebuffer::DeleteFramebuffer(FramebufferState_OpenGL33* framebufferStateIn)
    {
        this->framebufferState = framebufferStateIn;
    }

    void RCDeleteFramebuffer::Execute()
    {
        glDeleteFramebuffers(1, &this->framebufferState->framebufferObject);
    }
}
