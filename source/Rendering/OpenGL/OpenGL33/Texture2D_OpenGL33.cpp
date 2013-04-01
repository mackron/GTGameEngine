// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Texture2D_OpenGL33.hpp"

namespace GTEngine
{
    Texture2D_OpenGL33::Texture2D_OpenGL33(TextureState_OpenGL33* stateGLIn, GLenum targetIn)
        : stateGL(stateGLIn), target(targetIn)
    {
    }

    Texture2D_OpenGL33::~Texture2D_OpenGL33()
    {
    }


    TextureState_OpenGL33* Texture2D_OpenGL33::GetOpenGLState() const
    {
        return this->stateGL;
    }

    GLenum Texture2D_OpenGL33::GetTarget() const
    {
        return this->target;
    }
}