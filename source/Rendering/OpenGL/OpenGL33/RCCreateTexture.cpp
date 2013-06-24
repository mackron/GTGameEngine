// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateTexture.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCCreateTexture::RCCreateTexture()
        : textureState(nullptr)
    {
    }

    void RCCreateTexture::CreateTexture(TextureState_OpenGL33* textureStateIn)
    {
        this->textureState = textureStateIn;
    }

    void RCCreateTexture::Execute()
    {
        glGenTextures(1, &this->textureState->objectGL);
    }
}