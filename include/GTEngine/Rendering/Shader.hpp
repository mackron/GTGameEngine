// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Shader_hpp_
#define __GTEngine_Shader_hpp_

#include "ShaderStages.hpp"
#include "../Math.hpp"
#include "../ShaderParameterCache.hpp"
#include <GTCore/String.hpp>

namespace GTEngine
{
    class Texture2D;
    class TextureCube;
    class Material;

    /// Class representing a shader.
    ///
    /// The uniforms need to be pushed to the GPU with Renderer::PushShaderUniforms() before they will be used by the renderer.
    class Shader
    {
    protected:

        /// Constructor.
        Shader(const char* vertexSource, const char* fragmentSource, const char* geometryShader);


    public:

        /// Destructor.
        virtual ~Shader();


        /// Retrieves the shader source string.
        const char* GetVertexSource()   const { return this->vertexSource.c_str();   }
        const char* GetFragmentSource() const { return this->fragmentSource.c_str(); }
        const char* GetGeometrySource() const { return this->geometrySource.c_str(); }



        //////////////////////////////////////////////
        // Uniforms.

        virtual void SetUniform(const char* name, float x);
        virtual void SetUniform(const char* name, float x, float y);
        virtual void SetUniform(const char* name, float x, float y, float z);
        virtual void SetUniform(const char* name, float x, float y, float z, float w);
        virtual void SetUniform(const char* name, const glm::mat2 &value);
        virtual void SetUniform(const char* name, const glm::mat3 &value);
        virtual void SetUniform(const char* name, const glm::mat4 &value);
        virtual void SetUniform(const char* name, const Texture2D* value);
        virtual void SetUniform(const char* name, const TextureCube* value);

        void SetUniform(const char* name, const glm::vec2 &value) { this->SetUniform(name, value.x, value.y); }
        void SetUniform(const char* name, const glm::vec3 &value) { this->SetUniform(name, value.x, value.y, value.z); }
        void SetUniform(const char* name, const glm::vec4 &value) { this->SetUniform(name, value.x, value.y, value.z, value.w); }


        /// Helper for setting the uniforms defined by the given material.
        void SetUniformsFromMaterial(const Material &material);
        


    private:

        /// The source of the shader.
        GTCore::String vertexSource;
        GTCore::String fragmentSource;
        GTCore::String geometrySource;



    private:    // No copying.
        Shader(const Shader &);
        Shader & operator=(const Shader &);
    };
}

#endif
