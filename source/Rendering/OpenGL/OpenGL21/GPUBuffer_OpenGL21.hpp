// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUBuffer_OpenGL21_hpp_
#define __GT_GE_Rendering_GPUBuffer_OpenGL21_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTLib/ReferenceCountedObject.hpp>
#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

namespace GT
{
    /// Class representing a buffer on the GPU.
    class GPUBuffer_OpenGL21 : public ReferenceCountedObject
    {
    public:

        /// Constructor.
        GPUBuffer_OpenGL21(GLuint objectGL, GLenum targetGL, GLenum usageGL);

        /// Destructor.
        ~GPUBuffer_OpenGL21();


        /// Retrieves the OpenGL buffer object.
        GLuint GetOpenGLObject() const;

        /// Retrieves the OpenGL bind target.
        GLenum GetOpenGLTarget() const;

        /// Retrieves the OpenGL usage hint.
        GLenum GetOpenGLUsage() const;



    private:

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