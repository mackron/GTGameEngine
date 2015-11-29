// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCDeleteTexture_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GT
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
