// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetShaderState_OpenGL21.hpp"
#include "ServerState_OpenGL21.hpp"
#include "../TypeConversion.hpp"
#include <assert.h>

namespace GTEngine
{
    RCSetShaderState::RCSetShaderState()
        : operationBitfield(0),
          programState(nullptr),
          floatUniformsToSetByName(nullptr),
          float2UniformsToSetByName(nullptr),
          float3UniformsToSetByName(nullptr),
          float4UniformsToSetByName(nullptr),
          float2x2UniformsToSetByName(nullptr),
          float3x3UniformsToSetByName(nullptr),
          float4x4UniformsToSetByName(nullptr),
          textureUniformsToSetByName(nullptr),
          floatUniformsToSetByLocation(nullptr),
          float2UniformsToSetByLocation(nullptr),
          float3UniformsToSetByLocation(nullptr),
          float4UniformsToSetByLocation(nullptr),
          float2x2UniformsToSetByLocation(nullptr),
          float3x3UniformsToSetByLocation(nullptr),
          float4x4UniformsToSetByLocation(nullptr),
          textureUniformsToSetByLocation(nullptr)
    {
    }

    RCSetShaderState::~RCSetShaderState()
    {
        delete this->floatUniformsToSetByName;
        delete this->float2UniformsToSetByName;
        delete this->float3UniformsToSetByName;
        delete this->float4UniformsToSetByName;
        delete this->float2x2UniformsToSetByName;
        delete this->float3x3UniformsToSetByName;
        delete this->float4x4UniformsToSetByName;
        delete this->textureUniformsToSetByName;
        delete this->floatUniformsToSetByLocation;
        delete this->float2UniformsToSetByLocation;
        delete this->float3UniformsToSetByLocation;
        delete this->float4UniformsToSetByLocation;
        delete this->float2x2UniformsToSetByLocation;
        delete this->float3x3UniformsToSetByLocation;
        delete this->float4x4UniformsToSetByLocation;
        delete this->textureUniformsToSetByLocation;
    }


