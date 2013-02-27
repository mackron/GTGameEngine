// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteTexture.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDeleteTexture::RCDeleteTexture()
        : textureObject(nullptr)
    {
    }

    void RCDeleteTexture::DeleteTexture(GLuint* textureObjectIn)
    {
        this->textureObject = textureObjectIn;
    }

    void RCDeleteTexture::Execute()
    {
        glDeleteTextures(1, this->textureObject);
    }
}
