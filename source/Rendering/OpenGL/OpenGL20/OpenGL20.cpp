// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/OpenGL/OpenGL20.hpp>
#include <GTCore/Math.hpp>
#include <cstdint>

#include "../TypeConversion.hpp"


namespace GTEngine
{
    void OpenGL20::BindTexture(GTEngine::Texture2D &texture)
    {
#if 0
        auto textureData = static_cast<OpenGL20::Texture2D*>(texture.GetRendererData());
        assert(textureData != nullptr);

        //if (texture.GetTarget() == Texture2DTarget_Default)
        {
            glBindTexture(GL_TEXTURE_2D, textureData->object);
        }
#endif
    }

    void OpenGL20::BindTexture(GTEngine::Texture2D* texture)
    {
#if 0
        if (texture != nullptr)
        {
            OpenGL20::BindTexture(*texture);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
#endif
    }

    void OpenGL20::BindTexture(GTEngine::TextureCube &texture)
    {
#if 0
        auto textureData = static_cast<OpenGL20::Texture2D*>(texture.GetRendererData());
        assert(textureData != nullptr);

        glBindTexture(GL_TEXTURE_CUBE_MAP, textureData->object);
#endif
    }

    void OpenGL20::BindTexture(GTEngine::TextureCube* texture)
    {
        if (texture != nullptr)
        {
            OpenGL20::BindTexture(*texture);
        }
        else
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    }






    static const VertexArray* BoundVertexArray = nullptr;              ///< The currently bound vertex array.
    static GLuint CurrentGLVertexBufferObject = 0;
    static GLuint CurrentGLIndexBufferObject  = 0;


    void OpenGL20::BindVertexArray(const GTEngine::VertexArray* vertexArray)
    {
        if (vertexArray != nullptr)
        {
            if (vertexArray != BoundVertexArray || CurrentGLVertexBufferObject == 0 || CurrentGLIndexBufferObject == 0)
            {
                auto vertexArrayGL = static_cast<const OpenGL20::VertexArray*>(vertexArray->GetRendererData());
                assert(vertexArrayGL != nullptr);


                if (CurrentGLVertexBufferObject != vertexArrayGL->verticesObject)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayGL->verticesObject);
                    CurrentGLVertexBufferObject = vertexArrayGL->verticesObject;
                }

                if (CurrentGLIndexBufferObject != vertexArrayGL->indicesObject)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayGL->indicesObject);
                    CurrentGLIndexBufferObject  = vertexArrayGL->indicesObject;
                }
            }
        }
        else
        {
            if (CurrentGLVertexBufferObject != 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                CurrentGLVertexBufferObject = 0;
            }
            
            if (CurrentGLIndexBufferObject != 0)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                CurrentGLIndexBufferObject = 0;
            }
        }


        BoundVertexArray = vertexArray;
    }

    void OpenGL20::BindVertexArray(const GTEngine::VertexArray &vertexArray)
    {
        OpenGL20::BindVertexArray(&vertexArray);
    }


    static uint32_t VertexAttribEnableBits = 0;
    void OpenGL20::EnableVertexFormat(const VertexFormat &format, const float* vertices)
    {
        // We need to enable all of the relevant vertex attributes and set their pointers. What we do is we loop over the
        // attributes in the vertex array's format and set the pointers and enable. Then, we disable any previously enabled
        // attributes that we no longer want enabled.
        uint32_t  newVertexAttribEnableBits = 0x0;
        uint32_t &oldVertexAttribEnableBits = VertexAttribEnableBits;
        size_t    formatArraySize           = format.GetAttributeCount();

        GLsizei formatSizeInBytes = static_cast<GLsizei>(format.GetSizeInBytes());
        int offset = 0;

        for (size_t i = 0; i < formatArraySize; i += 2)
        {
            GLuint   attribIndex = static_cast<GLuint>(format[i]);
            GLint    attribSize  = static_cast<GLint>(format[i + 1]);
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

            glVertexAttribPointer(attribIndex, attribSize, GL_FLOAT, GL_FALSE, formatSizeInBytes, (vertices +  offset));

            // The offset must be set AFTER glVertexAttribPointer().
            offset += attribSize;
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
        VertexAttribEnableBits = newVertexAttribEnableBits;
    }


    GLuint OpenGL20::GetCurrentOpenGLVertexBufferObject()
    {
        return CurrentGLVertexBufferObject;
    }

    GLuint OpenGL20::GetCurrentOpenGLIndexBufferObject()
    {
        return CurrentGLIndexBufferObject;
    }



    void OpenGL20::SetFramebufferColourBuffer(GLuint colourBuffer, size_t colourBufferIndex, GLenum textureTarget)
    {
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + colourBufferIndex, textureTarget, colourBuffer, 0);
    }

    void OpenGL20::SetFramebufferDepthBuffer(GLuint buffer)
    {
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, buffer, 0);
    }

    void OpenGL20::SetFramebufferStencilBuffer(GLuint buffer)
    {
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, buffer, 0);
    }
}
