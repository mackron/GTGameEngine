// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetFramebufferState_OpenGL21.hpp"
#include "ServerState_OpenGL21.hpp"
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
                glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferState->framebufferObject);
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
                        glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachmentToRemove.attachmentPoint, attachmentToRemove.textureTarget, 0, 0);
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

                    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachmentToAdd.attachmentPoint, attachmentToAdd.textureTarget, attachmentToAdd.textureState->objectGL, 0);
                    this->framebufferState->attachments.Add(attachmentToAdd.attachmentPoint, attachmentToAdd);
                }
            }


            // Global state needs to be restored.
            if (ServerState_GL_FRAMEBUFFER_BINDING != this->framebufferState->framebufferObject)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, ServerState_GL_FRAMEBUFFER_BINDING);
            }
        }
    }
}
