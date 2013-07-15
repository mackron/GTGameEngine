// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "TextureState_OpenGL21.hpp"
#include "ShaderState_OpenGL21.hpp"

namespace GTEngine
{
    TextureState_OpenGL21::~TextureState_OpenGL21()
    {
        // If there are any shaders this texture is attached to, we need to make sure they are no longer marked as set.
        for (size_t iShader = 0; iShader < this->shaders.count; ++iShader)
        {
            auto shaderState = this->shaders[iShader];
            assert(shaderState != nullptr);
            {
                for (size_t iShaderTextureUniform = 0; iShaderTextureUniform < shaderState->currentTextureUniforms.count; )
                {
                    if (shaderState->currentTextureUniforms.buffer[iShaderTextureUniform]->value.textureState == this)
                    {
                        shaderState->currentTextureUniforms.RemoveByIndex(iShaderTextureUniform);
                    }
                    else
                    {
                        ++iShaderTextureUniform;
                    }
                }
            }
        }
    }
}