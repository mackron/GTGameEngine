// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDraw.hpp"
#include "ServerState_OpenGL33.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    #define DRAW_TYPE_NONE      0
    #define DRAW_TYPE_VA        1
    #define DRAW_TYPE_RAW       2


    RCDraw::RCDraw()
        : drawType(DRAW_TYPE_NONE), drawVA(), drawRaw()
    {
        this->drawRaw.vertices = nullptr;
        this->drawRaw.indices  = nullptr;
    }

    RCDraw::~RCDraw()
    {
        free(this->drawRaw.vertices);
        free(this->drawRaw.indices);
    }

    void RCDraw::Draw(GLuint* vertexArrayObjectIn, GLuint* vertexBufferObjectIn, GLenum drawModeIn, GLsizei indexCountIn)
    {
        this->drawVA.vertexArrayObject  = vertexArrayObjectIn;
        this->drawVA.vertexBufferObject = vertexBufferObjectIn;
        this->drawVA.drawMode           = drawModeIn;
        this->drawVA.indexCount         = indexCountIn;

        this->drawType = DRAW_TYPE_VA;
    }

    void RCDraw::Draw(const float* vertices, GLsizei vertexCount, const unsigned int* indices, GLsizei indexCount, const VertexFormat &format, GLenum drawMode)
    {
        size_t verticesBufferSize = vertexCount * format.GetSizeInBytes();
        size_t indicesBufferSize  = indexCount * sizeof(unsigned int);

        this->drawRaw.vertices = static_cast<float*>(malloc(verticesBufferSize));
        memcpy(this->drawRaw.vertices, vertices, verticesBufferSize);

        this->drawRaw.indices = static_cast<unsigned int*>(malloc(indicesBufferSize));
        memcpy(this->drawRaw.indices, indices, indicesBufferSize);

        this->drawRaw.vertexBufferSize = verticesBufferSize;
        this->drawRaw.indexCount       = indexCount;
        this->drawRaw.format           = format;
        this->drawRaw.drawMode         = drawMode;


        this->drawType = DRAW_TYPE_RAW;
    }

    void RCDraw::Execute()
    {
        // NOTE: It's OK to break global state with this call. It's considered and handled by the renderer.

        if (this->drawType == DRAW_TYPE_VA)
        {
            // 1) Bind the vertex array.
            glBindVertexArray(*this->drawVA.vertexArrayObject);
            glBindBuffer(GL_ARRAY_BUFFER, *this->drawVA.vertexBufferObject);


            // 2) Draw.
            glDrawElements(this->drawVA.drawMode, this->drawVA.indexCount, GL_UNSIGNED_INT, 0);


            // 3) Set state.
            ServerState_GL_VERTEX_ARRAY_BINDING = *this->drawVA.vertexArrayObject;
            ServerState_GL_ARRAY_BUFFER_BINDING = *this->drawVA.vertexBufferObject;
        }
        else if (this->drawType == DRAW_TYPE_RAW)
        {
            // Bind the vertex array and vertex buffer.
            glBindVertexArray(ServerState_DefaultVAO);
            glBindBuffer(GL_ARRAY_BUFFER, ServerState_DefaultVBO);

            // Set the data.
            glBufferData(GL_ARRAY_BUFFER,         this->drawRaw.vertexBufferSize,                    this->drawRaw.vertices, GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->drawRaw.indexCount  * sizeof(unsigned int),  this->drawRaw.indices,  GL_STATIC_DRAW);
            


            // Set the vertex state.
            uint32_t  newVertexAttribEnableBits = 0x0;
            uint32_t &oldVertexAttribEnableBits = ServerState_EnabledVertexAttributes;
            size_t    formatArraySize           = this->drawRaw.format.GetAttributeCount();

            GLsizei formatSizeInBytes = static_cast<GLsizei>(this->drawRaw.format.GetSizeInBytes());
            int offset = 0;

            for (size_t i = 0; i < formatArraySize; i += 2)
            {
                GLuint   attribIndex = static_cast<GLuint>(this->drawRaw.format[i]);
                GLint    attribSize  = static_cast<GLint>(this->drawRaw.format[i + 1]);
                uint32_t bit         = static_cast<uint32_t>(1 << attribIndex);

                newVertexAttribEnableBits |= bit;

                if (!(oldVertexAttribEnableBits & bit))
                {
                    glEnableVertexAttribArray(attribIndex);
                }
                else
                {
                    // We clear the bit from the old bitfield because after this loop we're going to check if any bits are remaining. If so,
                    // those attributes need to be disabled. By clearing, we can just check if the bitfield is 0, in which case nothing else
                    // needs disabling.
                    oldVertexAttribEnableBits &= ~bit;
                }

                glVertexAttribPointer(attribIndex, attribSize, GL_FLOAT, GL_FALSE, formatSizeInBytes, reinterpret_cast<const GLvoid*>(offset));

                // The offset must be set AFTER glVertexAttribPointer().
                offset += attribSize * sizeof(float);
            }

            // If any attributes need to be disable, we need to do that now. This is where our enabled bitfields come in handy.
            while (oldVertexAttribEnableBits != 0)
            {
                GLuint attribIndex  = static_cast<GLuint>(GTCore::NextBitIndex(oldVertexAttribEnableBits));

                glDisableVertexAttribArray(attribIndex);

                // The bit needs to be cleared in preperation for the next iteration.
                oldVertexAttribEnableBits &= ~(1 << attribIndex);
            }

            // Now we need to set the renderer's enabled bits to our new bits.
            ServerState_EnabledVertexAttributes = newVertexAttribEnableBits;



            // Draw.
            glDrawElements(this->drawRaw.drawMode, this->drawRaw.indexCount, GL_UNSIGNED_INT, 0);



            // Set State.
            ServerState_GL_VERTEX_ARRAY_BINDING = ServerState_DefaultVAO;
            ServerState_GL_ARRAY_BUFFER_BINDING = ServerState_DefaultVBO;
        }
    }
}
