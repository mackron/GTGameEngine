// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUShaderTargets_hpp_
#define __GT_GE_Rendering_GPUShaderTargets_hpp_

namespace GT
{
    namespace GE
    {
        enum GPUShaderTarget
        {
            GPUShaderTarget_Unknown      = 0,

            // OpenGL
            GPUShaderTarget_GLSL_120_VS  = 1,       // GLSL 1.20 vertex shader
            GPUShaderTarget_GLSL_120_FS  = 2,       // GLSL 1.20 fragment shader
            GPUShaderTarget_GLSL_450_VS  = 3,       // GLSL 4.50 vertex shader
            GPUShaderTarget_GLSL_450_TCS = 4,       // GLSL 4.50 tessellation control shader
            GPUShaderTarget_GLSL_450_TES = 5,       // GLSL 4.50 tessellation evaluation shader
            GPUShaderTarget_GLSL_450_GS  = 6,       // GLSL 4.50 geometry shader
            GPUShaderTarget_GLSL_450_FS  = 7,       // GLSL 4.50 fragment shader
            GPUShaderTarget_GLSL_450_CS  = 8,       // GLSL 4.50 compute shader
            GPUShaderTarget_ARB_VP       = 9,       // ARB_vertex_program vertex program
            GPUShaderTarget_ARB_FP       = 10,      // ARB_fragment_program fragment program
            GPUShaderTarget_NV_5_VP      = 11,      // NV_gpu_program5 vertex program
            GPUShaderTarget_NV_5_TCP     = 12,      // NV_gpu_program5 tessellation control program
            GPUShaderTarget_NV_5_TEP     = 13,      // NV_gpu_program5 tessellation evaluation program
            GPUShaderTarget_NV_5_GP      = 14,      // NV_gpu_program5 geometry program
            GPUShaderTarget_NV_5_FP      = 15,      // NV_gpu_program5 fragment program
            GPUShaderTarget_NV_5_CP      = 16,      // NV_gpu_program5 compute program

            // D3D
            GPUShaderTarget_HLSL_50_VS   = 17,      // D3D11 vertex shader
            GPUShaderTarget_HLSL_50_HS   = 28,      // D3D11 hull shader
            GPUShaderTarget_HLSL_50_DS   = 19,      // D3D11 domain shader
            GPUShaderTarget_HLSL_50_GS   = 20,      // D3D11 geometry shader
            GPUShaderTarget_HLSL_50_PS   = 21,      // D3D11 pixel shader
            GPUShaderTarget_HLSL_50_CS   = 22       // D3D11 compute shader
        };
    }
}

#endif
