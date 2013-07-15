// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Shader_OpenGL21.hpp"
#include "Texture2D_OpenGL21.hpp"
#include "TextureCube_OpenGL21.hpp"

namespace GTEngine
{
    Shader_OpenGL21::Shader_OpenGL21(ShaderState_OpenGL21* stateGLIn, const char* vertexShader, const char* fragmentShader, const char* geometryShader)
        : Shader(vertexShader, fragmentShader, geometryShader), stateGL(stateGLIn)
    {
    }

    Shader_OpenGL21::~Shader_OpenGL21()
    {
    }


    ShaderState_OpenGL21* Shader_OpenGL21::GetOpenGLState() const
    {
        return this->stateGL;
    }



    //////////////////////////////////////////////
    // Uniforms.
    //
    // The way uniforms are set here is that if the program object is not yet created it is pushed into the 'initial' uniform lists. Otherwise, they
    // are pushed into the 'pending' uniform lists. Not sure if that's good terminology, but it'll do. The reason this distinction exists is because
    // the actual creation of the object will be delayed until the next call cache swap. Optimizations are done after the object is created to make
    // things a little more efficient.

    void Shader_OpenGL21::SetUniform(const char* name, float x)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloatUniformsByName.Add(name, ShaderState_OpenGL21::FloatParameter(x));
        }
        else
        {
            GLint location = this->stateGL->GetFloatUniformLocation(name);
            if (location != -1)
            {
                this->stateGL->pendingFloatUniformsByLocation.PushBack(ShaderState_OpenGL21::FloatParameter(location, x));
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, float x, float y)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat2UniformsByName.Add(name, ShaderState_OpenGL21::Float2Parameter(x, y));
        }
        else
        {
            GLint location = this->stateGL->GetFloat2UniformLocation(name);
            if (location != -1)
            {
                this->stateGL->pendingFloat2UniformsByLocation.PushBack(ShaderState_OpenGL21::Float2Parameter(location, x, y));
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, float x, float y, float z)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat3UniformsByName.Add(name, ShaderState_OpenGL21::Float3Parameter(x, y, z));
        }
        else
        {
            GLint location = this->stateGL->GetFloat3UniformLocation(name);
            if (location != -1)
            {
                this->stateGL->pendingFloat3UniformsByLocation.PushBack(ShaderState_OpenGL21::Float3Parameter(location, x, y, z));
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, float x, float y, float z, float w)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat4UniformsByName.Add(name, ShaderState_OpenGL21::Float4Parameter(x, y, z, w));
        }
        else
        {
            GLint location = this->stateGL->GetFloat4UniformLocation(name);
            if (location != -1)
            {
                this->stateGL->pendingFloat4UniformsByLocation.PushBack(ShaderState_OpenGL21::Float4Parameter(location, x, y, z, w));
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, const glm::mat2 &value)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat2x2UniformsByName.Add(name, ShaderState_OpenGL21::Float2x2Parameter(value));
        }
        else
        {
            GLint location = this->stateGL->GetFloat2x2UniformLocation(name);
            if (location != -1)
            {
                this->stateGL->pendingFloat2x2UniformsByLocation.PushBack(ShaderState_OpenGL21::Float2x2Parameter(location, value));
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, const glm::mat3 &value)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat3x3UniformsByName.Add(name, ShaderState_OpenGL21::Float3x3Parameter(value));
        }
        else
        {
            GLint location = this->stateGL->GetFloat3x3UniformLocation(name);
            if (location != -1)
            {
                this->stateGL->pendingFloat3x3UniformsByLocation.PushBack(ShaderState_OpenGL21::Float3x3Parameter(location, value));
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, const glm::mat4 &value)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat4x4UniformsByName.Add(name, ShaderState_OpenGL21::Float4x4Parameter(value));
        }
        else
        {
            GLint location = this->stateGL->GetFloat4x4UniformLocation(name);
            if (location != -1)
            {
                this->stateGL->pendingFloat4x4UniformsByLocation.PushBack(ShaderState_OpenGL21::Float4x4Parameter(location, value));
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, const Texture2D* value)
    {
        assert(value != nullptr);
        {
            auto textureGL33 = static_cast<const Texture2D_OpenGL21*>(value);
            auto   textureState  = textureGL33->GetOpenGLState();
            GLenum textureTarget = textureGL33->GetTarget();


            if (this->stateGL->programObject == 0)
            {
                this->stateGL->pendingTextureUniformsByName.Add(name, ShaderState_OpenGL21::TextureParameter(textureState, textureTarget));
            }
            else
            {
                GLint location = this->stateGL->GetTextureUniformLocation(name);
                if (location != -1)
                {
                    this->stateGL->pendingTextureUniformsByLocation.PushBack(ShaderState_OpenGL21::TextureParameter(location, textureState, textureTarget));
                }
            }
        }
    }

    void Shader_OpenGL21::SetUniform(const char* name, const TextureCube* value)
    {
        assert(value != nullptr);
        {
            auto textureGL33 = static_cast<const TextureCube_OpenGL21*>(value);
            auto   textureState  = textureGL33->GetOpenGLState();
            GLenum textureTarget = GL_TEXTURE_CUBE_MAP;


            if (this->stateGL->programObject == 0)
            {
                this->stateGL->pendingTextureUniformsByName.Add(name, ShaderState_OpenGL21::TextureParameter(textureState, textureTarget));
            }
            else
            {
                GLint location = this->stateGL->GetTextureUniformLocation(name);
                if (location != -1)
                {
                    this->stateGL->pendingTextureUniformsByLocation.PushBack(ShaderState_OpenGL21::TextureParameter(location, textureState, textureTarget));
                }
            }
        }
    }
}