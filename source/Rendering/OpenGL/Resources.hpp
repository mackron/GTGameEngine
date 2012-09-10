
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

        GLuint object;
    };

    struct Framebuffer_GL20
    {
        Framebuffer_GL20()
            : object(0)
        {
        }

        GLuint object;
    };

    struct Shader_GL20
    {
        Shader_GL20()
            : program(0)
        {
        }

        GLuint program;
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