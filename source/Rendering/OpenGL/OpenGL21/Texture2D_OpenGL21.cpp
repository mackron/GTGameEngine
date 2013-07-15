// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Texture2D_OpenGL21.hpp"

namespace GTEngine
{
    Texture2D_OpenGL21::Texture2D_OpenGL21(TextureState_OpenGL21* stateGLIn, GLenum targetIn)
        : stateGL(stateGLIn), target(targetIn)
    {
    }

    Texture2D_OpenGL21::~Texture2D_OpenGL21()
    {
    }


    TextureState_OpenGL21* Texture2D_OpenGL21::GetOpenGLState() const
    {
        return this->stateGL;
    }

    GLenum Texture2D_OpenGL21::GetTarget() const
    {
        return this->target;
    }
}