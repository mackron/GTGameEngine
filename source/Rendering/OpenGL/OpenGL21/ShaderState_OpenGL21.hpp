// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_ShaderState_OpenGL21
#define GT_Rendering_ShaderState_OpenGL21

#include <GTGE/Math.hpp>
#include <GTGE/Rendering/Renderer.hpp>
#include <GTGE/Core/Dictionary.hpp>
#include <GTGE/Core/Map.hpp>
#include <GTGE/Core/Vector.hpp>
#include <GTGE/Core/String.hpp>
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL21.hpp"


namespace GT
{
    /// Structure containing the state of a shader for use by the OpenGL21 renderer.
    ///
    /// The main reason for this is that we need to somehow manage the activation of texture units as shaders are made current. All comes
    /// down to OpenGL's insane way of handling the passing of textures to shaders.
    ///
    /// We don't keep track of every piece of state here. We only store the state that we need for our own interface.
    struct ShaderState_OpenGL21
    {
        /// The main OpenGL program object.
        GLuint programObject;


        struct Parameter
        {
            Parameter()
                : location(-1)
            {
            }

            Parameter(GLint locationIn)
                : location(locationIn)
            {
            }

            Parameter(const Parameter &other)
                : location(other.location)
            {
            }

            virtual ~Parameter()
            {
            }

            Parameter & operator=(const Parameter &other)
            {
                this->location = other.location;

                return *this;
            }


            GLint location;
        };

        struct FloatParameter : public Parameter
        {
            FloatParameter(float xIn)
                : Parameter(), x(xIn)
            {
            }

            FloatParameter(GLint locationIn, float xIn)
                : Parameter(locationIn), x(xIn)
            {
            }

            GLfloat x;
        };

        struct Float2Parameter : public Parameter
        {
            Float2Parameter(float xIn, float yIn)
                : Parameter(), x(xIn), y(yIn)
            {
            }

            Float2Parameter(GLint locationIn, float xIn, float yIn)
                : Parameter(locationIn), x(xIn), y(yIn)
            {
            }

            GLfloat x;
            GLfloat y;
        };

        struct Float3Parameter : public Parameter
        {
            Float3Parameter(float xIn, float yIn, float zIn)
                : Parameter(), x(xIn), y(yIn), z(zIn)
            {
            }

            Float3Parameter(GLint locationIn, float xIn, float yIn, float zIn)
                : Parameter(locationIn), x(xIn), y(yIn), z(zIn)
            {
            }

            GLfloat x;
            GLfloat y;
            GLfloat z;
        };

        struct Float4Parameter : public Parameter
        {
            Float4Parameter(float xIn, float yIn, float zIn, float wIn)
                : Parameter(), x(xIn), y(yIn), z(zIn), w(wIn)
            {
            }

            Float4Parameter(GLint locationIn, float xIn, float yIn, float zIn, float wIn)
                : Parameter(locationIn), x(xIn), y(yIn), z(zIn), w(wIn)
            {
            }


            GLfloat x;
            GLfloat y;
            GLfloat z;
            GLfloat w;
        };

        struct Float2x2Parameter : public Parameter
        {
            Float2x2Parameter(const glm::mat2 &valueIn)
                : Parameter()
            {
                this->value[0] = valueIn[0][0];
                this->value[1] = valueIn[0][1];

                this->value[2] = valueIn[1][0];
                this->value[3] = valueIn[1][1];
            }

            Float2x2Parameter(GLint locationIn, const glm::mat2 &valueIn)
                : Parameter(locationIn)
            {
                this->value[0] = valueIn[0][0];
                this->value[1] = valueIn[0][1];

                this->value[2] = valueIn[1][0];
                this->value[3] = valueIn[1][1];
            }

            GLfloat value[4];
        };

        struct Float3x3Parameter : public Parameter
        {
            Float3x3Parameter(const glm::mat3 &valueIn)
                : Parameter()
            {
                this->value[0] = valueIn[0][0];
                this->value[1] = valueIn[0][1];
                this->value[2] = valueIn[0][2];

                this->value[3] = valueIn[1][0];
                this->value[4] = valueIn[1][1];
                this->value[5] = valueIn[1][2];

                this->value[6] = valueIn[2][0];
                this->value[7] = valueIn[2][1];
                this->value[8] = valueIn[2][2];
            }

            Float3x3Parameter(GLint locationIn, const glm::mat3 &valueIn)
                : Parameter(locationIn)
            {
                this->value[0] = valueIn[0][0];
                this->value[1] = valueIn[0][1];
                this->value[2] = valueIn[0][2];

                this->value[3] = valueIn[1][0];
                this->value[4] = valueIn[1][1];
                this->value[5] = valueIn[1][2];

                this->value[6] = valueIn[2][0];
                this->value[7] = valueIn[2][1];
                this->value[8] = valueIn[2][2];
            }

            GLfloat value[9];
        };

