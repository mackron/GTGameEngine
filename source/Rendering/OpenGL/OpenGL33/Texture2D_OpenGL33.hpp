// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Texture2D_OpenGL33_hpp_
#define __GTEngine_Rendering_Texture2D_OpenGL33_hpp_

#include <GTEngine/Rendering/Texture2D.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    class Texture2D_OpenGL33 : public Texture2D
    {
    public:

        /// Constructor.
        Texture2D_OpenGL33(GLuint* objectGLIn, GLenum targetIn);

        /// Destructor.
        ~Texture2D_OpenGL33();


        /// Retrieves a pointer to the OpenGL object.
        GLuint* GetOpenGLObjectPtr() const;

        /// Retrieves the target.
        GLenum GetTarget() const;



    private:

        /// A pointer to the OpenGL object.
        GLuint* objectGL;

        /// The target.
        GLenum target;
    };
}

#endif