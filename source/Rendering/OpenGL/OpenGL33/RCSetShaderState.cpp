// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetShaderState.hpp"
#include "../TypeConversion.hpp"
#include <assert.h>

namespace GTEngine
{
    #define SET_BASIC_PARAMETER_BIT       (1 << 1)
    #define SET_TEXTURE_PARAMETER_BIT     (1 << 2)


    RCSetShaderState::RCSetShaderState()
        : operationBitfield(0),
          programState(nullptr), basicParametersToSet(), textureParametersToSet()
    {
    }


    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x, float y)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x, y);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x, float y, float z)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x, y, z);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x, float y, float z, float w)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x, y, z, w);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x, int y)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x, y);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x, int y, int z)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x, y, z);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x, int y, int z, int w)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, x, y, z, w);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::mat2x2 &value)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, value);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::mat3x3 &value)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, value);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }

    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::mat4x4 &value)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                this->basicParametersToSet.Set(name, value);

                this->programState = programStateIn;
                this->operationBitfield |= SET_BASIC_PARAMETER_BIT;
            }
        }
    }


    void RCSetShaderState::SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, GLuint* textureObject, GLenum textureTarget)
    {
        assert(programStateIn != nullptr);
        {
            assert(this->programState == nullptr || this->programState == programStateIn);
            {
                ShaderState_OpenGL33::TextureParameter parameter;
                parameter.textureObject = textureObject;
                parameter.textureTarget = textureTarget;
                this->textureParametersToSet.Add(name, parameter);

                this->programState = programStateIn;
                this->operationBitfield |= SET_TEXTURE_PARAMETER_BIT;
            }
        }
    }


    ShaderState_OpenGL33* RCSetShaderState::GetProgramState() const
    {
        return this->programState;
    }


    void RCSetShaderState::Execute()
    {
        assert(this->programState != nullptr);
        {
            // We need to bind the program. However, we need to maintain the integrity of the global state. Thus, we need to grab the current
            // shader so it can be restored later. Basically, we're emulating Direct State Access.
            GLint previousCurrentProgram;
            glGetIntegerv(GL_CURRENT_PROGRAM, &previousCurrentProgram);

            if (static_cast<GLuint>(previousCurrentProgram) != this->programState->programObject)
            {
                glUseProgram(this->programState->programObject);
            }



            if ((this->operationBitfield & SET_BASIC_PARAMETER_BIT))
            {
                for (size_t i = 0; i < this->basicParametersToSet.GetCount(); ++i)
                {
                    auto parameterName = this->basicParametersToSet.GetNameByIndex(i);
                    auto parameter     = this->basicParametersToSet.GetByIndex(i);

                    assert(parameterName != nullptr);
                    assert(parameter     != nullptr);
                    {
                        switch (parameter->type)
                        {
                        // Floats.
                        case ShaderParameterType_Float:
                            {
                                glUniform1f(glGetUniformLocation(this->programState->programObject, parameterName), static_cast<ShaderParameter_Float*>(parameter)->value);
                                break;
                            }

                        case ShaderParameterType_Float2:
                            {
                                glUniform2fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, &static_cast<ShaderParameter_Float2*>(parameter)->value.x);
                                break;
                            }

                        case ShaderParameterType_Float3:
                            {
                                glUniform3fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, &static_cast<ShaderParameter_Float3*>(parameter)->value.x);
                                break;
                            }

                        case ShaderParameterType_Float4:
                            {
                                glUniform4fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, &static_cast<ShaderParameter_Float4*>(parameter)->value.x);
                                break;
                            }


                        // Integers.
                        case ShaderParameterType_Integer:
                            {
                                glUniform1i(glGetUniformLocation(this->programState->programObject, parameterName), static_cast<ShaderParameter_Integer*>(parameter)->value);
                                break;
                            }

                        case ShaderParameterType_Integer2:
                            {
                                glUniform2iv(glGetUniformLocation(this->programState->programObject, parameterName), 1, &static_cast<ShaderParameter_Integer2*>(parameter)->value.x);
                                break;
                            }

                        case ShaderParameterType_Integer3:
                            {
                                glUniform3iv(glGetUniformLocation(this->programState->programObject, parameterName), 1, &static_cast<ShaderParameter_Integer3*>(parameter)->value.x);
                                break;
                            }

                        case ShaderParameterType_Integer4:
                            {
                                glUniform4iv(glGetUniformLocation(this->programState->programObject, parameterName), 1, &static_cast<ShaderParameter_Integer4*>(parameter)->value.x);
                                break;
                            }


                        // Matrices.
                        case ShaderParameterType_Float2x2:
                            {
                                glUniformMatrix2fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, GL_FALSE, &static_cast<ShaderParameter_Float2x2*>(parameter)->value[0][0]);
                                break;
                            }

                        case ShaderParameterType_Float3x3:
                            {
                                glUniformMatrix3fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, GL_FALSE, &static_cast<ShaderParameter_Float3x3*>(parameter)->value[0][0]);
                                break;
                            }

                        case ShaderParameterType_Float4x4:
                            {
                                glUniformMatrix4fv(glGetUniformLocation(this->programState->programObject, parameterName), 1, GL_FALSE, &static_cast<ShaderParameter_Float4x4*>(parameter)->value[0][0]);
                                break;
                            }
                            
                            
                        case ShaderParameterType_Texture1D:
                        case ShaderParameterType_Texture2D:
                        case ShaderParameterType_Texture3D:
                        case ShaderParameterType_TextureCube:
                        default: break;
                        }
                    }
                }
            }

            if ((this->operationBitfield & SET_TEXTURE_PARAMETER_BIT))
            {
                for (size_t i = 0; i < this->textureParametersToSet.count; ++i)
                {
                    auto  parameterName = this->textureParametersToSet.buffer[i]->key;
                    auto &parameter     = this->textureParametersToSet.buffer[i]->value;

                    bool setUniform = true;
                    bool setBinding = true;

                    // We need to check if the shader state has the texture already set.
                    auto iExistingParameter = this->programState->textures.Find(parameterName);
                    if (iExistingParameter != nullptr)
                    {
                        // The parameter already exists. The uniform does not need to be set, but the texture binding might.
                        setUniform = false;

                        if (iExistingParameter->value.textureObject == parameter.textureObject)
                        {
                            setBinding = false;
                        }
                        else
                        {
                            setBinding = true;
                            iExistingParameter->value.textureObject = parameter.textureObject;

                            parameter.textureUnit     = iExistingParameter->value.textureUnit;
                            parameter.uniformLocation = iExistingParameter->value.uniformLocation;
                        }
                    }
                    else
                    {
                        // The parameter has not been set before. We need to set the uniform and bind.
                        parameter.textureUnit     = static_cast<GLint>(this->programState->textures.count);
                        parameter.uniformLocation = glGetUniformLocation(this->programState->programObject, parameterName);
                        this->programState->textures.Add(parameterName, parameter);
                    }



                    // If this program state is the current one, we'll need to bind the texture straight away.
                    if (static_cast<GLuint>(previousCurrentProgram) == this->programState->programObject && setBinding)
                    {
                        glActiveTexture(GL_TEXTURE0 + parameter.textureUnit);
                        glBindTexture(parameter.textureTarget, *parameter.textureObject);
                    }

                    if (setUniform)
                    {
                        glUniform1i(parameter.uniformLocation, parameter.textureUnit);
                    }
                }
            }



            // Need to rebind the previous program because it's important that we maintain the integrity of the global state.
            if (static_cast<GLuint>(previousCurrentProgram) != this->programState->programObject)
            {
                glUseProgram(static_cast<GLuint>(previousCurrentProgram));
            }
        }
    }
}
