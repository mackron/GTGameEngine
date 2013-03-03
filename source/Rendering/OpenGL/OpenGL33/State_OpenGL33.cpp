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
          deletedTextureObjects(),      deletedProgramObjects(),      deletedVertexArrayObjects(),      deletedBufferObjects(),      deletedFramebufferObjects()
    {
    }

    State_OpenGL33::~State_OpenGL33()
    {
        this->ClearDeletedOpenGLObjects();
    }


    void State_OpenGL33::MarkTextureObjectAsDeleted(GLuint* textureObject)
    {
        assert(this->instantiatedTextureObjects.Exists(textureObject));
        {
            this->instantiatedTextureObjects.RemoveFirstOccuranceOf(textureObject);
            this->deletedTextureObjects.PushBack(textureObject);
        }
    }

    void State_OpenGL33::MarkProgramObjectAsDeleted(ShaderState_OpenGL33* programObject)
    {
        assert(this->instantiatedProgramObjects.Exists(programObject));
        {
            this->instantiatedProgramObjects.RemoveFirstOccuranceOf(programObject);
            this->deletedProgramObjects.PushBack(programObject);
        }
    }

    void State_OpenGL33::MarkVertexArrayObjectAsDeleted(GLuint* vertexArrayObject)
    {
        assert(this->instantiatedVertexArrayObjects.Exists(vertexArrayObject));
        {
            this->instantiatedVertexArrayObjects.RemoveFirstOccuranceOf(vertexArrayObject);
            this->deletedVertexArrayObjects.PushBack(vertexArrayObject);
        }
    }

    void State_OpenGL33::MarkBufferObjectAsDeleted(GLuint* bufferObject)
    {
        assert(this->instantiatedBufferObjects.Exists(bufferObject));
        {
            this->instantiatedBufferObjects.RemoveFirstOccuranceOf(bufferObject);
            this->deletedBufferObjects.PushBack(bufferObject);
        }
    }

    void State_OpenGL33::MarkFramebufferObjectAsDeleted(FramebufferState_OpenGL33* framebufferObject)
    {
        assert(this->instantiatedFramebufferObjects.Exists(framebufferObject));
        {
            this->instantiatedFramebufferObjects.RemoveFirstOccuranceOf(framebufferObject);
            this->deletedFramebufferObjects.PushBack(framebufferObject);
        }
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
            // If the program is the current one, we'll just set it to null.
            if (this->deletedProgramObjects[i] == this->currentProgramState)
            {
                this->currentProgramState = nullptr;
            }

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
            // If the framebuffer is the current one, we'll just set it to null.
            if (this->deletedFramebufferObjects[i] == this->currentFramebufferState)
            {
                this->currentFramebufferState = nullptr;
            }

            delete this->deletedFramebufferObjects[i];
        }
        this->deletedFramebufferObjects.Clear();
    }
}