// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Framebuffer_OpenGL33_hpp_
#define __GTEngine_Rendering_Framebuffer_OpenGL33_hpp_

#include <GTEngine/Rendering/Framebuffer.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    class Framebuffer_OpenGL33 : public Framebuffer
    {
    public:

        /// Constructor.
        Framebuffer_OpenGL33(GLuint* objectGLIn);

        /// Destructor.
        ~Framebuffer_OpenGL33();

        
        /// Retrieves a pointer to the OpenGL object.
        GLuint* GetOpenGLObjectPtr();




    private:

        /// A pointer to the OpenGL framebuffer object.
        GLuint* objectGL;
    };
}

#endif