        struct Float4x4Parameter : public Parameter
        {
            Float4x4Parameter(const glm::mat4 &valueIn)
                : Parameter()
            {
                this->value[0]  = valueIn[0][0];
                this->value[1]  = valueIn[0][1];
                this->value[2]  = valueIn[0][2];
                this->value[3]  = valueIn[0][3];

                this->value[4]  = valueIn[1][0];
                this->value[5]  = valueIn[1][1];
                this->value[6]  = valueIn[1][2];
                this->value[7]  = valueIn[1][3];

                this->value[8]  = valueIn[2][0];
                this->value[9]  = valueIn[2][1];
                this->value[10] = valueIn[2][2];
                this->value[11] = valueIn[2][3];

                this->value[12] = valueIn[3][0];
                this->value[13] = valueIn[3][1];
                this->value[14] = valueIn[3][2];
                this->value[15] = valueIn[3][3];
            }

            Float4x4Parameter(GLint locationIn, const glm::mat4 &valueIn)
                : Parameter(locationIn)
            {
                this->value[0]  = valueIn[0][0];
                this->value[1]  = valueIn[0][1];
                this->value[2]  = valueIn[0][2];
                this->value[3]  = valueIn[0][3];

                this->value[4]  = valueIn[1][0];
                this->value[5]  = valueIn[1][1];
                this->value[6]  = valueIn[1][2];
                this->value[7]  = valueIn[1][3];

                this->value[8]  = valueIn[2][0];
                this->value[9]  = valueIn[2][1];
                this->value[10] = valueIn[2][2];
                this->value[11] = valueIn[2][3];

                this->value[12] = valueIn[3][0];
                this->value[13] = valueIn[3][1];
                this->value[14] = valueIn[3][2];
                this->value[15] = valueIn[3][3];
            }

            GLfloat value[16];
        };


        /// Structure containing information about a texture being used by the shader.
        struct TextureParameter : public Parameter
        {
            TextureParameter(TextureState_OpenGL21* textureStateIn, GLenum textureTargetIn, GLint textureUnitIn = 0)
                : Parameter(), textureState(textureStateIn), textureTarget(textureTargetIn), textureUnit(textureUnitIn)
            {
            }

            TextureParameter(GLint locationIn, TextureState_OpenGL21* textureStateIn, GLenum textureTargetIn, GLint textureUnitIn = 0)
                : Parameter(locationIn), textureState(textureStateIn), textureTarget(textureTargetIn), textureUnit(textureUnitIn)
            {
            }

            TextureParameter(const TextureParameter &other)
                : Parameter(other), textureState(other.textureState), textureTarget(other.textureTarget), textureUnit(other.textureUnit)
            {
            }

            TextureParameter & operator=(const TextureParameter &other)
            {
                Parameter::operator=(other);

                this->textureState  = other.textureState;
                this->textureTarget = other.textureTarget;
                this->textureUnit   = other.textureUnit;

                return *this;
            }



            /// A pointer to the relevant texture state.
            TextureState_OpenGL21* textureState;

            /// The texture target. Needed for binding.
            GLenum textureTarget;

            /// The texture unit to bind the texture on.
            GLint textureUnit;
        };

        /// The list of texture parameters currently set on the shader on the server side. Keyed by the location of the uniform.
        Map<GLint, TextureParameter> currentTextureUniforms;



        // The locations of every parameter used by the shader. These are intentionally separated into types for faster searching.
        Dictionary<GLint> floatUniformLocations;
        Dictionary<GLint> float2UniformLocations;
        Dictionary<GLint> float3UniformLocations;
        Dictionary<GLint> float4UniformLocations;
        Dictionary<GLint> float2x2UniformLocations;
        Dictionary<GLint> float3x3UniformLocations;
        Dictionary<GLint> float4x4UniformLocations;
        Dictionary<GLint> textureUniformLocations;


        // Parameters that have changed and need to be applied on the server side.
        Vector<FloatParameter>    pendingFloatUniformsByLocation;
        Vector<Float2Parameter>   pendingFloat2UniformsByLocation;
        Vector<Float3Parameter>   pendingFloat3UniformsByLocation;
        Vector<Float4Parameter>   pendingFloat4UniformsByLocation;
        Vector<Float2x2Parameter> pendingFloat2x2UniformsByLocation;
        Vector<Float3x3Parameter> pendingFloat3x3UniformsByLocation;
        Vector<Float4x4Parameter> pendingFloat4x4UniformsByLocation;
        Vector<TextureParameter>  pendingTextureUniformsByLocation;


        // Parameters that were set before the shader has had a chance to allocate storage for the parameters.
        Dictionary<FloatParameter>    pendingFloatUniformsByName;
        Dictionary<Float2Parameter>   pendingFloat2UniformsByName;
        Dictionary<Float3Parameter>   pendingFloat3UniformsByName;
        Dictionary<Float4Parameter>   pendingFloat4UniformsByName;
        Dictionary<Float2x2Parameter> pendingFloat2x2UniformsByName;
        Dictionary<Float3x3Parameter> pendingFloat3x3UniformsByName;
        Dictionary<Float4x4Parameter> pendingFloat4x4UniformsByName;
        Dictionary<TextureParameter>  pendingTextureUniformsByName;


