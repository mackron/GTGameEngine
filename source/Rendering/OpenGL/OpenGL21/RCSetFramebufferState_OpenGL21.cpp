// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetFramebufferState_OpenGL21.hpp"
#include "ServerState_OpenGL21.hpp"
#include "../TypeConversion.hpp"
#include <assert.h>

namespace GTEngine
{
    #define SET_ATTACHMENTS_BIT                         (1 << 1)
    #define SET_WRITE_ONLY_DEPTH_STENCIL_BUFFER_BIT     (1 << 2)


    RCSetFramebufferState::RCSetFramebufferState()
        : operationBitfield(0),
          framebufferState(nullptr),
          attachments()
    {
    }

    RCSetFramebufferState::~RCSetFramebufferState()
    {
    }


    void RCSetFramebufferState::SetAttachedBuffer(FramebufferState_OpenGL21* framebufferStateIn, GLenum attachmentPoint, GLenum textureTarget, TextureState_OpenGL21* textureState)
    {
        assert(framebufferStateIn != nullptr && (this->framebufferState == nullptr || this->framebufferState == framebufferStateIn));
        {
            FramebufferState_OpenGL21::Attachment attachment;
            attachment.attachmentPoint = attachmentPoint;
            attachment.textureTarget   = textureTarget;
            attachment.textureState    = textureState;

            this->attachments.Add(attachmentPoint, attachment);

            this->framebufferState   = framebufferStateIn;
            this->operationBitfield |= SET_ATTACHMENTS_BIT;
        }
    }

    void RCSetFramebufferState::SetWriteOnlyDepthStencilBuffer(FramebufferState_OpenGL21* framebufferStateIn, GLboolean attached, GLsizei width, GLsizei height)
    {
        assert(framebufferStateIn != nullptr && (this->framebufferState == nullptr || this->framebufferState == framebufferStateIn));
        {
            this->attachWriteOnlyDepthStencilBuffer = attached;
            this->writeOnlyDepthStencilBufferWidth  = width;
            this->writeOnlyDepthStencilBufferHeight = height;

            this->framebufferState = framebufferStateIn;
            this->operationBitfield |= SET_WRITE_ONLY_DEPTH_STENCIL_BUFFER_BIT;
        }
    }


    FramebufferState_OpenGL21* RCSetFramebufferState::GetFramebufferState()
    {
        return this->framebufferState;
    }


    void RCSetFramebufferState::Execute()
    {
        assert(this->framebufferState != nullptr);
        {
            // We don't want this to modify global state, so we'll need to grab the currently bound framebuffer so it can be restored later.
            if (ServerState_GL_FRAMEBUFFER_BINDING != this->framebufferState->framebufferObject)
            {
                this->__glBindFramebuffer(GL_FRAMEBUFFER_EXT, this->framebufferState->framebufferObject);
            }


            if ((this->operationBitfield & SET_ATTACHMENTS_BIT))
            {
                // We loop over the new attachments and mark them as such. Any attachment that was previously attached, but is not anymore needs to be explicitly detached. We will
                // do the detachments first.

                // 1) Detach.
                for (size_t i = 0; i < this->framebufferState->attachments.count; )
                {
                    auto &attachmentToRemove = this->framebufferState->attachments.buffer[i]->value;

                    if (!this->attachments.Exists(attachmentToRemove.attachmentPoint))
                    {
                        this->__glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachmentToRemove.attachmentPoint, attachmentToRemove.textureTarget, 0, 0);
                        this->framebufferState->attachments.RemoveByIndex(i);
                    }
                    else
                    {
                        ++i;
                    }
                }


                // 2) Attach.
                for (size_t i = 0; i < this->attachments.count; ++i)
                {
                    auto &attachmentToAdd = this->attachments.buffer[i]->value;

                    this->__glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachmentToAdd.attachmentPoint, attachmentToAdd.textureTarget, attachmentToAdd.textureState->objectGL, 0);
                    this->framebufferState->attachments.Add(attachmentToAdd.attachmentPoint, attachmentToAdd);
                }
            }


