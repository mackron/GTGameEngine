// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_TextureCube_OpenGL33_hpp_
#define __GTEngine_Rendering_TextureCube_OpenGL33_hpp_

#include <GTEngine/Rendering/TextureCube.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    class TextureCube_OpenGL33 : public TextureCube
    {
    public:

        /// Constructor.
        TextureCube_OpenGL33(GLuint* objectGLIn, Texture2D* positiveX, Texture2D* negativeX, Texture2D* positiveY, Texture2D* negativeY, Texture2D* positiveZ, Texture2D* negativeZ);

        /// Destructor.
        ~TextureCube_OpenGL33();


        /// Retrieves a pointer to the OpenGL object.
        GLuint* GetOpenGLObjectPtr() const;


    private:

        /// A pointer to the OpenGL object.
        GLuint* objectGL;
    };
}

#endif