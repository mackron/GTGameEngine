// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/RenderCommands/RCSetFramebuffer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    RCSetFramebuffer::RCSetFramebuffer()
        : framebuffer(nullptr)
    {
    }
    
    RCSetFramebuffer::RCSetFramebuffer(Framebuffer* framebuffer)
        : framebuffer(framebuffer)
    {
    }

    void RCSetFramebuffer::Execute()
    {
        Renderer::SetFramebuffer(this->framebuffer);
    }
}
