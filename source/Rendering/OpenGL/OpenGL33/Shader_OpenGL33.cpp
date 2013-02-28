// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Shader_OpenGL33.hpp"

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
}