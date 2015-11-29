// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_OpenGL21_RCSetFramebufferState_hpp_
#define __GTEngine_Rendering_OpenGL21_RCSetFramebufferState_hpp_

#include "../../RenderCommand.hpp"
#include "FramebufferState_OpenGL21.hpp"
#include <GTEngine/Core/Map.hpp>
#include <cstdint>

namespace GT
{
    /// Render command for clearing the buffers/
    class RCSetFramebufferState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetFramebufferState();

        /// Destructor.
        virtual ~RCSetFramebufferState();


        /// Marks an attachment as needing to be set.
        ///
        /// @param framebufferState [in] A pointer to the state of the framebuffer whose state is being set.
        /// @param attachmentPoint  [in] The attachment point to attach the buffer at.
        /// @param textureTarget    [in] The texture target.
        /// @param textureState     [in] A pointer to the texture state to attach. Should not be null.
        ///
        /// @remarks
        ///     This should be called for every currently attached buffer. Old buffers that are not longer used will be automatically detached.
        void SetAttachedBuffer(FramebufferState_OpenGL21* framebufferState, GLenum attachmentPoint, GLenum textureTarget, TextureState_OpenGL21* textureObject);

        /// Marks the write-only depth/stencil as needing to be set.
        ///
        /// @param framebufferState [in] A pointer to the state ofthe framebuffer whose state is being set.
        /// @param attached         [in] Whether or not the write-only depth/stencil buffer should be attached or detached.
        /// @param width            [in] The width of the write-only depth/stencil buffer.
        /// @param height           [in] The height of the write-only depth/stencil buffer.
        void SetWriteOnlyDepthStencilBuffer(FramebufferState_OpenGL21* framebufferState, GLboolean attached, GLsizei width, GLsizei height);


        /// Retrieves the program object whose state is being set.
        FramebufferState_OpenGL21* GetFramebufferState();



        /// RenderCommand::Execute()
        void Execute();


    protected:

        /// glBindFramebuffer.
        virtual void __glBindFramebuffer(GLenum target, GLuint framebuffer) = 0;

        /// glFramebufferTexture2D.
        virtual void __glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = 0;

        /// glGenRenderbuffers.
        virtual void __glGenRenderbuffers(GLsizei n, GLuint *renderbuffers) = 0;

        /// glDeleteRenderbuffers.
        virtual void __glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) = 0;

        /// glBindRenderbuffer.
        virtual void __glBindRenderbuffer(GLenum target, GLuint renderbuffer) = 0;

        /// glRenderbufferStorage.
        virtual void __glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = 0;

        /// glFramebufferRenderbuffer.
        virtual void __glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = 0;




    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;

        /// A pointer to the OpenGL framebuffer object.
        FramebufferState_OpenGL21* framebufferState;


        /// The list of attachments. We use a map here just to make it easier to overwrite existing attachments. Keyed by the attachment point.
        Map<GLenum, FramebufferState_OpenGL21::Attachment> attachments;


        /// Whether or not the write-only depth/stencil buffer should be attached.
        GLboolean attachWriteOnlyDepthStencilBuffer;

        /// The width of the write-only depth/stencil buffer.
        GLsizei writeOnlyDepthStencilBufferWidth;

        /// The height of the write-only depth/stencil buffer.
        GLsizei writeOnlyDepthStencilBufferHeight;



    private:    // No copying.
        RCSetFramebufferState(const RCSetFramebufferState &);
        RCSetFramebufferState & operator=(const RCSetFramebufferState &);
    };



    /// The EXT_framebuffer_object version of RCSetFramebufferState.
    class RCSetFramebufferStateEXT : public RCSetFramebufferState
    {
    public:

        /// Constructor.
        RCSetFramebufferStateEXT()
            : RCSetFramebufferState()
        {
        }


    private:

        /// glBindFramebuffer.
        void __glBindFramebuffer(GLenum target, GLuint framebuffer);

        /// glFramebufferTexture2D.
        void __glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

        /// glGenRenderbuffers.
        void __glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);

        /// glDeleteRenderbuffers.
        void __glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);

        /// glBindRenderbuffer.
        void __glBindRenderbuffer(GLenum target, GLuint renderbuffer);

        /// glRenderbufferStorage.
        void __glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

        /// glFramebufferRenderbuffer.
        void __glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    };

    /// The ARB_framebuffer_object version of RCSetFramebufferState.
    class RCSetFramebufferStateARB : public RCSetFramebufferState
    {
    public:

        /// Constructor.
        RCSetFramebufferStateARB()
            : RCSetFramebufferState()
        {
        }


    private:

        /// glBindFramebuffer.
        void __glBindFramebuffer(GLenum target, GLuint framebuffer);

        /// glFramebufferTexture2D.
        void __glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

        /// glGenRenderbuffers.
        void __glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);

        /// glDeleteRenderbuffers.
        void __glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);

        /// glBindRenderbuffer.
        void __glBindRenderbuffer(GLenum target, GLuint renderbuffer);

        /// glRenderbufferStorage.
        void __glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

        /// glFramebufferRenderbuffer.
        void __glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    };
}

#endif
