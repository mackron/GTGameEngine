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
          vertexArrayObject(nullptr),
          vertexData(), indexData()
    {
    }

    void RCSetVertexArrayState::SetVertexData(GLuint* vertexArrayObjectIn, const float* vertices, size_t vertexCount, size_t vertexSizeInBytes, GLenum usage)
    {
        assert(this->vertexArrayObject == nullptr || this->vertexArrayObject != vertexArrayObjectIn);
        {
            free(this->vertexData.vertices);

            this->vertexData.vertices = static_cast<float*>(malloc(vertexCount * vertexSizeInBytes));
            memcpy(this->vertexData.vertices, vertices, vertexCount * vertexSizeInBytes);

            this->vertexData.count             = vertexCount;
            this->vertexData.vertexSizeInBytes = vertexSizeInBytes;
            this->vertexData.usage             = usage;


            this->vertexArrayObject  = vertexArrayObjectIn;
            this->operationBitfield |= VERTEX_DATA_BIT;
        }
    }

    void RCSetVertexArrayState::SetIndexData(GLuint* vertexArrayObjectIn, const unsigned int* indices, size_t indexCount, GLenum usage)
    {
        assert(this->vertexArrayObject == nullptr || this->vertexArrayObject != vertexArrayObjectIn);
        {
            free(this->indexData.indices);

            this->indexData.indices = static_cast<unsigned int*>(malloc(indexCount * sizeof(unsigned int)));
            memcpy(this->indexData.indices, indices, indexCount * sizeof(unsigned int));

            this->indexData.count = indexCount;
            this->indexData.usage = usage;


            this->vertexArrayObject  = vertexArrayObjectIn;
            this->operationBitfield |= INDEX_DATA_BIT;
        }
    }

    void RCSetVertexArrayState::Execute()
    {
        assert(this->vertexArrayObject != nullptr);
        {
            glBindVertexArray(*this->vertexArrayObject);

            if ((this->operationBitfield & VERTEX_DATA_BIT))
            {
                glBufferData(GL_ARRAY_BUFFER, this->vertexData.count * this->vertexData.vertexSizeInBytes, this->vertexData.vertices, this->vertexData.usage);
                free(this->vertexData.vertices);
            }

            if ((this->operationBitfield & INDEX_DATA_BIT))
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexData.count * sizeof(unsigned int), this->indexData.indices, this->indexData.usage);
                free(this->indexData.indices);
            }
        }
    }
}
