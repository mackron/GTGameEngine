// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Framebuffer_OpenGL21_hpp_
#define __GTEngine_Rendering_Framebuffer_OpenGL21_hpp_

#include <GTEngine/Rendering/Framebuffer.hpp>
#include "FramebufferState_OpenGL21.hpp"

namespace GTEngine
{
    class Framebuffer_OpenGL21 : public Framebuffer
    {
    public:

        /// Constructor.
        Framebuffer_OpenGL21(FramebufferState_OpenGL21* stateGLIn);

        /// Destructor.
        ~Framebuffer_OpenGL21();

        
        /// Retrieves a pointer to the OpenGL state.
        FramebufferState_OpenGL21* GetOpenGLState() const;




    private:

        /// A pointer to the OpenGL framebuffer object.
        FramebufferState_OpenGL21* stateGL;
        
        
        
    private:    // No copying.
        Framebuffer_OpenGL21(const Framebuffer_OpenGL21 &);
        Framebuffer_OpenGL21 & operator=(const Framebuffer_OpenGL21 &);
    };
}

#endif
