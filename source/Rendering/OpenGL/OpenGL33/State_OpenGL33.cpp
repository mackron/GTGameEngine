// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "State_OpenGL33.hpp"
#include <new>

namespace GTEngine
{
    State_OpenGL33::State_OpenGL33()
        : currentTexture1D(nullptr), currentTexture2D(nullptr), currentTexture3D(nullptr), currentTextureCube(nullptr),
          currentProgram(nullptr),
          currentVAO(nullptr),
          currentFBO(nullptr),
          enabledStates(COLOUR_WRITES_BIT | DEPTH_WRITES_BIT | DEPTH_TEST_BIT | FACE_CULLING_BIT),
          stencilMask(-1),
          currentRCClear(nullptr), currentRCSetGlobalState(nullptr)
    {
    }

    State_OpenGL33::State_OpenGL33(const State_OpenGL33 &other)
        : currentTexture1D(other.currentTexture1D), currentTexture2D(other.currentTexture2D), currentTexture3D(other.currentTexture3D), currentTextureCube(other.currentTextureCube),
          currentProgram(other.currentProgram),
          currentVAO(other.currentVAO),
          currentFBO(other.currentFBO),
          enabledStates(other.enabledStates),
          stencilMask(other.stencilMask),
          currentRCClear(other.currentRCClear), currentRCSetGlobalState(nullptr)
    {
    }

    State_OpenGL33 & State_OpenGL33::operator=(const State_OpenGL33 &other)
    {
        new (this) State_OpenGL33(other);
        return *this;
    }
}