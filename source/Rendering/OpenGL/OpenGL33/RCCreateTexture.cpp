// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateTexture.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCCreateTexture::RCCreateTexture()
        : textureObject(nullptr)
    {
    }

    void RCCreateTexture::CreateTexture(GLuint* textureObjectIn)
    {
        this->textureObject = textureObjectIn;
    }

    void RCCreateTexture::Execute()
    {
        glGenTextures(1, this->textureObject);
    }
}
