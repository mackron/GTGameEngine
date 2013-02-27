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
        Texture2D_OpenGL33(GLuint* objectGLIn);

        /// Destructor.
        ~Texture2D_OpenGL33();


        /// Retrieves a pointer to the OpenGL object.
        GLuint* GetOpenGLObjectPtr() const;


    private:

        /// A pointer to the OpenGL object.
        GLuint* objectGL;
    };
}

#endif