// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCCreateShader.hpp"
#include <GTEngine/Logging.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCCreateShader::RCCreateShader()
        : programState(nullptr), vertexShaderSource(), fragmentShaderSource(), geometryShaderSource()
    {
    }


    void RCCreateShader::CreateShader(ShaderState_OpenGL33* programStateIn, const char* vertexShaderSourceIn, const char* fragmentShaderSourceIn, const char* geometryShaderSourceIn)
    {
        this->programState = programStateIn;

        this->vertexShaderSource   = vertexShaderSourceIn;
        this->fragmentShaderSource = fragmentShaderSourceIn;

        if (geometryShaderSourceIn != nullptr)
        {
            this->geometryShaderSource = geometryShaderSourceIn;
        }
    }


    void RCCreateShader::Execute()
    {
        // 1) Create the shader objects (vertex, fragment and geometry shader objects).
        GLuint vertexShaderObject   = this->CreateShader(GL_VERTEX_SHADER,   this->vertexShaderSource.c_str());
        GLuint fragmentShaderObject = this->CreateShader(GL_FRAGMENT_SHADER, this->fragmentShaderSource.c_str());
        GLuint geometryShaderObject = 0;
        if (!this->geometryShaderSource.IsEmpty())
        {
            geometryShaderObject = this->CreateShader(GL_GEOMETRY_SHADER, this->geometryShaderSource.c_str());
        }


        // 2) Link the program.
        this->programState->programObject = this->LinkProgram(vertexShaderObject, fragmentShaderObject, geometryShaderObject);


        // 3) Grab the uniforms.
        if (this->programState->programObject != 0)
        {
            GLint uniformCount = 0;
            glGetProgramiv(this->programState->programObject, GL_ACTIVE_UNIFORMS, &uniformCount);


            for (GLint iUniform = 0; iUniform < uniformCount; ++iUniform)
            {
                GLint  uniformSize;
                GLenum uniformType;
                char   uniformName[100];
                glGetActiveUniform(this->programState->programObject, iUniform, sizeof(uniformName) - 1, nullptr, &uniformSize, &uniformType, uniformName);


                switch (uniformType)
                {
                case GL_FLOAT:
                    {
                        this->programState->floatUniformLocations.Add(uniformName, iUniform);
                        break;
                    }

                case GL_FLOAT_VEC2:
                    {
                        this->programState->float2UniformLocations.Add(uniformName, iUniform);
                        break;
                    }

                case GL_FLOAT_VEC3:
                    {
                        this->programState->float3UniformLocations.Add(uniformName, iUniform);
                        break;
                    }

                case GL_FLOAT_VEC4:
                    {
                        this->programState->float4UniformLocations.Add(uniformName, iUniform);
                        break;
                    }


                case GL_FLOAT_MAT2:
                    {
                        this->programState->float2x2UniformLocations.Add(uniformName, iUniform);
                        break;
                    }

                case GL_FLOAT_MAT3:
                    {
                        this->programState->float3x3UniformLocations.Add(uniformName, iUniform);
                        break;
                    }

                case GL_FLOAT_MAT4:
                    {
                        this->programState->float4x4UniformLocations.Add(uniformName, iUniform);
                        break;
                    }


                case GL_SAMPLER_1D:
                case GL_SAMPLER_1D_ARRAY:
                case GL_SAMPLER_2D:
                case GL_SAMPLER_2D_ARRAY:
                case GL_SAMPLER_3D:
                case GL_SAMPLER_CUBE:
                case GL_SAMPLER_2D_MULTISAMPLE:
                case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
                case GL_SAMPLER_BUFFER:
                case GL_INT_SAMPLER_1D:
                case GL_INT_SAMPLER_1D_ARRAY:
                case GL_INT_SAMPLER_2D:
                case GL_INT_SAMPLER_2D_ARRAY:
                case GL_INT_SAMPLER_3D:
                case GL_INT_SAMPLER_CUBE:
                case GL_INT_SAMPLER_BUFFER:
                case GL_UNSIGNED_INT_SAMPLER_1D:
                case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
                case GL_UNSIGNED_INT_SAMPLER_2D:
                case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
                case GL_UNSIGNED_INT_SAMPLER_3D:
                case GL_UNSIGNED_INT_SAMPLER_CUBE:
                case GL_UNSIGNED_INT_SAMPLER_BUFFER:
                    {
                        this->programState->textureUniformLocations.Add(uniformName, iUniform);
                        break;
                    }


                default:
                    {
                        break;
                    }
                }
            }
        }


        // 4) Delete the shader objects.
        if (vertexShaderObject   != 0) glDeleteShader(vertexShaderObject);
        if (fragmentShaderObject != 0) glDeleteShader(fragmentShaderObject);
        if (geometryShaderObject != 0) glDeleteShader(geometryShaderObject);
    }



    ////////////////////////////////////////////////////
    // Private

    GLuint RCCreateShader::CreateShader(GLenum type, const GTCore::String &sourceIn)
    {
        auto source       = sourceIn.c_str();
        auto sourceLength = static_cast<GLint>(sourceIn.GetLengthInTs());

        auto shaderObject = glCreateShader(type);
        glShaderSource(shaderObject, 1, &source, &sourceLength);
        glCompileShader(shaderObject);


        // Here we need to log any details.
        GLint compileStatus;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint shaderType;
            glGetShaderiv(shaderObject, GL_SHADER_TYPE, &shaderType);

            GLint logLength;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLength);

            auto log = static_cast<char*>(malloc(static_cast<size_t>(logLength)));
            glGetShaderInfoLog(shaderObject, logLength, nullptr, log);


            GTCore::String title;
            if (shaderType == GL_VERTEX_SHADER)
            {
                title = "Vertex Shader Info Log";
            }
            else if (shaderType == GL_FRAGMENT_SHADER)
            {
                title = "Fragment Shader Info Log";
            }
            else
            {
                title = "Geometry Shader Info Log";
            }

            GTEngine::Log("--- %s ---\n%s\n%s", title.c_str(), log, source);


            glDeleteShader(shaderObject);
            shaderObject = 0;

            free(log);
        }


        return shaderObject;
    }

    GLuint RCCreateShader::LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
    {
        // We'll need a program object.
        GLuint program = glCreateProgram();


        // We need to have concretely defined vertex attributes for OpenGL 2.0 GLSL since we don't really have much control of vertex attributes
        // from inside the shader code. Thus, we're going to have to use hard coded attributes names. Later on we might make this configurable
        // via the shader library or a config file.
        glBindAttribLocation(program, 0, "VertexInput_Position");
        glBindAttribLocation(program, 1, "VertexInput_TexCoord");
        glBindAttribLocation(program, 2, "VertexInput_Normal");
        glBindAttribLocation(program, 3, "VertexInput_Tangent");
        glBindAttribLocation(program, 4, "VertexInput_Bitangent");
        glBindAttribLocation(program, 5, "VertexInput_Colour");


        // Finally we reattach the shaders, link the program and check for errors.
        if (vertexShader   != 0) glAttachShader(program, vertexShader);
        if (fragmentShader != 0) glAttachShader(program, fragmentShader);
        if (geometryShader != 0) glAttachShader(program, geometryShader);

        glLinkProgram(program);


        // Check for link errors.
        GLint linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus == GL_FALSE)
        {
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            auto log = static_cast<char*>(malloc(static_cast<size_t>(logLength)));
            glGetProgramInfoLog(program, logLength, nullptr, log);

            GTEngine::Log("--- Program Link Status ---\n%s", log);

            free(log);

            glDeleteProgram(program);
            program = 0;
        }


        return program;
    }
}
