// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "State_OpenGL21.hpp"
#include <new>

namespace GT
{
    State_OpenGL21::State_OpenGL21()
        : currentProgramState(nullptr), currentFramebufferState(nullptr),
          enabledStates(COLOUR_WRITES_BIT | DEPTH_WRITES_BIT | DEPTH_TEST_BIT | FACE_CULLING_BIT),
          stencilMask(-1),
          currentRCSetGlobalState(nullptr), currentRCSetVertexArrayState(nullptr), currentRCSetTextureState(nullptr), currentRCSetShaderState(nullptr), currentRCSetFramebufferState(nullptr),
          currentRCClear(nullptr), currentRCDraw(nullptr),
          instantiatedTextureObjects(), instantiatedProgramObjects(), instantiatedBufferObjects(), instantiatedFramebufferObjects(),
          deletedTextureObjects(),      deletedProgramObjects(),      deletedBufferObjects(),      deletedFramebufferObjects(),
          swapInterval(1), swapIntervalNeedsUpdate(false),
          backIndex(0)
    {
    }

    State_OpenGL21::~State_OpenGL21()
    {
    }


    void State_OpenGL21::MarkTextureObjectAsDeleted(TextureState_OpenGL21* textureState)
    {
        assert(this->instantiatedTextureObjects.Exists(textureState));
        {
            this->instantiatedTextureObjects.RemoveFirstOccuranceOf(textureState);
            this->deletedTextureObjects[this->backIndex].PushBack(textureState);
        }
    }

    void State_OpenGL21::MarkProgramObjectAsDeleted(ShaderState_OpenGL21* programObject)
    {
        assert(this->instantiatedProgramObjects.Exists(programObject));
        {
            this->instantiatedProgramObjects.RemoveFirstOccuranceOf(programObject);
            this->deletedProgramObjects[this->backIndex].PushBack(programObject);
        }
    }

    void State_OpenGL21::MarkBufferObjectAsDeleted(GLuint* bufferObject)
    {
        assert(this->instantiatedBufferObjects.Exists(bufferObject));
        {
            this->instantiatedBufferObjects.RemoveFirstOccuranceOf(bufferObject);
            this->deletedBufferObjects[this->backIndex].PushBack(bufferObject);
        }
    }

    void State_OpenGL21::MarkFramebufferObjectAsDeleted(FramebufferState_OpenGL21* framebufferObject)
    {
        assert(this->instantiatedFramebufferObjects.Exists(framebufferObject));
        {
            this->instantiatedFramebufferObjects.RemoveFirstOccuranceOf(framebufferObject);
            this->deletedFramebufferObjects[this->backIndex].PushBack(framebufferObject);
        }
    }


    void State_OpenGL21::ClearDeletedOpenGLObjects()
    {
        // Textures.
        for (size_t i = 0; i < this->deletedTextureObjects[!this->backIndex].count; ++i)
        {
            auto textureStateToDelete = this->deletedTextureObjects[!this->backIndex][i];
            assert(textureStateToDelete != nullptr);
            {
                delete textureStateToDelete;
            }
        }
        this->deletedTextureObjects[!this->backIndex].Clear();



        // Programs.
        for (size_t i = 0; i < this->deletedProgramObjects[!this->backIndex].count; ++i)
        {
            auto programStateToDelete = this->deletedProgramObjects[!this->backIndex][i];
            assert(programStateToDelete != nullptr);
            {
                // If the program is the current one, we'll just set it to null.
                if (programStateToDelete == this->currentProgramState)
                {
                    this->currentProgramState = nullptr;
                }

                delete programStateToDelete;
            }
        }
        this->deletedProgramObjects[!this->backIndex].Clear();



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


    void State_OpenGL21::Shutdown()
    {
        // We call this twice just to make sure both the back and front buffers are cleared correctly.
        this->ClearDeletedOpenGLObjects();
        this->ClearDeletedOpenGLObjects();
    }
}