        /// Constructor.
        ShaderState_OpenGL21()
            : programObject(0), currentTextureUniforms(),
              floatUniformLocations(),
              float2UniformLocations(),
              float3UniformLocations(),
              float4UniformLocations(),
              float2x2UniformLocations(),
              float3x3UniformLocations(),
              float4x4UniformLocations(),
              textureUniformLocations(),
              pendingFloatUniformsByLocation(),
              pendingFloat2UniformsByLocation(),
              pendingFloat3UniformsByLocation(),
              pendingFloat4UniformsByLocation(),
              pendingFloat2x2UniformsByLocation(),
              pendingFloat3x3UniformsByLocation(),
              pendingFloat4x4UniformsByLocation(),
              pendingTextureUniformsByLocation(),
              pendingFloatUniformsByName(),
              pendingFloat2UniformsByName(),
              pendingFloat3UniformsByName(),
              pendingFloat4UniformsByName(),
              pendingFloat2x2UniformsByName(),
              pendingFloat3x3UniformsByName(),
              pendingFloat4x4UniformsByName(),
              pendingTextureUniformsByName()
        {
        }

        /// Destructor.
        ~ShaderState_OpenGL21()
        {
            // Textures need to be notified.
            for (size_t iTextureUniform = 0; iTextureUniform < this->currentTextureUniforms.count; ++iTextureUniform)
            {
                this->currentTextureUniforms.buffer[iTextureUniform]->value.textureState->shaders.RemoveFirstOccuranceOf(this);
            }
        }


        /// Clears all pending uniforms.
        void ClearPendingUniforms()
        {
            this->pendingFloatUniformsByLocation.Clear();
            this->pendingFloat2UniformsByLocation.Clear();
            this->pendingFloat3UniformsByLocation.Clear();
            this->pendingFloat4UniformsByLocation.Clear();
            this->pendingFloat2x2UniformsByLocation.Clear();
            this->pendingFloat3x3UniformsByLocation.Clear();
            this->pendingFloat4x4UniformsByLocation.Clear();
            this->pendingTextureUniformsByLocation.Clear();

            this->pendingFloatUniformsByName.Clear();
            this->pendingFloat2UniformsByName.Clear();
            this->pendingFloat3UniformsByName.Clear();
            this->pendingFloat4UniformsByName.Clear();
            this->pendingFloat2x2UniformsByName.Clear();
            this->pendingFloat3x3UniformsByName.Clear();
            this->pendingFloat4x4UniformsByName.Clear();
            this->pendingTextureUniformsByName.Clear();
        }


        /// Retrieves the location of a uniform variables.
        ///
        /// @remarks
        ///     If the object has not yet been created or the uniform doesn't exist, -1 will be returned.
        GLint GetFloatUniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->floatUniformLocations);
        }

        GLint GetFloat2UniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->float2UniformLocations);
        }

        GLint GetFloat3UniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->float3UniformLocations);
        }

        GLint GetFloat4UniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->float4UniformLocations);
        }


        GLint GetFloat2x2UniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->float2x2UniformLocations);
        }

        GLint GetFloat3x3UniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->float3x3UniformLocations);
        }

        GLint GetFloat4x4UniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->float4x4UniformLocations);
        }


        GLint GetTextureUniformLocation(const char* name)
        {
            return this->GetUniformLocation(name, this->textureUniformLocations);
        }


        /// Retrieves a pointer to the texture uniform at the given texture unit.
        const TextureParameter* FindTextureUniformByUnit(GLint unit) const
        {
            for (size_t i = 0; i < this->currentTextureUniforms.count; ++i)
            {
                auto &uniform = this->currentTextureUniforms.buffer[i]->value;
                {
                    if (uniform.textureUnit == unit)
                    {
                        return &uniform;
                    }
                }
            }

            return nullptr;
        }

        /// Finds an available texture unit.
        ///
        /// @remarks
        ///     This will return -1 if no texture unit is available.
        GLint FindAvailableTextureUnit()
        {
            for (GLuint i = 0; i < Renderer::GetMaxTextureUnits(); ++i)
            {
                if (this->FindTextureUniformByUnit(static_cast<GLint>(i)) == nullptr)
                {
                    return i;
                }
            }

            return -1;
        }


    private:

        /// Generic function for retrieving a uniform location from the given list.
        GLint GetUniformLocation(const char* name, const Dictionary<GLint> &uniformLocations)
        {
            if (this->programObject != 0)
            {
                auto iLocation = uniformLocations.Find(name);
                if (iLocation != nullptr)
                {
                    return iLocation->value;
                }
            }

            return -1;
        }



    private:    // No copying.
        ShaderState_OpenGL21(const ShaderState_OpenGL21 &);
        ShaderState_OpenGL21 & operator=(const ShaderState_OpenGL21 &);
    };
}

#endif
