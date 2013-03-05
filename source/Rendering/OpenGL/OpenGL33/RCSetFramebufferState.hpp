// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCSetFramebufferState_hpp_
#define __GTEngine_Rendering_OpenGL33_RCSetFramebufferState_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include "FramebufferState_OpenGL33.hpp"
#include <GTCore/Map.hpp>
#include <cstdint>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetFramebufferState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetFramebufferState();


        /// Marks an attachment as needing to be set.
        ///
        /// @param framebufferState [in] A pointer to the state of the framebuffer whose state is being set.
        /// @param attachmentPoint  [in] The attachment point to attach the buffer at.
        /// @param textureTarget    [in] The texture target.
        /// @param textureObject    [in] A pointer to the texture object to attach. Should not be null.
        ///
        /// @remarks
        ///     This should be called for every currently attached buffer. Old buffers that are not longer used will be automatically detached.
        void SetAttachedBuffer(FramebufferState_OpenGL33* framebufferState, GLenum attachmentPoint, GLenum textureTarget, GLuint* textureObject);


        /// Retrieves the program object whose state is being set.
        FramebufferState_OpenGL33* GetFramebufferState();



        /// RenderCommand::Execute()
        void Execute();



    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// A pointer to the OpenGL framebuffer object.
        FramebufferState_OpenGL33* framebufferState;


        /// The list of attachments. We use a map here just to make it easier to overwrite existing attachments. Keyed by the attachment point.
        GTCore::Map<GLenum, FramebufferState_OpenGL33::Attachment> attachments;



    private:    // No copying.
        RCSetFramebufferState(const RCSetFramebufferState &);
        RCSetFramebufferState & operator=(const RCSetFramebufferState &);
    };
}

#endif
