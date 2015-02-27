// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUVertexShader_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)

namespace GT
{
    GPUVertexShader_D3D11::GPUVertexShader_D3D11(ID3D11VertexShader* vertexShaderD3D, const void* shaderBinaryD3D, size_t shaderBinarySizeD3D)
        : m_vertexShaderD3D(vertexShaderD3D),
            m_shaderBinaryD3D(malloc(shaderBinarySizeD3D)),
            m_shaderBinarySizeD3D(shaderBinarySizeD3D)
    {
        memcpy(m_shaderBinaryD3D, shaderBinaryD3D, shaderBinarySizeD3D);
    }

    GPUVertexShader_D3D11::~GPUVertexShader_D3D11()
    {
        free(m_shaderBinaryD3D);
    }


    ID3D11VertexShader* GPUVertexShader_D3D11::GetD3D11VertexShader()
    {
        return m_vertexShaderD3D;
    }

    const void* GPUVertexShader_D3D11::GetD3D11ShaderBinary() const
    {
        return m_shaderBinaryD3D;
    }

    size_t GPUVertexShader_D3D11::GetD3D11ShaderBinarySize() const
    {
        return m_shaderBinarySizeD3D;
    }
}

#endif
