#ifndef __GTEngine_Shader_hpp_
#define __GTEngine_Shader_hpp_

#include "ShaderStages.hpp"
#include "Texture2D.hpp"
#include "../Math.hpp"
#include <GTCore/Dictionary.hpp>

namespace GTEngine
{
    struct ShaderParameter;

    /// Class representing a shader.
    ///
    /// The source of a shader is defined at construction time. It can not be changed after construction.
    ///
    /// There are two ways of setting shader parameters. Each tecnique has slightly different details that
    /// applications needs to be aware of.
    ///
    /// The first way of setting parameters is via the shader object itself. With this technique, you simply
    /// use a method such as <code>someShader->SetParameter("SomeParam", SomeFloat4)</code>. The important
    /// detail here is that the parameter won't actually be set until the shader is made current. It is OK
    /// to set parameters with this technique on a thread other than the rendering thread, but it is not
    /// thread-safe. Care needs to be taken to ensure parameters are not being set during Renderer::SetShader().
    ///
    /// The second way of setting parameters is via the renderer itself using Renderer::SetParameter(). With
    /// this technique, the parameter is set on the shader that was made current with the last call to
    /// Renderer::SetShader(). With this technique, the parameter is set immediately and directly.
    class Shader
    {
    public:

        /**
        *   \brief  Constructor.
        */
        Shader(const char* vertexSource, const char* fragmentSource);

        /**
        *   \brief  Destructor.
        */
        ~Shader();

        /**
        *   \brief  Retrieves the shader source string.
        */
        const char* GetVertexSource() const   { return this->vertexSource; }
        const char* GetFragmentSource() const { return this->fragmentSource; }

        
    // Property setters.
    public:

        void SetParameter(const char* name, float value);
        void SetParameter(const char* name, const glm::vec2 &value);
        void SetParameter(const char* name, const glm::vec3 &value);
        void SetParameter(const char* name, const glm::vec4 &value);

        void SetParameter(const char* name, const glm::mat2 &value);
        void SetParameter(const char* name, const glm::mat3 &value);
        void SetParameter(const char* name, const glm::mat4 &value);

        void SetParameter(const char* name, Texture2D* value);

        void SetParameter(const char* name, float x, float y)                   { this->SetParameter(name, glm::vec2(x, y)); }
        void SetParameter(const char* name, float x, float y, float z)          { this->SetParameter(name, glm::vec3(x, y, z)); }
        void SetParameter(const char* name, float x, float y, float z, float w) { this->SetParameter(name, glm::vec4(x, y, z, w)); }


    // The methods below should only be called by the renderer and it's support functions.
    public:

        /**
        *   \brief  Retrieves a pointer to the internal renderer data.
        */
              void * GetRendererData()       { return this->rendererData; }
        const void * GetRendererData() const { return this->rendererData; }

        /**
        *   \brief  Sets the pointer to the internal renderer data.
        *
        *   \remarks
        *       This does not deallocate the previous renderer data. That is the responsibility of the caller.
        */
        void SetRendererData(void *rendererData) { this->rendererData = rendererData; }

        /// Retrieves the internal list of pending parameters.
        /// @return A reference to the internal list of rendering parameters waiting to be set.
        GTCore::Dictionary<char, ShaderParameter*> & GetPendingParameters() { return this->pendingParams; }

        /// Clears the pending parameters.
        void ClearPendingParameters();


    private:

        /// Sets the value of a parameter. T is the type class (ShaderParameter_Float2, etc) and U is the value type (vec2, mat4, Texture2D, etc).
        template <typename T, typename U>
        void SetParameter(const char* name, const U &value)
        {
            auto iParam = this->pendingParams.Find(name);
            if (iParam != nullptr)
            {
                auto param = T::Upcast(iParam->value);
                if (param != nullptr)
                {
                    param->value = value;
                }
                else
                {
                    delete iParam;
                    this->pendingParams.Add(name, new T(value));
                }
            }
            else
            {
                this->pendingParams.Add(name, new T(value));
            }
        }


    private:

        /// The source of the shader.
        char* vertexSource;
        char* fragmentSource;

        /// The parameters that are waiting to be set on the shader. This will be cleared when the shader is
        /// make current on the renderer.
        GTCore::Dictionary<char, ShaderParameter*> pendingParams;

        /// A pointer to renderer-specific data.
        void *rendererData;

        

        friend class Renderer;


    private:    // No copying.
        Shader(const Shader &);
        Shader & operator=(const Shader &);
    };
}

#endif