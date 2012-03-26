
#ifndef __GTEngine_DrawCallCache_hpp_
#define __GTEngine_DrawCallCache_hpp_

#include "RenderCommands.hpp"
#include "ShaderStages.hpp"
#include "DrawModes.hpp"
#include <GTCore/Vector.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#define GLM_FORCE_ONLY_XYZW
#include <glm/glm.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

namespace GTEngine
{
    class Texture2D;
    class Shader;
    class Framebuffer;
    class VertexArray;
    class Material;

    /**
    *   \brief  Class representing a buffer of rendering commands.
    *
    *   This class does not make copies of any attached draw calls. It is the applications responsibility to manage
    *   allocation and deallocation of render commands.
    */
    class RenderCommandBuffer
    {
    public:

        /// Constructor.
        RenderCommandBuffer();

        /**
        *   \brief  Appends a single draw call to the end of the cache.
        *
        *   \remarks
        *       You can append a chain of draw calls by using Append(RenderCommand *, RenderCommand *).
        */
        void Append(RenderCommand &cmd);

        /**
        *   \brief  Executes all of the commands in the buffer, but does not clear it.
        *
        *   \remarks
        *       The rendering commands will remain in the buffer after calling this function. Use Clear() to remove
        *       the commands.
        */
        void Execute();

        /**
        *   \brief  Clears the buffer.
        */
        void Clear();

        /**
        *   \brief  Checks if the cache is empty.
        */
        bool IsEmpty() const
        {
            return this->commands.count == 0;
        }


    // The methods below are helper functions for caching an engine draw call. These methods will create the appropriate
    // RenderCommand class and add them with Append().
    public:

        void SetViewport(int x, int y, unsigned int width, unsigned int height);
        void SetScissor(int x, int y, unsigned int width, unsigned int height);
        void Draw(const VertexArray *vertexArray, DrawMode mode = DrawMode_Triangles);

        void SetShader(Shader *shader);
        void SetShaderParameter(const char *name, Texture2D *value);
        void SetShaderParameter(const char *name, float value);
        void SetShaderParameter(const char *name, const glm::vec2 &value);
        void SetShaderParameter(const char *name, const glm::vec3 &value);
        void SetShaderParameter(const char *name, const glm::vec4 &value);
        void SetShaderParameter(const char *name, const glm::mat2 &value);
        void SetShaderParameter(const char *name, const glm::mat3 &value);
        void SetShaderParameter(const char *name, const glm::mat4 &value);

        void SetFramebuffer(Framebuffer *framebuffer);

    private:

        /// The list containing all of the rendering commands.
        GTCore::Vector<RenderCommand*> commands;


    private:    // No copying.
        RenderCommandBuffer(const RenderCommandBuffer &);
        RenderCommandBuffer & operator=(const RenderCommandBuffer &);

    };
}

#endif