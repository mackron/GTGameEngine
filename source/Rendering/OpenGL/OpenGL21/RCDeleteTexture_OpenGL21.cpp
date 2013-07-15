// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteTexture_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDeleteTexture::RCDeleteTexture()
        : textureState(nullptr)
    {
    }

    void RCDeleteTexture::DeleteTexture(TextureState_OpenGL21* textureStateIn)
    {
        this->textureState = textureStateIn;
    }

    void RCDeleteTexture::Execute()
    {
        glDeleteTextures(1, &this->textureState->objectGL);
    }
}
