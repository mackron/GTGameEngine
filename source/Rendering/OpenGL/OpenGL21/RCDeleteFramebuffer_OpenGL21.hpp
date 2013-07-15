// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL21_RCDeleteFramebuffer_hpp_
#define __GTEngine_Rendering_OpenGL21_RCDeleteFramebuffer_hpp_

#include "../../RenderCommand.hpp"
#include "FramebufferState_OpenGL21.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCDeleteFramebuffer : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteFramebuffer();


        /// Renderer::DeleteFramebuffer()
        void DeleteFramebuffer(FramebufferState_OpenGL21* framebufferStateIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL shader program object.
        FramebufferState_OpenGL21* framebufferState;



    private:    // No copying.
        RCDeleteFramebuffer(const RCDeleteFramebuffer &);
        RCDeleteFramebuffer & operator=(const RCDeleteFramebuffer &);
    };
}

#endif
