// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUShaderProgram_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)

namespace GT
{
    GPUShaderProgram_OpenGL21::GPUShaderProgram_OpenGL21(GLuint objectGL)
        : ReferenceCountedObject(), m_objectGL(objectGL)
    {
    }

    GPUShaderProgram_OpenGL21::~GPUShaderProgram_OpenGL21()
    {
    }


    GLuint GPUShaderProgram_OpenGL21::GetOpenGLObject() const
    {
        return m_objectGL;
    }
}

#endif
