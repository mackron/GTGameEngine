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
          currentRCClear(nullptr), currentRCSetGlobalState(nullptr),
          currentTextureObjects(), currentProgramObjects(), currentVertexArrayObjects(), currentBufferObjects(), currentFramebufferObjects(),
          deletedTextureObjects(), deletedProgramObjects(), deletedVertexArrayObjects(), deletedBufferObjects(), deletedFramebufferObjects()
    {
    }

    State_OpenGL33::~State_OpenGL33()
    {
        this->ClearDeletedOpenGLObjects();
    }


    void State_OpenGL33::ClearDeletedOpenGLObjects()
    {
        for (size_t i = 0; i < this->deletedTextureObjects.count; ++i)
        {
            delete this->deletedTextureObjects[i];
        }
        this->deletedTextureObjects.Clear();

        for (size_t i = 0; i < this->deletedProgramObjects.count; ++i)
        {
            delete this->deletedProgramObjects[i];
        }
        this->deletedProgramObjects.Clear();

        for (size_t i = 0; i < this->deletedVertexArrayObjects.count; ++i)
        {
            delete this->deletedVertexArrayObjects[i];
        }
        this->deletedVertexArrayObjects.Clear();

        for (size_t i = 0; i < this->deletedBufferObjects.count; ++i)
        {
            delete this->deletedBufferObjects[i];
        }
        this->deletedBufferObjects.Clear();

        for (size_t i = 0; i < this->deletedFramebufferObjects.count; ++i)
        {
            delete this->deletedFramebufferObjects[i];
        }
        this->deletedFramebufferObjects.Clear();
    }
}