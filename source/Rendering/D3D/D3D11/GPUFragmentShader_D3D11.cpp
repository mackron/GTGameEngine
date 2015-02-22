// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUFragmentShader_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)

namespace GT
{
    namespace GE
    {
        GPUFragmentShader_D3D11::GPUFragmentShader_D3D11(ID3D11PixelShader* pixelShaderD3D, const void* shaderBinaryD3D, size_t shaderBinarySizeD3D)
            : m_pixelShaderD3D(pixelShaderD3D),
              m_shaderBinaryD3D(malloc(shaderBinarySizeD3D)),
              m_shaderBinarySizeD3D(shaderBinarySizeD3D)
        {
            memcpy(m_shaderBinaryD3D, shaderBinaryD3D, shaderBinarySizeD3D);
        }

        GPUFragmentShader_D3D11::~GPUFragmentShader_D3D11()
        {
            free(m_shaderBinaryD3D);
        }


        ID3D11PixelShader* GPUFragmentShader_D3D11::GetD3D11PixelShader()
        {
            return m_pixelShaderD3D;
        }

        const void* GPUFragmentShader_D3D11::GetD3D11ShaderBinary() const
        {
            return m_shaderBinaryD3D;
        }

        size_t GPUFragmentShader_D3D11::GetD3D11ShaderBinarySize() const
        {
            return m_shaderBinarySizeD3D;
        }
    }
}

#endif