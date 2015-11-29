// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "TextureCube_OpenGL21.hpp"

namespace GT
{
    TextureCube_OpenGL21::TextureCube_OpenGL21(TextureState_OpenGL21* stateGLIn, Texture2D* positiveX, Texture2D* negativeX, Texture2D* positiveY, Texture2D* negativeY, Texture2D* positiveZ, Texture2D* negativeZ)
        : TextureCube(positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ), stateGL(stateGLIn)
    {
    }

    TextureCube_OpenGL21::~TextureCube_OpenGL21()
    {
    }


    TextureState_OpenGL21* TextureCube_OpenGL21::GetOpenGLState() const
    {
        return this->stateGL;
    }
}