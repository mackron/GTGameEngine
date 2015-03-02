// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_RenderingTypes_D3D11_hpp_
#define __GT_RenderingTypes_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTGameEngine/Rendering/RenderingTypes.hpp>
#include <d3d11.h>

namespace GT
{
    struct ShaderProgram_D3D11
    {
        ShaderProgram_D3D11(HVertexShader hVertexShaderIn, HFragmentShader hFragmentShaderIn)
            : hVertexShader(hVertexShaderIn), hFragmentShader(hFragmentShaderIn)
        {
        }

        HVertexShader   hVertexShader;
        HFragmentShader hFragmentShader;
    };



    static const DXGI_FORMAT g_D3DTextureFormatsTable[] =
    {
        DXGI_FORMAT_UNKNOWN,

        // Unsigned Normalized.
        DXGI_FORMAT_R8_UNORM,
        DXGI_FORMAT_R8G8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R16_UNORM,
        DXGI_FORMAT_R16G16_UNORM,
        DXGI_FORMAT_R16G16B16A16_UNORM,
        DXGI_FORMAT_R10G10B10A2_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,

        // Signed Normalized.
        DXGI_FORMAT_R8_SNORM,
        DXGI_FORMAT_R8G8_SNORM,
        DXGI_FORMAT_R8G8B8A8_SNORM,
        DXGI_FORMAT_R16_SNORM,
        DXGI_FORMAT_R16G16_SNORM,
        DXGI_FORMAT_R16G16B16A16_SNORM,

        // Unsigned Integral.
        DXGI_FORMAT_R8_UINT,
        DXGI_FORMAT_R8G8_UINT,
        DXGI_FORMAT_R8G8B8A8_UINT,
        DXGI_FORMAT_R16_UINT,
        DXGI_FORMAT_R16G16_UINT,
        DXGI_FORMAT_R16G16B16A16_UINT,
        DXGI_FORMAT_R32_UINT,
        DXGI_FORMAT_R32G32_UINT,
        DXGI_FORMAT_R32G32B32_UINT,
        DXGI_FORMAT_R32G32B32A32_UINT,

        // Signed Integral.
        DXGI_FORMAT_R8_SINT,
        DXGI_FORMAT_R8G8_SINT,
        DXGI_FORMAT_R8G8B8A8_SINT,
        DXGI_FORMAT_R16_SINT,
        DXGI_FORMAT_R16G16_SINT,
        DXGI_FORMAT_R16G16B16A16_SINT,
        DXGI_FORMAT_R32_SINT,
        DXGI_FORMAT_R32G32_SINT,
        DXGI_FORMAT_R32G32B32_SINT,
        DXGI_FORMAT_R32G32B32A32_SINT,

        // Float
        DXGI_FORMAT_R16_FLOAT,
        DXGI_FORMAT_R16G16_FLOAT,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R11G11B10_FLOAT,

        // Special
        DXGI_FORMAT_D24_UNORM_S8_UINT,

        // Compressed
        DXGI_FORMAT_BC1_UNORM,
        DXGI_FORMAT_BC2_UNORM,
        DXGI_FORMAT_BC3_UNORM,
        DXGI_FORMAT_BC1_UNORM_SRGB,
        DXGI_FORMAT_BC2_UNORM_SRGB,
        DXGI_FORMAT_BC3_UNORM_SRGB,

        DXGI_FORMAT_BC4_UNORM,
        DXGI_FORMAT_BC4_SNORM,
        DXGI_FORMAT_BC5_UNORM,
        DXGI_FORMAT_BC5_SNORM,

        DXGI_FORMAT_BC7_UNORM,
        DXGI_FORMAT_BC7_UNORM_SRGB,
        DXGI_FORMAT_BC6H_UF16,
        DXGI_FORMAT_BC6H_SF16
    };
}


#endif

#endif
