
#ifndef __GTEngine_Rendering_OpenGL_Resources_hpp_
#define __GTEngine_Rendering_OpenGL_Resources_hpp_

#include <GTEngine/Rendering/Renderer.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    struct Texture2D_GL20
    {
        Texture2D_GL20()
            : object(0)
        {
        }

        Texture2D_GL20(GLuint object)
            : object(object)
        {
        }

        GLuint object;
    };

    struct Framebuffer_GL20
    {
        Framebuffer_GL20()
            : object(0), colourAttachments(nullptr), depthStencilAttachment(nullptr)
        {
            size_t colourCount = Renderer::GetMaxColourAttachments();

            this->colourAttachments = new Texture2D*[colourCount];
            for (size_t i = 0; i < colourCount; ++i)
            {
                this->colourAttachments[i] = nullptr;
            }
        }

        ~Framebuffer_GL20()
        {
            delete [] this->colourAttachments;
        }

        GLuint object;

        Texture2D** colourAttachments;
        Texture2D * depthStencilAttachment;

    private:    // No copying.
        Framebuffer_GL20(const Framebuffer_GL20 &);
        Framebuffer_GL20 & operator=(const Framebuffer_GL20 &);
    };

    struct Shader_GL20
    {
        Shader_GL20()
            : program(0), vertexShader(0), fragmentShader(0)
        {
        }

        /// The main program object.
        GLuint program;

        /// The vertex shader object. We keep hold of this so we can relink when vertex atribute positions are changed.
        GLuint vertexShader;

        /// The fragment shader object. We keep hold of this so we can relink when vertex attribute positions are changed.
        GLuint fragmentShader;
    };

    struct VertexArray_GL20
    {
        VertexArray_GL20()
            : verticesObject(0), indicesObject(0),
              vertexCount(0), indexCount(0)
        {
        }

        GLuint verticesObject;
        GLuint indicesObject;

        GLsizei vertexCount;
        GLsizei indexCount;
    };
}

#endif