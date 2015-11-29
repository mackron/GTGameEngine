// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_OpenGL21_RCDeleteFramebuffer
#define GT_Rendering_OpenGL21_RCDeleteFramebuffer

#include "../../RenderCommand.hpp"
#include "FramebufferState_OpenGL21.hpp"

namespace GT
{
    /// Render command for clearing the buffers/
    class RCDeleteFramebuffer : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteFramebuffer();

        /// Destructor.
        virtual ~RCDeleteFramebuffer();


        /// Renderer::DeleteFramebuffer()
        void DeleteFramebuffer(FramebufferState_OpenGL21* framebufferStateIn);


        /// RenderCommand::Execute()
        void Execute();


    protected:

        /// glDeleteFramebuffers.
        virtual void __glDeleteFramebuffers(GLsizei n, const GLuint *renderbuffers) = 0;

        /// glDeleteRenderbuffers.
        virtual void __glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) = 0;


    private:

        /// A pointer to the OpenGL shader program object.
        FramebufferState_OpenGL21* framebufferState;



    private:    // No copying.
        RCDeleteFramebuffer(const RCDeleteFramebuffer &);
        RCDeleteFramebuffer & operator=(const RCDeleteFramebuffer &);
    };



    /// EXT_framebuffer_object version of RCDeleteFramebuffer.
    class RCDeleteFramebufferEXT : public RCDeleteFramebuffer
    {
    public:

        /// Constructor.
        RCDeleteFramebufferEXT()
            : RCDeleteFramebuffer()
        {
        }


    private:

        /// glDeleteFramebuffers.
        void __glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);

        /// glDeleteRenderbuffers.
        void __glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
    };


    /// ARB_framebuffer_object version of RCDeleteFramebuffer.
    class RCDeleteFramebufferARB : public RCDeleteFramebuffer
    {
    public:

        /// Constructor.
        RCDeleteFramebufferARB()
            : RCDeleteFramebuffer()
        {
        }


    private:

        /// glDeleteFramebuffers.
        void __glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);

        /// glDeleteRenderbuffers.
        void __glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
    };
}

#endif
