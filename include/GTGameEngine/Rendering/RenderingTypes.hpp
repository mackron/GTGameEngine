// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_RenderingTypes_hpp_
#define __GT_RenderingTypes_hpp_

#include <cstdint>

#define GT_GE_MAX_VERTEX_BUFFER_SLOTS       32

namespace GT
{
    //////////////////////////////////////////
    // Object Handle Types

    typedef size_t HBuffer;
    typedef size_t HInputLayout;
    typedef size_t HVertexShader;
    typedef size_t HTessellationControlShader;
    typedef size_t HTessellationEvaluationShader;
    typedef size_t HGeometryShader;
    typedef size_t HFragmentShader;
    typedef size_t HComputeShader;
    typedef size_t HShaderProgram;
    typedef size_t HTexture1D;
    typedef size_t HTexture2D;
    typedef size_t HTexture3D;
    typedef size_t HTextureCube;
    typedef size_t HFramebuffer;
    typedef size_t HFramebufferRenderbuffer;
    typedef size_t HRasterizerState;
    typedef size_t HDepthStencilState;
    typedef size_t HBlendState;




    //////////////////////////////////////////
    // Enumerators

    enum GPUBufferCPUAccessFlags
    {
        GPUBufferCPUAccess_None      = 0x00,
        GPUBufferCPUAccess_Read      = 0x01,
        GPUBufferCPUAccess_Write     = 0x02,
        GPUBufferCPUAccess_ReadWrite = GPUBufferCPUAccess_Read | GPUBufferCPUAccess_Write
    };

    enum GPUBufferMapType
    {
        GPUBufferMapType_Read              = 0,
        GPUBufferMapType_Write             = 1,
        GPUBufferMapType_ReadWrite         = 2,
        GPUBufferMapType_Write_Discard     = 3,
        GPUBufferMapType_Write_NoOverwrite = 4      //< Cannot be used with constant buffer.
    };

    enum GPUBufferType
    {
        GPUBufferType_Vertex   = 0,
        GPUBufferType_Index    = 1,
        GPUBufferType_Constant = 2
    };

    enum GPUBufferUsage
    {
        GPUBufferUsage_Default   = 0,
        GPUBufferUsage_Immutable = 1,
        GPUBufferUsage_Dynamic   = 2,
        GPUBufferUsage_Staging   = 3
    };

    enum GPUVertexAttribFormat
    {
        GPUVertexAttribFormat_Float       = 0,
        GPUVertexAttribFormat_Int         = 1,
        GPUVertexAttribFormat_UnsignedInt = 2,
    };

    enum GPUIndexFormat
    {
        GPUIndexFormat_UnsignedInt8  = 0,
        GPUIndexFormat_UnsignedInt16 = 1,
        GPUIndexFormat_UnsignedInt32 = 2,
    };

    enum GPUInputClassification
    {
        GPUInputClassification_PerVertex   = 0,
        GPUInputClassification_PerInstance = 1,
    };

    enum GPUPrimitiveTopology
    {
        GPUPrimitiveTopology_Point                  = 0,
        GPUPrimitiveTopology_Line                   = 1,
        GPUPrimitiveTopology_LineStrip              = 2,
        GPUPrimitiveTopology_Triangle               = 3,
        GPUPrimitiveTopology_TriangleStrip          = 4,
        GPUPrimitiveTopology_Line_Adjacency         = 5,
        GPUPrimitiveTopology_LineList_Adjacency     = 6,
        GPUPrimitiveTopology_Triangle_Adjacency     = 7,
        GPUPrimitiveTopology_TriangleList_Adjacency = 8
    };

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

    enum GPUFillMode
    {
        GPUFillMode_Wireframe = 0,
        GPUFillMode_Solid     = 1
    };

    enum GPUCullMode
    {
        GPUCullMode_None  = 0,
        GPUCullMode_Front = 1,
        GPUCullMode_Back  = 2
    };

    enum GPUPolygonWinding
    {
        GPUPolygonWinding_CCW = 0,
        GPUPolygonWinding_CW  = 1
    };

