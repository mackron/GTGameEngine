// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Shader_OpenGL33.hpp"
#include "Texture2D_OpenGL33.hpp"
#include "TextureCube_OpenGL33.hpp"

namespace GTEngine
{
    Shader_OpenGL33::Shader_OpenGL33(ShaderState_OpenGL33* stateGLIn, const char* vertexShader, const char* fragmentShader, const char* geometryShader)
        : Shader(vertexShader, fragmentShader, geometryShader), stateGL(stateGLIn)
    {
    }

    Shader_OpenGL33::~Shader_OpenGL33()
    {
    }


    ShaderState_OpenGL33* Shader_OpenGL33::GetOpenGLState() const
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

    void Shader_OpenGL33::SetUniform(const char* name, float x)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloatUniformsByName.Add(name, ShaderState_OpenGL33::FloatParameter(x));
        }
        else
        {
            this->stateGL->pendingFloatUniformsByLocation.PushBack(ShaderState_OpenGL33::FloatParameter(this->stateGL->GetFloatUniformLocation(name), x));
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, float x, float y)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat2UniformsByName.Add(name, ShaderState_OpenGL33::Float2Parameter(x, y));
        }
        else
        {
            this->stateGL->pendingFloat2UniformsByLocation.PushBack(ShaderState_OpenGL33::Float2Parameter(this->stateGL->GetFloat2UniformLocation(name), x, y));
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, float x, float y, float z)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat3UniformsByName.Add(name, ShaderState_OpenGL33::Float3Parameter(x, y, z));
        }
        else
        {
            this->stateGL->pendingFloat3UniformsByLocation.PushBack(ShaderState_OpenGL33::Float3Parameter(this->stateGL->GetFloat3UniformLocation(name), x, y, z));
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, float x, float y, float z, float w)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat4UniformsByName.Add(name, ShaderState_OpenGL33::Float4Parameter(x, y, z, w));
        }
        else
        {
            this->stateGL->pendingFloat4UniformsByLocation.PushBack(ShaderState_OpenGL33::Float4Parameter(this->stateGL->GetFloat4UniformLocation(name), x, y, z, w));
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, const glm::mat2 &value)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat2x2UniformsByName.Add(name, ShaderState_OpenGL33::Float2x2Parameter(value));
        }
        else
        {
            this->stateGL->pendingFloat2x2UniformsByLocation.PushBack(ShaderState_OpenGL33::Float2x2Parameter(this->stateGL->GetFloat2x2UniformLocation(name), value));
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, const glm::mat3 &value)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat3x3UniformsByName.Add(name, ShaderState_OpenGL33::Float3x3Parameter(value));
        }
        else
        {
            this->stateGL->pendingFloat3x3UniformsByLocation.PushBack(ShaderState_OpenGL33::Float3x3Parameter(this->stateGL->GetFloat3x3UniformLocation(name), value));
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, const glm::mat4 &value)
    {
        if (this->stateGL->programObject == 0)
        {
            this->stateGL->pendingFloat4x4UniformsByName.Add(name, ShaderState_OpenGL33::Float4x4Parameter(value));
        }
        else
        {
            this->stateGL->pendingFloat4x4UniformsByLocation.PushBack(ShaderState_OpenGL33::Float4x4Parameter(this->stateGL->GetFloat4x4UniformLocation(name), value));
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, const Texture2D* value)
    {
        assert(value != nullptr);
        {
            auto textureGL33 = static_cast<const Texture2D_OpenGL33*>(value);
            GLuint* textureObject = textureGL33->GetOpenGLObjectPtr();
            GLenum  textureTarget = textureGL33->GetTarget();


            if (this->stateGL->programObject == 0)
            {
                this->stateGL->pendingTextureUniformsByName.Add(name, ShaderState_OpenGL33::TextureParameter(textureObject, textureTarget));
            }
            else
            {
                this->stateGL->pendingTextureUniformsByLocation.PushBack(ShaderState_OpenGL33::TextureParameter(this->stateGL->GetTextureUniformLocation(name), textureObject, textureTarget));
            }
        }
    }

    void Shader_OpenGL33::SetUniform(const char* name, const TextureCube* value)
    {
        assert(value != nullptr);
        {
            auto textureGL33 = static_cast<const TextureCube_OpenGL33*>(value);
            GLuint* textureObject = textureGL33->GetOpenGLObjectPtr();
            GLenum  textureTarget = GL_TEXTURE_CUBE_MAP;


            if (this->stateGL->programObject == 0)
            {
                this->stateGL->pendingTextureUniformsByName.Add(name, ShaderState_OpenGL33::TextureParameter(textureObject, textureTarget));
            }
            else
            {
                this->stateGL->pendingTextureUniformsByLocation.PushBack(ShaderState_OpenGL33::TextureParameter(this->stateGL->GetTextureUniformLocation(name), textureObject, textureTarget));
            }
        }
    }
}