    void RCSetShaderState::SetFloatUniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingFloatUniformsByName.count > 0)
                {
                    this->floatUniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::FloatParameter>(programStateIn->pendingFloatUniformsByName);
                }
                if (programStateIn->pendingFloatUniformsByLocation.count > 0)
                {
                    this->floatUniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::FloatParameter>(programStateIn->pendingFloatUniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }

    void RCSetShaderState::SetFloat2Uniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingFloat2UniformsByName.count > 0)
                {
                    this->float2UniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::Float2Parameter>(programStateIn->pendingFloat2UniformsByName);
                }
                if (programStateIn->pendingFloat2UniformsByLocation.count > 0)
                {
                    this->float2UniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::Float2Parameter>(programStateIn->pendingFloat2UniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }

    void RCSetShaderState::SetFloat3Uniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingFloat3UniformsByName.count > 0)
                {
                    this->float3UniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::Float3Parameter>(programStateIn->pendingFloat3UniformsByName);
                }
                if (programStateIn->pendingFloat3UniformsByLocation.count > 0)
                {
                    this->float3UniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::Float3Parameter>(programStateIn->pendingFloat3UniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }

    void RCSetShaderState::SetFloat4Uniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingFloat4UniformsByName.count > 0)
                {
                    this->float4UniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::Float4Parameter>(programStateIn->pendingFloat4UniformsByName);
                }
                if (programStateIn->pendingFloat4UniformsByLocation.count > 0)
                {
                    this->float4UniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::Float4Parameter>(programStateIn->pendingFloat4UniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }


    void RCSetShaderState::SetFloat2x2Uniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingFloat2x2UniformsByName.count > 0)
                {
                    this->float2x2UniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::Float2x2Parameter>(programStateIn->pendingFloat2x2UniformsByName);
                }
                if (programStateIn->pendingFloat2x2UniformsByLocation.count > 0)
                {
                    this->float2x2UniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::Float2x2Parameter>(programStateIn->pendingFloat2x2UniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }

    void RCSetShaderState::SetFloat3x3Uniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingFloat3x3UniformsByName.count > 0)
                {
                    this->float3x3UniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::Float3x3Parameter>(programStateIn->pendingFloat3x3UniformsByName);
                }
                if (programStateIn->pendingFloat3x3UniformsByLocation.count > 0)
                {
                    this->float3x3UniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::Float3x3Parameter>(programStateIn->pendingFloat3x3UniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }

    void RCSetShaderState::SetFloat4x4Uniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingFloat4x4UniformsByName.count > 0)
                {
                    this->float4x4UniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::Float4x4Parameter>(programStateIn->pendingFloat4x4UniformsByName);
                }
                if (programStateIn->pendingFloat4x4UniformsByLocation.count > 0)
                {
                    this->float4x4UniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::Float4x4Parameter>(programStateIn->pendingFloat4x4UniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }


    void RCSetShaderState::SetTextureUniforms(ShaderState_OpenGL21* programStateIn)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                if (programStateIn->pendingTextureUniformsByName.count > 0)
                {
                    this->textureUniformsToSetByName = new GTLib::Dictionary<ShaderState_OpenGL21::TextureParameter>(programStateIn->pendingTextureUniformsByName);
                }
                if (programStateIn->pendingTextureUniformsByLocation.count > 0)
                {
                    this->textureUniformsToSetByLocation = new GTLib::Vector<ShaderState_OpenGL21::TextureParameter>(programStateIn->pendingTextureUniformsByLocation);
                }


                this->programState = programStateIn;
            }
        }
    }



    ShaderState_OpenGL21* RCSetShaderState::GetProgramState() const
    {
        return this->programState;
    }


    void RCSetShaderState::Execute()
    {
        assert(this->programState != nullptr);
        {
            // We need to bind the program. However, we need to maintain the integrity of the global state. Thus, we need to grab the current
            // shader so it can be restored later. Basically, we're emulating Direct State Access.
            if (ServerState_GL_CURRENT_PROGRAM != this->programState->programObject)
            {
                glUseProgram(this->programState->programObject);
            }



            // Float.
            if (this->floatUniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->floatUniformsToSetByName->count; ++i)
                {
                    auto  parameterName  = this->floatUniformsToSetByName->buffer[i]->key;
                    auto &parameterValue = this->floatUniformsToSetByName->buffer[i]->value;

                    glUniform1f(glGetUniformLocation(this->programState->programObject, parameterName), parameterValue.x);
                }
            }

            if (this->floatUniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0; i < this->floatUniformsToSetByLocation->count; ++i)
                {
                    auto &parameter = this->floatUniformsToSetByLocation->Get(i);
                    {
                        glUniform1f(parameter.location, parameter.x);
                    }
                }
            }

            // Float2.
            if (this->float2UniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->float2UniformsToSetByName->count; ++i)
                {
                    auto  parameterName  = this->float2UniformsToSetByName->buffer[i]->key;
                    auto &parameterValue = this->float2UniformsToSetByName->buffer[i]->value;

                    glUniform2f(glGetUniformLocation(this->programState->programObject, parameterName), parameterValue.x, parameterValue.y);
                }
            }

            if (this->float2UniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0; i < this->float2UniformsToSetByLocation->count; ++i)
                {
                    auto &parameter = this->float2UniformsToSetByLocation->Get(i);
                    {
                        glUniform2f(parameter.location, parameter.x, parameter.y);
                    }
                }
            }

            // Float3.
            if (this->float3UniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->float3UniformsToSetByName->count; ++i)
                {
                    auto  parameterName  = this->float3UniformsToSetByName->buffer[i]->key;
                    auto &parameterValue = this->float3UniformsToSetByName->buffer[i]->value;

                    glUniform3f(glGetUniformLocation(this->programState->programObject, parameterName), parameterValue.x, parameterValue.y, parameterValue.z);
                }
            }

            if (this->float3UniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0; i < this->float3UniformsToSetByLocation->count; ++i)
                {
                    auto &parameter = this->float3UniformsToSetByLocation->Get(i);
                    {
                        glUniform3f(parameter.location, parameter.x, parameter.y, parameter.z);
                    }
                }
            }

            // Float4.
            if (this->float4UniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->float4UniformsToSetByName->count; ++i)
                {
                    auto  parameterName  = this->float4UniformsToSetByName->buffer[i]->key;
                    auto &parameterValue = this->float4UniformsToSetByName->buffer[i]->value;

                    glUniform4f(glGetUniformLocation(this->programState->programObject, parameterName), parameterValue.x, parameterValue.y, parameterValue.z, parameterValue.w);
                }
            }

            if (this->float4UniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0; i < this->float4UniformsToSetByLocation->count; ++i)
                {
                    auto &parameter = this->float4UniformsToSetByLocation->Get(i);
                    {
                        glUniform4f(parameter.location, parameter.x, parameter.y, parameter.z, parameter.w);
                    }
                }
            }


            // Float2x2.
            if (this->float2x2UniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->float2x2UniformsToSetByName->count; ++i)
                {
                    auto  parameterName  = this->float2x2UniformsToSetByName->buffer[i]->key;
                    auto &parameterValue = this->float2x2UniformsToSetByName->buffer[i]->value;

                    glUniformMatrix2fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, false, parameterValue.value);
                }
            }

            if (this->float2x2UniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0; i < this->float2x2UniformsToSetByLocation->count; ++i)
                {
                    auto &parameter = this->float2x2UniformsToSetByLocation->Get(i);
                    {
                        glUniformMatrix2fv(parameter.location, 1, false, parameter.value);
                    }
                }
            }

            // Float3x3.
            if (this->float3x3UniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->float3x3UniformsToSetByName->count; ++i)
                {
                    auto  parameterName  = this->float3x3UniformsToSetByName->buffer[i]->key;
                    auto &parameterValue = this->float3x3UniformsToSetByName->buffer[i]->value;

                    glUniformMatrix3fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, false, parameterValue.value);
                }
            }

            if (this->float3x3UniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0; i < this->float3x3UniformsToSetByLocation->count; ++i)
                {
                    auto &parameter = this->float3x3UniformsToSetByLocation->Get(i);
                    {
                        glUniformMatrix3fv(parameter.location, 1, false, parameter.value);
                    }
                }
            }

            // Float4x4.
            if (this->float4x4UniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->float4x4UniformsToSetByName->count; ++i)
                {
                    auto  parameterName  = this->float4x4UniformsToSetByName->buffer[i]->key;
                    auto &parameterValue = this->float4x4UniformsToSetByName->buffer[i]->value;

                    glUniformMatrix4fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, false, parameterValue.value);
                }
            }

            if (this->float4x4UniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0; i < this->float4x4UniformsToSetByLocation->count; ++i)
                {
                    auto &parameter = this->float4x4UniformsToSetByLocation->Get(i);
                    {
                        glUniformMatrix4fv(parameter.location, 1, false, parameter.value);
                    }
                }
            }


            // Textures.
            if (this->textureUniformsToSetByName != nullptr)
            {
                for (size_t i = 0; i < this->textureUniformsToSetByName->count; ++i)
                {
                    GLint uniformLocation = glGetUniformLocation(this->programState->programObject, this->textureUniformsToSetByName->buffer[i]->key);
                    auto &uniformValue    = this->textureUniformsToSetByName->buffer[i]->value;

                    this->SetTextureUniform(uniformLocation, uniformValue);
                }
            }

            if (this->textureUniformsToSetByLocation != nullptr)
            {
                for (size_t i = 0 ; i < this->textureUniformsToSetByLocation->count; ++i)
                {
                    auto &uniform = this->textureUniformsToSetByLocation->Get(i);
                    {
                        this->SetTextureUniform(uniform.location, uniform);
                    }
                }
            }




            // Need to rebind the previous program because it's important that we maintain the integrity of the global state.
            if (ServerState_GL_CURRENT_PROGRAM != this->programState->programObject)
            {
                glUseProgram(ServerState_GL_CURRENT_PROGRAM);
            }
        }
    }

    void RCSetShaderState::SetTextureUniform(GLint location, ShaderState_OpenGL21::TextureParameter &value)
    {
        if (location != -1)
        {
            bool setUniform = true;
            bool setBinding = true;

            // We need to check if the shader state has the texture already set.
            auto iExistingParameter = this->programState->currentTextureUniforms.Find(location);
            if (iExistingParameter != nullptr)
            {
                // The parameter already exists. The uniform does not need to be set, but the texture binding might.
                setUniform = false;

                if (iExistingParameter->value.textureState == value.textureState)
                {
                    setBinding = false;
                }
                else
                {
                    iExistingParameter->value.textureState->shaders.RemoveFirstOccuranceOf(this->programState);
                    iExistingParameter->value.textureState = value.textureState;
                    iExistingParameter->value.textureState->shaders.PushBack(this->programState);

                    value.textureUnit = iExistingParameter->value.textureUnit;
                    value.location    = iExistingParameter->value.location;
                }
            }
            else
            {
                // The parameter has not been set before. We need to set the uniform and bind.
                value.textureUnit = this->programState->FindAvailableTextureUnit();
                value.location    = location;
                value.textureState->shaders.PushBack(this->programState);

                this->programState->currentTextureUniforms.Add(value.location, value);
            }


            // If this program state is the current one, we'll need to bind the texture straight away.
            if (ServerState_GL_CURRENT_PROGRAM == this->programState->programObject && setBinding)
            {
                glActiveTexture(GL_TEXTURE0 + value.textureUnit);
                glBindTexture(value.textureTarget, value.textureState->objectGL);

                // State needs to be set.
                if (value.textureTarget == GL_TEXTURE_1D)
                {
                    ServerState_GL_TEXTURE_BINDING_1D = value.textureState->objectGL;
                }
                else if (value.textureTarget == GL_TEXTURE_2D)
                {
                    ServerState_GL_TEXTURE_BINDING_2D = value.textureState->objectGL;
                }
                else if (value.textureTarget == GL_TEXTURE_3D)
                {
                    ServerState_GL_TEXTURE_BINDING_3D = value.textureState->objectGL;
                }
                else if (value.textureTarget == GL_TEXTURE_CUBE_MAP)
                {
                    ServerState_GL_TEXTURE_BINDING_CUBE = value.textureState->objectGL;
                }
            }

            if (setUniform)
            {
                glUniform1i(value.location, value.textureUnit);
            }
        }
    }
}
