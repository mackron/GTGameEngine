// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUBuffer_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)

namespace GT
{
    GPUBuffer_OpenGL21::GPUBuffer_OpenGL21(GLuint objectGL, GLenum targetGL, GLenum usageGL)
        : ReferenceCountedObject(),
          m_objectGL(objectGL),
          m_targetGL(targetGL),
          m_usageGL(usageGL)
    {
    }

    GPUBuffer_OpenGL21::~GPUBuffer_OpenGL21()
    {
    }


    GLuint GPUBuffer_OpenGL21::GetOpenGLObject() const
    {
        return m_objectGL;
    }

    GLenum GPUBuffer_OpenGL21::GetOpenGLTarget() const
    {
        return m_targetGL;
    }

    GLenum GPUBuffer_OpenGL21::GetOpenGLUsage() const
    {
        return m_usageGL;
    }
}

#endif
