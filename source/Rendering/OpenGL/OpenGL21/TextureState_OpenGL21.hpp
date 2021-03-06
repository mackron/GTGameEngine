// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_TextureState_OpenGL21
#define GT_Rendering_TextureState_OpenGL21

#include <GTGE/Core/Vector.hpp>
#include <gtgl/gtgl.h>


namespace GT
{
    struct ShaderState_OpenGL21;


    /// Structure containing the state of a texture object.
    struct TextureState_OpenGL21
    {
        /// The OpenGL object.
        GLuint objectGL;

        /// The list of shader states that belong to shaders that are currently references this texture as one it's uniform variables. We need to keep
        /// track of this so we can let the shader know when we are deleted so it can set it's uniform to a sane value.
        Vector<ShaderState_OpenGL21*> shaders;


        /// Constructor.
        TextureState_OpenGL21()
            : objectGL(0), shaders()
        {
        }

        /// Destructor.
        ~TextureState_OpenGL21();



    private:    // No copying.
        TextureState_OpenGL21(const TextureState_OpenGL21 &);
        TextureState_OpenGL21 & operator=(const TextureState_OpenGL21 &);
    };
}

#endif