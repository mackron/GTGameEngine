// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCCreateTexture_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GT
{
    RCCreateTexture::RCCreateTexture()
        : textureState(nullptr)
    {
    }

    void RCCreateTexture::CreateTexture(TextureState_OpenGL21* textureStateIn)
    {
        this->textureState = textureStateIn;
    }

    void RCCreateTexture::Execute()
    {
        glGenTextures(1, &this->textureState->objectGL);
    }
}
