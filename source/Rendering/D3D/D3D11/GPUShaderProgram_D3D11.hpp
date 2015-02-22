// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUShaderProgram_D3D11_hpp_
#define __GT_GE_GPUShaderProgram_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTGameEngine/Rendering/GPUShaderProgram.hpp>
#include "GPUVertexShader_D3D11.hpp"
#include "GPUFragmentShader_D3D11.hpp"

namespace GT
{
    namespace GE
    {
        /// D3D11 implementation of the shader program class.
        ///
        /// This is just a container around a vertex and fragment shader object.
        class GPUShaderProgram_D3D11 : public GPUShaderProgram
        {
        public:

            /// Constructor.
            GPUShaderProgram_D3D11(GPUVertexShader_D3D11* vertexShader, GPUFragmentShader_D3D11* fragmentShader);

            /// Destructor.
            virtual ~GPUShaderProgram_D3D11();



            /// Retrieves a pointer to the vertex shader.
            ///
            /// @return A pointer to the vertex shader.
            GPUVertexShader_D3D11* GetVertexShader();

            /// Retrieves a pointer to the fragment shader.
            ///
            /// @return A pointer to the fragment shader.
            GPUFragmentShader_D3D11* GetFragmentShader();



        private:

            /// A pointer to the vertex shader object.
            GPUVertexShader_D3D11* m_vertexShader;

            /// A pointer to the fragment shader object.
            GPUFragmentShader_D3D11* m_fragmentShader;



        private:    // No copying.
            GPUShaderProgram_D3D11(const GPUShaderProgram_D3D11 &);
            GPUShaderProgram_D3D11 & operator=(const GPUShaderProgram_D3D11 &);
        };
    }
}

#endif
#endif
