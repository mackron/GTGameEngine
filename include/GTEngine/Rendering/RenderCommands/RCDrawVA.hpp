// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_RCDrawVA_hpp_
#define __GTEngine_RCDrawVA_hpp_

#include "../RenderCommand.hpp"
#include "../../ShaderParameterCache.hpp"

namespace GTEngine
{
    class VertexArray;
    class Shader;

    /// A generic render command for drawing a vertex array with a shader.
    ///
    /// Where possible, it may be more efficient to use a specialized render command. However, this class is very useful
    /// for drawing meshes in a generic way. This render command is used in DefaultViewportRenderer() for drawing meshes.
    class RCDrawVA : public RenderCommand
    {
    // The methods below are called on the non-rendering thread(s).
    public:

        /// Constructor.
        RCDrawVA();

        /// Destructor.
        ~RCDrawVA();

        /// Sets a shader parameter that will be set on the shader in Execute(). It's important that parameters are not set
        /// while Execute() is running.
        template <typename T>
        void SetParameter(const char* name, const T &value)
        {
            this->parameters.Set(name, value);
        }

        void SetParameter(const char* name, float x, float y)                   { this->SetParameter(name, glm::vec2(x, y)); }
        void SetParameter(const char* name, float x, float y, float z)          { this->SetParameter(name, glm::vec3(x, y, z)); }
        void SetParameter(const char* name, float x, float y, float z, float w) { this->SetParameter(name, glm::vec4(x, y, z, w)); }

        /// Clears the vertex array, shader and shader parameters. This is useful when RCDrawVA objects are cached and reused.
        void Clear();

        /// Sets the vertex array.
        void SetVertexArray(VertexArray* newVA) { this->va = newVA; }

        /// Sets the shader to use when rendering the vertex array.
        void SetShader(Shader* shader) { this->shader = shader; }

        
        
    // The methods below are called on the rendering thread.
    public:

        /// RenderCommand::Execute().
        void Execute();


    private:

        /// The vertex array being drawn.
        VertexArray* va;

        /// The shader to use when drawing the vertex array.
        Shader* shader;

        /// The cache of shader parameters. 
        ShaderParameterCache parameters;

    private:    // No copying.
        RCDrawVA(const RCDrawVA &);
        RCDrawVA & operator=(const RCDrawVA &);
    };
}

#endif