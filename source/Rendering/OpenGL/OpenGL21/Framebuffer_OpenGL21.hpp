// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_Framebuffer_OpenGL21
#define GT_Rendering_Framebuffer_OpenGL21

#include <GTGE/Rendering/Framebuffer.hpp>
#include "FramebufferState_OpenGL21.hpp"

namespace GT
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
