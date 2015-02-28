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
}


#endif

#endif
