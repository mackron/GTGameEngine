// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUShaderProgram_OpenGL21_hpp_
#define __GT_GE_GPUShaderProgram_OpenGL21_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL21)

#include <GTGameEngine/Rendering/GPUShaderProgram.hpp>
#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

namespace GT
{
    /// OpenGL 2.1 implementation of the GPUShaderProgram class.
    class GPUShaderProgram_OpenGL21 : public GPUShaderProgram
    {
    public:

        /// Constructor.
        GPUShaderProgram_OpenGL21(GLuint objectGL);

        /// Destructor.
        ~GPUShaderProgram_OpenGL21();


        /// Retrieves the OpenGL object.
        GLuint GetOpenGLObject() const;



    private:

        /// The OpenGL program object.
        GLuint m_objectGL;




    private:    // No copying.
        GPUShaderProgram_OpenGL21(const GPUShaderProgram_OpenGL21 &);
        GPUShaderProgram_OpenGL21 & operator=(const GPUShaderProgram_OpenGL21 &);
    };
}

#endif

#endif