// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCDeleteFramebuffer_OpenGL21.hpp"

namespace GTEngine
{
    RCDeleteFramebuffer::RCDeleteFramebuffer()
        : framebufferState(nullptr)
    {
    }

    RCDeleteFramebuffer::~RCDeleteFramebuffer()
    {
    }

    void RCDeleteFramebuffer::DeleteFramebuffer(FramebufferState_OpenGL21* framebufferStateIn)
    {
        this->framebufferState = framebufferStateIn;
    }

    void RCDeleteFramebuffer::Execute()
    {
        this->__glDeleteFramebuffers(1, &this->framebufferState->framebufferObject);
        this->framebufferState->framebufferObject = 0;

        if (this->framebufferState->depthStencilRenderbuffer != 0)
        {
            this->__glDeleteRenderbuffers(1, &this->framebufferState->depthStencilRenderbuffer);
            
            this->framebufferState->depthStencilRenderbuffer = 0;
            this->framebufferState->depthStencilRenderbufferWidth  = 0;
            this->framebufferState->depthStencilRenderbufferHeight = 0;
        }
    }



    ///////////////////////////////
    // EXT_framebuffer_object

    void RCDeleteFramebufferEXT::__glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
    {
        glDeleteFramebuffersEXT(n, framebuffers);
    }

    void RCDeleteFramebufferEXT::__glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
    {
        glDeleteRenderbuffersEXT(n, renderbuffers);
    }



    ///////////////////////////////
    // ARB_framebuffer_object

    void RCDeleteFramebufferARB::__glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
    {
        glDeleteFramebuffers(n, framebuffers);
    }

    void RCDeleteFramebufferARB::__glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
    {
        glDeleteRenderbuffers(n, renderbuffers);
    }
}
