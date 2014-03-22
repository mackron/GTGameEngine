// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDraw_OpenGL21.hpp"
#include "ServerState_OpenGL21.hpp"
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

    void RCDraw::Draw(GLuint* vertexBufferObjectIn, GLuint* indexBufferObjectIn, GLsizei indexCount, const VertexFormat &format, GLenum drawMode)
    {
        this->drawVA.vertexBufferObject = vertexBufferObjectIn;
        this->drawVA.indexBufferObject  = indexBufferObjectIn;
        this->drawVA.indexCount         = indexCount;
        this->drawVA.format             = format;
        this->drawVA.drawMode           = drawMode;

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
            // Bind the vertex array.
            glBindBuffer(GL_ARRAY_BUFFER,         *this->drawVA.vertexBufferObject);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->drawVA.indexBufferObject);

            // Set the vertex state from the given format.
            this->SetVertexAttribState(this->drawVA.format, nullptr);


            // Draw.
            glDrawElements(this->drawVA.drawMode, this->drawVA.indexCount, GL_UNSIGNED_INT, 0);


            // Set state.
            ServerState_GL_ARRAY_BUFFER_BINDING         = *this->drawVA.vertexBufferObject;
            ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING = *this->drawVA.indexBufferObject;
        }
        else if (this->drawType == DRAW_TYPE_RAW)
        {
            // Bind.
            glBindBuffer(GL_ARRAY_BUFFER,         0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // Set the vertex state from the given format.
            this->SetVertexAttribState(this->drawRaw.format, this->drawRaw.vertices);


            // Draw.
            glDrawElements(this->drawRaw.drawMode, this->drawRaw.indexCount, GL_UNSIGNED_INT, this->drawRaw.indices);


            // Set State.
            ServerState_GL_ARRAY_BUFFER_BINDING         = 0;
            ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING = 0;
        }
    }


    ///////////////////////////////////////////////
    // Private

    void RCDraw::SetVertexAttribState(const VertexFormat &format, const float* vertices)
    {
        // Set the vertex state.
        uint32_t  newVertexAttribEnableBits = 0x0;
        uint32_t &oldVertexAttribEnableBits = ServerState_EnabledVertexAttributes;
        size_t    formatArraySize           = format.GetAttributeCount();

        GLsizei formatSizeInBytes = static_cast<GLsizei>(format.GetSizeInBytes());
        int offset = 0;

        for (size_t i = 0; i < formatArraySize; i += 2)
        {
            GLuint   attribIndex = static_cast<GLuint>(format[i]);
            GLint    attribSize  = static_cast<GLint>( format[i + 1]);
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

            glVertexAttribPointer(attribIndex, attribSize, GL_FLOAT, GL_FALSE, formatSizeInBytes, vertices + offset);

            // The offset must be set AFTER glVertexAttribPointer().
            offset += attribSize;
        }

        // If any attributes need to be disable, we need to do that now. This is where our enabled bitfields come in handy.
        while (oldVertexAttribEnableBits != 0)
        {
            GLuint attribIndex  = static_cast<GLuint>(GTLib::NextBitIndex(oldVertexAttribEnableBits));

            glDisableVertexAttribArray(attribIndex);

            // The bit needs to be cleared in preperation for the next iteration.
            oldVertexAttribEnableBits &= ~(1 << attribIndex);
        }

        // Now we need to set the renderer's enabled bits to our new bits.
        ServerState_EnabledVertexAttributes = newVertexAttribEnableBits;
    }
}
