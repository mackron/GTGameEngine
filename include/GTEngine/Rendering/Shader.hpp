#ifndef __GTEngine_Shader_hpp_
#define __GTEngine_Shader_hpp_

#include "ShaderStages.hpp"
#include "../Math.hpp"
#include "../ShaderParameterCache.hpp"

namespace GTEngine
{
    class Texture2D;

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

        template <typename T>
        void SetParameter(const char* name, const T &value)
        {
            this->pendingParameters.Set(name, value);
        }

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
        void SetRendererData(void* rendererData) { this->rendererData = rendererData; }

        /// Retrieves the internal list of pending parameters.
        /// @return A reference to the internal list of rendering parameters waiting to be set.
        GTCore::Dictionary<ShaderParameter*> & GetPendingParameters() { return this->pendingParameters.GetParameters(); }

        /// Clears the pending parameters.
        void ClearPendingParameters();


        /// Called when a texture is destructed and thus no longer part of the shader.
        void OnTextureDeleted(Texture2D* texture);

        /// Called when a texture parameter is being changed. We use this to check whether or not we need to let the texture
        /// know that it's no longer being used by the shader.
        void OnTextureParameterChanged(Texture2D* oldTexture);



    private:

        /// The source of the shader.
        char* vertexSource;
        char* fragmentSource;

        /// The parameters that are waiting to be set on the shader. This will be cleared when the shader is made current on the renderer.
        ShaderParameterCache pendingParameters;

        /// A pointer to renderer-specific data.
        void *rendererData;


        /// We keep an array of texture 2Ds being used by the shader. As the shader parameters of a shader change, they are updated here. These
        /// are index by parameter name. This is NOT updated by SetParameter(), but is instead used internally by the renderer.
        GTCore::Dictionary<Texture2D*> currentTexture2Ds;


    private:    // No copying.
        Shader(const Shader &);
        Shader & operator=(const Shader &);


    friend class Renderer;
    };
}

#endif