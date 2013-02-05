// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_RCSetFramebuffer_hpp_
#define __GTEngine_Rendering_RCSetFramebuffer_hpp_

#include "../RenderCommand.hpp"

namespace GTEngine
{
    class Framebuffer;

    /// A super simple render command that sets the current framebuffer.
    class RCSetFramebuffer : public RenderCommand
    {
    public:

        RCSetFramebuffer();
        RCSetFramebuffer(Framebuffer* framebuffer);

        void Execute();


    private:

        /// The framebuffer being set. Can be null.
        Framebuffer* framebuffer;
        
        
    private:    // No copying.
        RCSetFramebuffer(const RCSetFramebuffer &);
        RCSetFramebuffer & operator=(const RCSetFramebuffer &);
    };
}

#endif
