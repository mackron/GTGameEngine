// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUBuffer_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)

namespace GT
{
    GPUBuffer_OpenGL21::GPUBuffer_OpenGL21(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, GLuint objectGL, GLenum targetGL, GLenum usageGL)
        : m_type(type),
            m_usage(usage),
            m_cpuAccessFlags(cpuAccessFlags),
            m_objectGL(objectGL),
            m_targetGL(targetGL),
            m_usageGL(usageGL)
    {
    }

    GPUBuffer_OpenGL21::~GPUBuffer_OpenGL21()
    {
    }


    GPUBufferType GPUBuffer_OpenGL21::GetBufferType() const
    {
        return m_type;
    }

    GPUBufferUsage GPUBuffer_OpenGL21::GetBufferUsage() const
    {
        return m_usage;
    }

    GPUBufferCPUAccessFlags GPUBuffer_OpenGL21::GetBufferCPUAccessFlags() const
    {
        return m_cpuAccessFlags;
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
