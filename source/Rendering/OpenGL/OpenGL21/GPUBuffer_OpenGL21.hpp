// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUBuffer_OpenGL21_hpp_
#define __GT_GE_Rendering_GPUBuffer_OpenGL21_hpp_

#include <GTGameEngine/Rendering/GPUBuffer.hpp>
#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

#if defined(GT_GE_BUILD_OPENGL21)

namespace GT
{
    /// Class representing a buffer on the GPU.
    class GPUBuffer_OpenGL21 : public GPUBuffer
    {
    public:

        /// Constructor.
        GPUBuffer_OpenGL21(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, GLuint objectGL, GLenum targetGL, GLenum usageGL);

        /// Destructor.
        ~GPUBuffer_OpenGL21();

            
        /// GPUBuffer::GetBufferType().
        GPUBufferType GetBufferType() const;

        /// GPUBuffer::GetBufferUsage().
        GPUBufferUsage GetBufferUsage() const;

        /// GPUBuffer::GetBufferCPUAccessFlags().
        GPUBufferCPUAccessFlags GetBufferCPUAccessFlags() const;



        /// Retrieves the OpenGL buffer object.
        GLuint GetOpenGLObject() const;

        /// Retrieves the OpenGL bind target.
        GLenum GetOpenGLTarget() const;

        /// Retrieves the OpenGL usage hint.
        GLenum GetOpenGLUsage() const;



    private:

        /// The buffer type.
        GPUBufferType m_type;

        /// The buffer usage.
        GPUBufferUsage m_usage;

        /// The buffer CPU access flags.
        GPUBufferCPUAccessFlags m_cpuAccessFlags;


        /// The OpenGL buffer object.
        GLuint m_objectGL;

        /// The OpenGL target for glBindBuffer(), etc.
        GLenum m_targetGL;

        /// The OpenGL usage flags for glBufferData().
        GLenum m_usageGL;
    };
}
#endif

#endif