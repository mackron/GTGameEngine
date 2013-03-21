// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_ShaderState_OpenGL33_hpp_
#define __GTEngine_Rendering_ShaderState_OpenGL33_hpp_

#include <GTEngine/Math.hpp>
#include <GTCore/Dictionary.hpp>
#include <GTCore/Map.hpp>
#include <GTCore/Vector.hpp>
#include <GTCore/String.hpp>
#include <gtgl/gtgl.h>


namespace GTEngine
{
    /// Structure containing the state of a shader for use by the OpenGL33 renderer.
    ///
    /// The main reason for this is that we need to somehow manage the activation of texture units as shaders are made current. All comes
    /// down to OpenGL's insane way of handling the passing of textures to shaders.
    ///
    /// We don't keep track of every piece of state here. We only store the state that we need for our own interface.
    struct ShaderState_OpenGL33
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
            TextureParameter(GLuint* textureObjectIn, GLenum textureTargetIn, GLint textureUnitIn = 0)
                : Parameter(), textureObject(textureObjectIn), textureTarget(textureTargetIn), textureUnit(textureUnitIn)
            {
            }

            TextureParameter(GLint locationIn, GLuint* textureObjectIn, GLenum textureTargetIn, GLint textureUnitIn = 0)
                : Parameter(locationIn), textureObject(textureObjectIn), textureTarget(textureTargetIn), textureUnit(textureUnitIn)
            {
            }



            /// A pointer to the relevant texture object.
            GLuint* textureObject;

            /// The texture target. Needed for binding.
            GLenum textureTarget;

            /// The texture unit to bind the texture on.
            GLint textureUnit;
        };

        /// The list of texture parameters currently set on the shader on the server side. Keyed by the location of the uniform.
        GTCore::Map<GLint, TextureParameter> currentTextureUniforms;



        // The locations of every parameter used by the shader. These are intentionally separated into types for faster searching.
        GTCore::Dictionary<GLint> floatUniformLocations;
        GTCore::Dictionary<GLint> float2UniformLocations;
        GTCore::Dictionary<GLint> float3UniformLocations;
        GTCore::Dictionary<GLint> float4UniformLocations;
        GTCore::Dictionary<GLint> float2x2UniformLocations;
        GTCore::Dictionary<GLint> float3x3UniformLocations;
        GTCore::Dictionary<GLint> float4x4UniformLocations;
        GTCore::Dictionary<GLint> textureUniformLocations;


        // Parameters that have changed and need to be applied on the server side.
        GTCore::Vector<FloatParameter>    pendingFloatUniformsByLocation;
        GTCore::Vector<Float2Parameter>   pendingFloat2UniformsByLocation;
        GTCore::Vector<Float3Parameter>   pendingFloat3UniformsByLocation;
        GTCore::Vector<Float4Parameter>   pendingFloat4UniformsByLocation;
        GTCore::Vector<Float2x2Parameter> pendingFloat2x2UniformsByLocation;
        GTCore::Vector<Float3x3Parameter> pendingFloat3x3UniformsByLocation;
        GTCore::Vector<Float4x4Parameter> pendingFloat4x4UniformsByLocation;
        GTCore::Vector<TextureParameter>  pendingTextureUniformsByLocation;


        // Parameters that were set before the shader has had a chance to allocate storage for the parameters.
        GTCore::Dictionary<FloatParameter>    pendingFloatUniformsByName;
        GTCore::Dictionary<Float2Parameter>   pendingFloat2UniformsByName;
        GTCore::Dictionary<Float3Parameter>   pendingFloat3UniformsByName;
        GTCore::Dictionary<Float4Parameter>   pendingFloat4UniformsByName;
        GTCore::Dictionary<Float2x2Parameter> pendingFloat2x2UniformsByName;
        GTCore::Dictionary<Float3x3Parameter> pendingFloat3x3UniformsByName;
        GTCore::Dictionary<Float4x4Parameter> pendingFloat4x4UniformsByName;
        GTCore::Dictionary<TextureParameter>  pendingTextureUniformsByName;

        
        /// Constructor.
        ShaderState_OpenGL33()
            : programObject(0), currentTextureUniforms()
        {
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


        
    private:
        
        /// Generic function for retrieving a uniform location from the given list.
        GLint GetUniformLocation(const char* name, const GTCore::Dictionary<GLint> &uniformLocations)
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
    };
}

#endif