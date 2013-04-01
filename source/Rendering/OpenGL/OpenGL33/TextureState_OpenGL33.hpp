// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_TextureState_OpenGL33_hpp_
#define __GTEngine_Rendering_TextureState_OpenGL33_hpp_

#include <GTCore/Vector.hpp>

namespace GTEngine
{
    struct ShaderState_OpenGL33;


    /// Structure containing the state of a texture object.
    struct TextureState_OpenGL33
    {
        /// The OpenGL object.
        GLuint objectGL;


        /// The list of shader states that belong to shaders that are currently references this texture as one it's uniform variables. We need to keep
        /// track of this so we can let the shader know when we are deleted so it can set it's uniform to a sane value.
        GTCore::Vector<ShaderState_OpenGL33*> shaders;
    };
}

#endif