    enum GPUDepthWriteMask
    {
        GPUDepthWriteMask_Zero = 0,
        GPUDepthWriteMast_All  = 1
    };

    enum GPUComparisonFunc
    {
        GPUComparisonFunc_Never         = 0,
        GPUComparisonFunc_Less          = 1,
        GPUComparisonFunc_Equal         = 2,
        GPUComparisonFunc_Less_Equal    = 3,
        GPUComparisonFunc_Greater       = 4,
        GPUComparisonFunc_Not_Equal     = 5,
        GPUComparisonFunc_Greater_Eqaul = 6,
        GPUComparisonFunc_Always        = 7
    };

    enum GPUStencilOp
    {
        GPUStencilOp_Keep            = 0,
        GPUStencilOp_Zero            = 1,
        GPUStencilOp_Replace         = 2,
        GPUStencilOp_Increment_Clamp = 3,
        GPUStencilOp_Decrement_Clamp = 4,
        GPUStencilOp_Invert          = 5,
        GPUStencilOp_Increment       = 6,
        GPUStencilOp_Decrement       = 7
    };

    enum GPUClearFlag
    {
        GPUClearFlag_Depth   = 0x01,
        GPUClearFlag_Stencil = 0x02,
    };

    enum GPUHandedness
    {
        GPUHandedness_Left,     //< Forward direction = -Z
        GPUHandedness_Right     //< Forward direction = +Z
    };

    enum GPUVendor
    {
        GPUVendor_Unknown,
        GPUVendor_NVIDIA,
        GPUVendor_AMD,
        GPUVendor_Intel
    };


    //////////////////////////////////////////
    // Structures

    struct GPUInputLayoutAttribDesc
    {
        unsigned int           slotIndex;
        const char*            attributeName;               // The name of the attribute variable inside the shader, or the semantic name in the case of Direct3D.
        GPUVertexAttribFormat  attributeComponentType;      // Float, Signed Int, etc.
        unsigned int           attributeComponentCount;     // float = 1, float2 = 2, etc.
        unsigned int           attributeOffset;             // The attribute's offset.
        GPUInputClassification attributeClass;              // Per-Vertex or Per-Instance
        unsigned int           instanceStepRate;
    };

    struct GPUShaderDefine
    {
        const char* name;
        const char* value;
    };


    /// Structure describing a rasterization state.
    ///
    /// This is used when creating a rasterization state object.
    struct GPURasterizerStateDesc
    {
        GPUFillMode       fillMode;
        GPUCullMode       cullMode;
        GPUPolygonWinding polygonWinding;
        int               depthBias;
        float             depthBiasClamp;
        float             slopeScaledDepthBias;
        bool              enableDepthClip;
        bool              enableScissor;
        bool              enableMultisample;
        bool              enableAntialiasedLine;
    };



    struct GPUStencilOpDesc
    {
        GPUStencilOp stencilFailOp;
        GPUStencilOp stencilDepthFailOp;
        GPUStencilOp stencilPassOp;
        GPUComparisonFunc stencilFunc;
    };

    /// Structure describing a depth/stencil state.
    ///
    /// This is used when creating a depth/stencil state object.
    struct GPUDepthStencilStateDesc
    {
        bool              enableDepthTest;
        GPUDepthWriteMask depthWriteMask;
        GPUComparisonFunc depthFunc;
            
        bool              enableStencilTest;
        uint8_t           stencilReadMask;
        uint8_t           stencilWriteMask;
        GPUStencilOpDesc  stencilFrontFaceOp;
        GPUStencilOpDesc  stencilBackFaceOp;
    };


    /// Structure representing a viewport.
    struct GPUViewport
    {
        /// The position of the viewport on the X axis.
        float x;

        /// The position of the viewport on the Y axis.
        float y;

        /// The width of the viewport.
        float width;

        /// The height of the viewport.
        float height;

        /// The near depth range.
        float depthRangeNear;

        /// The far depth range.
        float depthRangeFar;
    };
}

#endif
