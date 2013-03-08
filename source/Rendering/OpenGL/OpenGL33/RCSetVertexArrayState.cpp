// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetVertexArrayState.hpp"
#include <gtgl/gtgl.h>
#include <assert.h>

namespace GTEngine
{
    #define VERTEX_DATA_BIT         (1 << 1)
    #define INDEX_DATA_BIT          (1 << 2)


    RCSetVertexArrayState::RCSetVertexArrayState()
        : operationBitfield(0),
          vertexArrayObject(nullptr), vertexBufferObject(nullptr),
          vertexData(), indexData()
    {
    }

    void RCSetVertexArrayState::SetVertexData(GLuint* vertexArrayObjectIn, GLuint* vertexBufferObjectIn, const float* vertices, size_t vertexCount, size_t vertexSizeInBytes, GLenum usage)
    {
        assert(this->vertexArrayObject == nullptr || this->vertexArrayObject == vertexArrayObjectIn);
        {
            free(this->vertexData.vertices);

            if (vertices != nullptr)
            {
                this->vertexData.vertices = static_cast<float*>(malloc(vertexCount * vertexSizeInBytes));
                memcpy(this->vertexData.vertices, vertices, vertexCount * vertexSizeInBytes);
            }
            else
            {
                this->vertexData.vertices = nullptr;
            }

            this->vertexData.count             = vertexCount;
            this->vertexData.vertexSizeInBytes = vertexSizeInBytes;
            this->vertexData.usage             = usage;


            this->vertexArrayObject  = vertexArrayObjectIn;
            this->vertexBufferObject = vertexBufferObjectIn;
            this->operationBitfield |= VERTEX_DATA_BIT;
        }
    }

    void RCSetVertexArrayState::SetIndexData(GLuint* vertexArrayObjectIn, const unsigned int* indices, size_t indexCount, GLenum usage)
    {
        assert(this->vertexArrayObject == nullptr || this->vertexArrayObject == vertexArrayObjectIn);
        {
            free(this->indexData.indices);

            if (indices != nullptr)
            {
                this->indexData.indices = static_cast<unsigned int*>(malloc(indexCount * sizeof(unsigned int)));
                memcpy(this->indexData.indices, indices, indexCount * sizeof(unsigned int));
            }
            else
            {
                this->indexData.indices = nullptr;
            }

            this->indexData.count = indexCount;
            this->indexData.usage = usage;


            this->vertexArrayObject  = vertexArrayObjectIn;
            this->operationBitfield |= INDEX_DATA_BIT;
        }
    }


    GLuint* RCSetVertexArrayState::GetVertexArrayObject()
    {
        return this->vertexArrayObject;
    }


    void RCSetVertexArrayState::Execute()
    {
        assert(this->vertexArrayObject != nullptr);
        {
            // We need to maintain the integrity of the global state, so we'll have to grab the previous vertex array binding.
            GLint previousCurrentVAO;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousCurrentVAO);

            if (static_cast<GLuint>(previousCurrentVAO) != *this->vertexArrayObject)
            {
                glBindVertexArray(*this->vertexArrayObject);
            }


            if ((this->operationBitfield & VERTEX_DATA_BIT))
            {
                // Unfortunately GL_ARRAY_BUFFER is not stored in the VAO state so we need to bind it here. As usual, the global state needs to remain in tact.
                GLint previousCurrentVertexBuffer;
                glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousCurrentVertexBuffer);

                if (static_cast<GLuint>(previousCurrentVertexBuffer) != *this->vertexBufferObject)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, *this->vertexBufferObject);
                }


                glBufferData(GL_ARRAY_BUFFER, this->vertexData.count * this->vertexData.vertexSizeInBytes, this->vertexData.vertices, this->vertexData.usage);
                free(this->vertexData.vertices);


                if (static_cast<GLuint>(previousCurrentVertexBuffer) != *this->vertexBufferObject)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(previousCurrentVertexBuffer));
                }
            }

            if ((this->operationBitfield & INDEX_DATA_BIT))
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexData.count * sizeof(unsigned int), this->indexData.indices, this->indexData.usage);
                free(this->indexData.indices);
            }


            // Global state needs to be restored.
            if (static_cast<GLuint>(previousCurrentVAO) != *this->vertexArrayObject)
            {
                glBindVertexArray(static_cast<GLuint>(previousCurrentVAO));
            }
        }
    }
}
