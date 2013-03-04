// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL20_hpp_
#define __GTEngine_Rendering_OpenGL20_hpp_

#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/TextureCube.hpp>
#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/Rendering/Framebuffer.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Static helper class for working with OpenGL.
    class OpenGL20
    {
    public:

        /// Binds the given 2D texture.
        ///
        /// @param texture [in] The texture to bind.
        static void BindTexture(Texture2D &texture);
        static void BindTexture(Texture2D* texture);

        /// Binds the given cube map.
        ///
        /// @param texture [in] The texture to bind.
        static void BindTexture(TextureCube &texture);
        static void BindTexture(TextureCube* texture);



        /// Binds the given vertex array.
        ///
        /// @param vertexArray [in] The vertex array to bind.
        static void BindVertexArray(const VertexArray* vertexArray);
        static void BindVertexArray(const VertexArray &vertexArray);

        /// Enables the given vertex format.
        ///
        /// @param format   [in] The vertex format to enable.
        /// @param vertices [in] A pointer to the vertex data to enable.
        static void EnableVertexFormat(const VertexFormat &format, const float* vertices);


        /// Retrieves the OpenGL object of the current vertex buffer.
        static GLuint GetCurrentOpenGLVertexBufferObject();

        /// Retrieves the OpenGL object of the current index buffer.
        static GLuint GetCurrentOpenGLIndexBufferObject();



        /// Sets a colour buffer on the currently bound framebuffer.
        ///
        /// @param colourBuffer      [in] The colour to attach to the framebuffer.
        /// @param colourBufferIndex [in] The index of the colour buffer to attach the colour buffer to.
        /// @param textureTarget     [in] The texture target, such as GL_TEXTURE_2D.
        static void SetFramebufferColourBuffer(GLuint colourBuffer, size_t colourBufferIndex, GLenum textureTarget);

        /// Sets the depth buffer on the given framebuffer.
        ///
        /// @param buffer [in] The buffer object to attach to the framebuffer. This is a Texture2D object.
        static void SetFramebufferDepthBuffer(GLuint buffer);

        /// Sets the stencil buffer for the given framebuffer.
        ///
        /// @param buffer [in] The buffer object to attach to the framebuffer. This is a Texture2D object.
        static void SetFramebufferStencilBuffer(GLuint buffer);





        ///////////////////////////////////////////////////////////////////////////////
        // Resource Objects.

        struct Texture2D
        {
            Texture2D()
                : object(0)
            {
            }

            GLuint object;
        };

        struct TextureCube
        {
            TextureCube()
                : object(0)
            {
            }

            GLuint object;
        };

        struct Framebuffer
        {
            Framebuffer()
                : object(0)
            {
            }

            GLuint object;
        };

        struct Shader
        {
            Shader()
                : program(0)
            {
            }

            GLuint program;
        };

        struct VertexArray
        {
            VertexArray()
                : verticesObject(0), indicesObject(0),
                  vertexCount(0), indexCount(0)
            {
            }

            GLuint verticesObject;
            GLuint indicesObject;

            GLsizei vertexCount;
            GLsizei indexCount;
        };
    };
}

#endif