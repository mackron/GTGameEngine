// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Framebuffer_OpenGL33_hpp_
#define __GTEngine_Rendering_Framebuffer_OpenGL33_hpp_

#include <GTEngine/Rendering/Framebuffer.hpp>
#include "FramebufferState_OpenGL33.hpp"

namespace GTEngine
{
    class Framebuffer_OpenGL33 : public Framebuffer
    {
    public:

        /// Constructor.
        Framebuffer_OpenGL33(FramebufferState_OpenGL33* stateGLIn);

        /// Destructor.
        ~Framebuffer_OpenGL33();

        
        /// Retrieves a pointer to the OpenGL state.
        FramebufferState_OpenGL33* GetOpenGLState() const;




    private:

        /// A pointer to the OpenGL framebuffer object.
        FramebufferState_OpenGL33* stateGL;
        
        
        
    private:    // No copying.
        Framebuffer_OpenGL33(const Framebuffer_OpenGL33 &);
        Framebuffer_OpenGL33 & operator=(const Framebuffer_OpenGL33 &);
    };
}

#endif
