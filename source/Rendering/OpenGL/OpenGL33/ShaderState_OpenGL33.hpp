// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_ShaderState_OpenGL33_hpp_
#define __GTEngine_Rendering_ShaderState_OpenGL33_hpp_

#include <GTCore/Dictionary.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Structure containing the state of a shader for use by the OpenGL33 renderer.
    ///
    /// The main reason for this is that we need to somehow manage the activation of texture units as shaders are made current. All comes
    /// down to OpenGL's insane way of handling the passing of textures to shaders.
    ///
    /// We don't keep track of every piece of state here. We only store the state that we need for our own interface.
    struct ShaderState_OpenGL33
    {
        /// The main OpenGL program object.
        GLuint programObject;


        /// Structure containing information about a texture being used by the shader.
        struct TextureParameter
        {
            /// A pointer to the relevant texture object.
            GLuint* textureObject;

            /// The texture target. Needed for binding.
            GLenum textureTarget;

            /// The texture unit to bind the texture on.
            GLint textureUnit;
        };

        /// The list of texture parameters currently set on the shader. Keyed by the name of the texture.
        GTCore::Dictionary<TextureParameter> textures;


        
        /// Constructor.
        ShaderState_OpenGL33()
            : programObject(0), textures()
        {
        }
    };
}

#endif