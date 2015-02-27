// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUShaderProgram_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)

namespace GT
{
    GPUShaderProgram_D3D11::GPUShaderProgram_D3D11(GPUVertexShader_D3D11* vertexShader, GPUFragmentShader_D3D11* fragmentShader)
        : m_vertexShader(vertexShader),
            m_fragmentShader(fragmentShader)
    {
    }

    GPUShaderProgram_D3D11::~GPUShaderProgram_D3D11()
    {
    }


    GPUVertexShader_D3D11* GPUShaderProgram_D3D11::GetVertexShader()
    {
        return m_vertexShader;
    }

    GPUFragmentShader_D3D11* GPUShaderProgram_D3D11::GetFragmentShader()
    {
        return m_fragmentShader;
    }
}

#endif