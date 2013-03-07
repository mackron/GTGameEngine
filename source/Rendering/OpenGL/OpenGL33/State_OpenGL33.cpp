// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "State_OpenGL33.hpp"
#include <new>

namespace GTEngine
{
    State_OpenGL33::State_OpenGL33()
        : currentProgramState(nullptr), currentFramebufferState(nullptr),
          enabledStates(COLOUR_WRITES_BIT | DEPTH_WRITES_BIT | DEPTH_TEST_BIT | FACE_CULLING_BIT),
          stencilMask(-1),
          currentRCSetGlobalState(nullptr), currentRCSetVertexArrayState(nullptr), currentRCSetTextureState(nullptr), currentRCSetShaderState(nullptr), currentRCSetFramebufferState(nullptr),
          currentRCClear(nullptr), currentRCDraw(nullptr),
          instantiatedTextureObjects(), instantiatedProgramObjects(), instantiatedVertexArrayObjects(), instantiatedBufferObjects(), instantiatedFramebufferObjects(),
          deletedTextureObjects(),      deletedProgramObjects(),      deletedVertexArrayObjects(),      deletedBufferObjects(),      deletedFramebufferObjects(),
          swapInterval(1), swapIntervalNeedsUpdate(false),
          backIndex(0)
    {
    }

    State_OpenGL33::~State_OpenGL33()
    {
        // We call this twice just to make sure both the back and front buffers are cleared correctly.
        this->ClearDeletedOpenGLObjects();
        this->ClearDeletedOpenGLObjects();
    }


    void State_OpenGL33::MarkTextureObjectAsDeleted(GLuint* textureObject)
    {
        assert(this->instantiatedTextureObjects.Exists(textureObject));
        {
            this->instantiatedTextureObjects.RemoveFirstOccuranceOf(textureObject);
            this->deletedTextureObjects[this->backIndex].PushBack(textureObject);
        }
    }

    void State_OpenGL33::MarkProgramObjectAsDeleted(ShaderState_OpenGL33* programObject)
    {
        assert(this->instantiatedProgramObjects.Exists(programObject));
        {
            this->instantiatedProgramObjects.RemoveFirstOccuranceOf(programObject);
            this->deletedProgramObjects[this->backIndex].PushBack(programObject);
        }
    }

    void State_OpenGL33::MarkVertexArrayObjectAsDeleted(GLuint* vertexArrayObject)
    {
        assert(this->instantiatedVertexArrayObjects.Exists(vertexArrayObject));
        {
            this->instantiatedVertexArrayObjects.RemoveFirstOccuranceOf(vertexArrayObject);
            this->deletedVertexArrayObjects[this->backIndex].PushBack(vertexArrayObject);
        }
    }

    void State_OpenGL33::MarkBufferObjectAsDeleted(GLuint* bufferObject)
    {
        assert(this->instantiatedBufferObjects.Exists(bufferObject));
        {
            this->instantiatedBufferObjects.RemoveFirstOccuranceOf(bufferObject);
            this->deletedBufferObjects[this->backIndex].PushBack(bufferObject);
        }
    }

    void State_OpenGL33::MarkFramebufferObjectAsDeleted(FramebufferState_OpenGL33* framebufferObject)
    {
        assert(this->instantiatedFramebufferObjects.Exists(framebufferObject));
        {
            this->instantiatedFramebufferObjects.RemoveFirstOccuranceOf(framebufferObject);
            this->deletedFramebufferObjects[this->backIndex].PushBack(framebufferObject);
        }
    }


    void State_OpenGL33::ClearDeletedOpenGLObjects()
    {
        // Textures.
        for (size_t i = 0; i < this->deletedTextureObjects[!this->backIndex].count; ++i)
        {
            delete this->deletedTextureObjects[!this->backIndex][i];
        }
        this->deletedTextureObjects[!this->backIndex].Clear();



        // Programs.
        for (size_t i = 0; i < this->deletedProgramObjects[!this->backIndex].count; ++i)
        {
            // If the program is the current one, we'll just set it to null.
            if (this->deletedProgramObjects[!this->backIndex][i] == this->currentProgramState)
            {
                this->currentProgramState = nullptr;
            }

            delete this->deletedProgramObjects[!this->backIndex][i];
        }
        this->deletedProgramObjects[!this->backIndex].Clear();



        // Vertex arrays.
        for (size_t i = 0; i < this->deletedVertexArrayObjects[!this->backIndex].count; ++i)
        {
            delete this->deletedVertexArrayObjects[!this->backIndex][i];
        }
        this->deletedVertexArrayObjects[!this->backIndex].Clear();



        // Buffers.
        for (size_t i = 0; i < this->deletedBufferObjects[!this->backIndex].count; ++i)
        {
            delete this->deletedBufferObjects[!this->backIndex][i];
        }
        this->deletedBufferObjects[!this->backIndex].Clear();



        // Framebuffers.
        for (size_t i = 0; i < this->deletedFramebufferObjects[!this->backIndex].count; ++i)
        {
            // If the framebuffer is the current one, we'll just set it to null.
            if (this->deletedFramebufferObjects[!this->backIndex][i] == this->currentFramebufferState)
            {
                this->currentFramebufferState = nullptr;
            }

            delete this->deletedFramebufferObjects[!this->backIndex][i];
        }
        this->deletedFramebufferObjects[!this->backIndex].Clear();



        // We need to swap the back index now.
        this->backIndex = !this->backIndex;
    }
}