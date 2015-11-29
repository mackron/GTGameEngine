// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_OpenGL21_RCSetShaderState_hpp_
#define __GTEngine_Rendering_OpenGL21_RCSetShaderState_hpp_

#include <GTEngine/ShaderParameterCache.hpp>
#include <GTEngine/Core/Dictionary.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

#include "../../RenderCommand.hpp"
#include "ShaderState_OpenGL21.hpp"

namespace GT
{
    /// Render command for clearing the buffers/
    class RCSetShaderState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetShaderState();

        /// Destructor.
        ~RCSetShaderState();


        /// Sets the pending uniforms of the given state.
        void SetFloatUniforms(ShaderState_OpenGL21* programStateIn);
        void SetFloat2Uniforms(ShaderState_OpenGL21* programStateIn);
        void SetFloat3Uniforms(ShaderState_OpenGL21* programStateIn);
        void SetFloat4Uniforms(ShaderState_OpenGL21* programStateIn);
        void SetFloat2x2Uniforms(ShaderState_OpenGL21* programStateIn);
        void SetFloat3x3Uniforms(ShaderState_OpenGL21* programStateIn);
        void SetFloat4x4Uniforms(ShaderState_OpenGL21* programStateIn);
        void SetTextureUniforms(ShaderState_OpenGL21* programStateIn);


        /// Retrieves the program object whose state is being set.
        ShaderState_OpenGL21* GetProgramState() const;



        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A helper for setting a texture uniform.
        void SetTextureUniform(GLint location, ShaderState_OpenGL21::TextureParameter &value);



    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// The shader state being modified.
        ShaderState_OpenGL21* programState;


        /// The parameters that should be set by name. These default to null. We use pointers here so make it easier to determine whether or not a group can be ignored.
        Dictionary<ShaderState_OpenGL21::FloatParameter>*    floatUniformsToSetByName;
        Dictionary<ShaderState_OpenGL21::Float2Parameter>*   float2UniformsToSetByName;
        Dictionary<ShaderState_OpenGL21::Float3Parameter>*   float3UniformsToSetByName;
        Dictionary<ShaderState_OpenGL21::Float4Parameter>*   float4UniformsToSetByName;
        Dictionary<ShaderState_OpenGL21::Float2x2Parameter>* float2x2UniformsToSetByName;
        Dictionary<ShaderState_OpenGL21::Float3x3Parameter>* float3x3UniformsToSetByName;
        Dictionary<ShaderState_OpenGL21::Float4x4Parameter>* float4x4UniformsToSetByName;
        Dictionary<ShaderState_OpenGL21::TextureParameter>*  textureUniformsToSetByName;


        /// The uniforms that should be set by their location. These default to null. We use pointers here so make it easier to determine whether or not a group can be ignored.
        Vector<ShaderState_OpenGL21::FloatParameter>*    floatUniformsToSetByLocation;
        Vector<ShaderState_OpenGL21::Float2Parameter>*   float2UniformsToSetByLocation;
        Vector<ShaderState_OpenGL21::Float3Parameter>*   float3UniformsToSetByLocation;
        Vector<ShaderState_OpenGL21::Float4Parameter>*   float4UniformsToSetByLocation;
        Vector<ShaderState_OpenGL21::Float2x2Parameter>* float2x2UniformsToSetByLocation;
        Vector<ShaderState_OpenGL21::Float3x3Parameter>* float3x3UniformsToSetByLocation;
        Vector<ShaderState_OpenGL21::Float4x4Parameter>* float4x4UniformsToSetByLocation;
        Vector<ShaderState_OpenGL21::TextureParameter>*  textureUniformsToSetByLocation;



    private:    // No copying.
        RCSetShaderState(const RCSetShaderState &);
        RCSetShaderState & operator=(const RCSetShaderState &);
    };
}

#endif
