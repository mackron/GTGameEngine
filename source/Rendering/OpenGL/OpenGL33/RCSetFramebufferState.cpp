// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetFramebufferState.hpp"
#include "../TypeConversion.hpp"
#include <assert.h>

namespace GTEngine
{
    #define SET_ATTACHMENTS_BIT          (1 << 1)


    RCSetFramebufferState::RCSetFramebufferState()
        : operationBitfield(0),
          framebufferState(nullptr),
          attachments()
    {
    }


    void RCSetFramebufferState::SetAttachedBuffer(FramebufferState_OpenGL33* framebufferStateIn, GLenum attachmentPoint, GLenum textureTarget, GLuint* textureObject)
    {
        assert(framebufferStateIn != nullptr && (this->framebufferState == nullptr || this->framebufferState == framebufferStateIn));
        {
            FramebufferState_OpenGL33::Attachment attachment;
            attachment.attachmentPoint = attachmentPoint;
            attachment.textureTarget   = textureTarget;
            attachment.textureObject   = textureObject;

            this->attachments.Add(attachmentPoint, attachment);

            this->framebufferState   = framebufferStateIn;
            this->operationBitfield |= SET_ATTACHMENTS_BIT;
        }
    }


    FramebufferState_OpenGL33* RCSetFramebufferState::GetFramebufferState()
    {
        return this->framebufferState;
    }


    void RCSetFramebufferState::Execute()
    {
        assert(this->framebufferState != nullptr);
        {
            // We don't want this to modify global state, so we'll need to grab the currently bound framebuffer so it can be restored later.
            GLint previousCurrentFramebuffer;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousCurrentFramebuffer);        // <-- framebuffers are always bound with GL_FRAMEBUFFER, so we can also use GL_READ_FRAMEBUFFER_BINDING if we wanted to.

            if (static_cast<GLuint>(previousCurrentFramebuffer) != this->framebufferState->framebufferObject)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferState->framebufferObject);
            }


            if ((this->operationBitfield & SET_ATTACHMENTS_BIT))
            {
                // We loop over the new attachments and mark them as such. Any attachment that was previously attached, but is not anymore needs to be explicitly detached. We will
                // do the detachments first.

                // 1) Detach.
                for (size_t i = 0; i < this->framebufferState->attachments.count; ++i)
                {
                    auto &attachmentToRemove = this->framebufferState->attachments.buffer[i]->value;

                    if (!this->attachments.Exists(attachmentToRemove.attachmentPoint))
                    {
                        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachmentToRemove.attachmentPoint, attachmentToRemove.textureTarget, 0, 0);
                    }
                }


                // 2) Attach.
                for (size_t i = 0; i < this->attachments.count; ++i)
                {
                    auto &attachmentToAdd = this->attachments.buffer[i]->value;

                    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachmentToAdd.attachmentPoint, attachmentToAdd.textureTarget, *attachmentToAdd.textureObject, 0);
                }
            }


            // Global state needs to be restored.
            if (static_cast<GLuint>(previousCurrentFramebuffer) != this->framebufferState->framebufferObject)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousCurrentFramebuffer));
            }
        }
    }
}
