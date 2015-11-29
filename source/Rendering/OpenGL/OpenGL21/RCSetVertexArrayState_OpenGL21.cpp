// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCSetVertexArrayState_OpenGL21.hpp"
#include "ServerState_OpenGL21.hpp"
#include <gtgl/gtgl.h>
#include <assert.h>

namespace GT
{
    #define VERTEX_DATA_BIT         (1 << 1)
    #define INDEX_DATA_BIT          (1 << 2)


    RCSetVertexArrayState::RCSetVertexArrayState()
        : operationBitfield(0),
          vertexBufferObject(nullptr), indexBufferObject(nullptr),
          vertexData(), indexData()
    {
    }

    void RCSetVertexArrayState::SetVertexData(GLuint* vertexBufferObjectIn, const float* vertices, size_t vertexCount, size_t vertexSizeInBytes, GLenum usage)
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


        this->vertexBufferObject = vertexBufferObjectIn;
        this->operationBitfield |= VERTEX_DATA_BIT;
    }

    void RCSetVertexArrayState::SetIndexData(GLuint* indexBufferObjectIn, const unsigned int* indices, size_t indexCount, GLenum usage)
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


        this->indexBufferObject  = indexBufferObjectIn;
        this->operationBitfield |= INDEX_DATA_BIT;
    }


    void RCSetVertexArrayState::Execute()
    {
        if ((this->operationBitfield & VERTEX_DATA_BIT))
        {
            if (ServerState_GL_ARRAY_BUFFER_BINDING != *this->vertexBufferObject)
            {
                glBindBuffer(GL_ARRAY_BUFFER, *this->vertexBufferObject);
            }

            glBufferData(GL_ARRAY_BUFFER, this->vertexData.count * this->vertexData.vertexSizeInBytes, this->vertexData.vertices, this->vertexData.usage);
            free(this->vertexData.vertices);

            if (ServerState_GL_ARRAY_BUFFER_BINDING != *this->vertexBufferObject)
            {
                glBindBuffer(GL_ARRAY_BUFFER, ServerState_GL_ARRAY_BUFFER_BINDING);
            }
        }

        if ((this->operationBitfield & INDEX_DATA_BIT))
        {
            if (ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING != *this->indexBufferObject)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->indexBufferObject);
            }

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexData.count * sizeof(unsigned int), this->indexData.indices, this->indexData.usage);
            free(this->indexData.indices);

            if (ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING != *this->indexBufferObject)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING);
            }
        }
    }
}
