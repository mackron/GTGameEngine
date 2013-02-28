// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Shader_OpenGL33.hpp"

namespace GTEngine
{
    Shader_OpenGL33::Shader_OpenGL33(GLuint* objectGLIn, const char* vertexShader, const char* fragmentShader, const char* geometryShader)
        : Shader(vertexShader, fragmentShader, geometryShader), objectGL(objectGLIn)
    {
    }

    Shader_OpenGL33::~Shader_OpenGL33()
    {
    }


    GLuint* Shader_OpenGL33::GetOpenGLObjectPtr() const
    {
        return this->objectGL;
    }
}