            // Write-only depth/stencil buffer.
            if ((this->operationBitfield & SET_WRITE_ONLY_DEPTH_STENCIL_BUFFER_BIT))
            {
                if (this->attachWriteOnlyDepthStencilBuffer)
                {
                    // We're attaching.
                    bool needAttach = false;
                    if (this->framebufferState->depthStencilRenderbuffer == 0)
                    {
                        this->__glGenRenderbuffers(1, &this->framebufferState->depthStencilRenderbuffer);
                        needAttach = true;
                    }

                    if (needAttach || this->framebufferState->depthStencilRenderbufferWidth != this->writeOnlyDepthStencilBufferWidth || this->framebufferState->depthStencilRenderbufferHeight != this->writeOnlyDepthStencilBufferHeight)
                    {
                        this->__glBindRenderbuffer(GL_RENDERBUFFER_EXT, this->framebufferState->depthStencilRenderbuffer);
                        {
                            this->__glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, this->writeOnlyDepthStencilBufferWidth, this->writeOnlyDepthStencilBufferHeight);
                            
                            this->framebufferState->depthStencilRenderbufferWidth  = this->writeOnlyDepthStencilBufferWidth;
                            this->framebufferState->depthStencilRenderbufferHeight = this->writeOnlyDepthStencilBufferHeight;
                        }
                        this->__glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
                    }

                    if (needAttach)
                    {
                        this->__glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,   GL_RENDERBUFFER_EXT, this->framebufferState->depthStencilRenderbuffer);
                        this->__glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->framebufferState->depthStencilRenderbuffer);
                    }
                }
                else
                {
                    // We're detaching.
                    if (this->framebufferState->depthStencilRenderbuffer != 0)
                    {
                        this->__glDeleteRenderbuffers(1, &this->framebufferState->depthStencilRenderbuffer);

                        this->framebufferState->depthStencilRenderbuffer = 0;
                        this->framebufferState->depthStencilRenderbufferWidth  = 0;
                        this->framebufferState->depthStencilRenderbufferHeight = 0;
                    }
                }
            }


            // Global state needs to be restored.
            if (ServerState_GL_FRAMEBUFFER_BINDING != this->framebufferState->framebufferObject)
            {
                this->__glBindFramebuffer(GL_FRAMEBUFFER, ServerState_GL_FRAMEBUFFER_BINDING);
            }
        }
    }


    ///////////////////////////////
    // EXT_framebuffer_object

    void RCSetFramebufferStateEXT::__glBindFramebuffer(GLenum target, GLuint framebuffer)
    {
        glBindFramebufferEXT(target, framebuffer);
    }

    void RCSetFramebufferStateEXT::__glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
    {
        glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
    }

    void RCSetFramebufferStateEXT::__glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
    {
        glGenRenderbuffersEXT(n, renderbuffers);
    }

    void RCSetFramebufferStateEXT::__glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
    {
        glDeleteRenderbuffersEXT(n, renderbuffers);
    }

    void RCSetFramebufferStateEXT::__glBindRenderbuffer(GLenum target, GLuint renderbuffer)
    {
        glBindRenderbufferEXT(target, renderbuffer);
    }

    void RCSetFramebufferStateEXT::__glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
    {
        glRenderbufferStorageEXT(target, internalformat, width, height);
    }

    void RCSetFramebufferStateEXT::__glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
    {
        glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
    }


    ///////////////////////////////
    // ARB_framebuffer_object

    void RCSetFramebufferStateARB::__glBindFramebuffer(GLenum target, GLuint framebuffer)
    {
        glBindFramebuffer(target, framebuffer);
    }

    void RCSetFramebufferStateARB::__glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
    {
        glFramebufferTexture2D(target, attachment, textarget, texture, level);
    }

    void RCSetFramebufferStateARB::__glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
    {
        glGenRenderbuffers(n, renderbuffers);
    }

    void RCSetFramebufferStateARB::__glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
    {
        glDeleteRenderbuffers(n, renderbuffers);
    }

    void RCSetFramebufferStateARB::__glBindRenderbuffer(GLenum target, GLuint renderbuffer)
    {
        glBindRenderbuffer(target, renderbuffer);
    }

    void RCSetFramebufferStateARB::__glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
    {
        glRenderbufferStorage(target, internalformat, width, height);
    }

    void RCSetFramebufferStateARB::__glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
    {
        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    }
}
