// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateFramebuffer_OpenGL21.hpp"
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    RCCreateFramebuffer::RCCreateFramebuffer()
        : framebufferState(nullptr)
    {
    }

    RCCreateFramebuffer::~RCCreateFramebuffer()
    {
    }


    void RCCreateFramebuffer::CreateFramebuffer(FramebufferState_OpenGL21* framebufferStateIn)
    {
        this->framebufferState = framebufferStateIn;
    }


    void RCCreateFramebuffer::Execute()
    {
        this->__glGenFramebuffers(1, &this->framebufferState->framebufferObject);
    }




    ///////////////////////////////
    // EXT_framebuffer_object

    void RCCreateFramebufferEXT::__glGenFramebuffers(GLsizei n, GLuint *framebuffers)
    {
        glGenFramebuffersEXT(n, framebuffers);
    }



    ///////////////////////////////
    // ARB_framebuffer_object

    void RCCreateFramebufferARB::__glGenFramebuffers(GLsizei n, GLuint *framebuffers)
    {
        glGenFramebuffers(n, framebuffers);
    }
}
