// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_RenderingTypes_hpp_
#define __GT_RenderingTypes_hpp_

#include <cstdint>
#include <cfloat>

#define GT_GE_MAX_VERTEX_BUFFER_SLOTS       32

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    //////////////////////////////////////////
    // Object Handle Types

    typedef size_t HGeneric;        //< A generic handle whose purpose is to cast to another handle type.
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
    typedef size_t HTexture2DMultisample;
    typedef size_t HTexture3D;
    typedef size_t HTextureCube;
    typedef size_t HTexture;
    typedef size_t HTextureView;
    typedef size_t HSampler;
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

    enum ShaderLanguage
    {
        ShaderLanguage_Unknown = 0,

        // OpenGL
        ShaderLanguage_GLSL_400,
        ShaderLanguage_GLSL_410,
        ShaderLanguage_GLSL_420,
        ShaderLanguage_GLSL_430,
        ShaderLanguage_GLSL_440,
        ShaderLanguage_GLSL_450,
        ShaderLanguage_GL_NV_5,         //< NVIDIA assembly shaders

        // D3D
        ShaderLanguage_HLSL_50
    };

    enum ShaderType
    {
        ShaderType_Vertex                 = 0,
        ShaderType_TessellationControl    = 1,
        ShaderType_TessellationEvaluation = 2,
        ShaderType_Geometry               = 3,
        ShaderType_Fragment               = 4,
        ShaderType_Compute                = 5
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


    enum TextureType
    {
        TextureType_1D,
        TextureType_1D_Array,
        TextureType_2D,
        TextureType_2D_Array,
        TextureType_2D_Multisample,
        TextureType_2D_Multisample_Array,
        TextureType_3D,
        TextureType_Cube,
        TextureType_Cube_Array
    };

    enum TextureUsage
    {
        TextureUsage_ShaderResource     = (1 << 0),
        TextureUsage_RenderTarget       = (1 << 1),
        TextureUsage_DepthStencilTarget = (1 << 2)
    };

    enum TextureFormat
    {
        TextureFormat_Unknown = 0,

        // Unsigned Normalized.
        TextureFormat_R8,
        TextureFormat_RG8,
        TextureFormat_RGBA8,
        TextureFormat_R16,
        TextureFormat_RG16,
        TextureFormat_RGBA16,
        TextureFormat_RGB10_A2,
        TextureFormat_SRGB8_A8,     //< sRGB, linear alpha.

        // Signed Normalized.
        TextureFormat_R8_SNORM,
        TextureFormat_RG8_SNORM,
        TextureFormat_RGBA8_SNORM,
        TextureFormat_R16_SNORM,
        TextureFormat_RG16_SNORM,
        TextureFormat_RGBA16_SNORM,

        // Unsigned Integral.
        TextureFormat_R8UI,
        TextureFormat_RG8UI,
        TextureFormat_RGBA8UI,
        TextureFormat_R16UI,
        TextureFormat_RG16UI,
        TextureFormat_RGBA16UI,
        TextureFormat_R32UI,
        TextureFormat_RG32UI,
        TextureFormat_RGB32UI,
        TextureFormat_RGBA32UI,

        // Signed Integral.
        TextureFormat_RBSI,
        TextureFormat_RG8SI,
        TextureFormat_RGBA8SI,
        TextureFormat_R16SI,
        TextureFormat_RG16SI,
        TextureFormat_RGBA16SI,
        TextureFormat_R32SI,
        TextureFormat_RG32SI,
        TextureFormat_RGB32SI,
        TextureFormat_RGBA32SI,

        // Float
        TextureFormat_R16F,
        TextureFormat_RG16F,
        TextureFormat_RGBA16F,
        TextureFormat_R32F,
        TextureFormat_RG32F,
        TextureFormat_RGB32F,
        TextureFormat_RGBA32F,
        TextureFormat_R11G11B10F,

        // Special
        TextureFormat_D24_S8,               //< GL_DEPTH24_STENCIL8 / DXGI_FORMAT_D24_UNORM_S8_UINT

        // Compressed.
        TextureFormat_RGBA_DXT1,            //< GL_COMPRESSED_RGBA_S3TC_DXT1_EXT       / DXGI_FORMAT_BC1_UNORM
        TextureFormat_RGBA_DXT3,            //< GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       / DXGI_FORMAT_BC2_UNORM
        TextureFormat_RGBA_DXT5,            //< GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       / DXGI_FORMAT_BC3_UNORM
        TextureFormat_SRGB_Alpha_DXT1,      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT / DXGI_FORMAT_BC1_UNORM_SRGB
        TextureFormat_SRGB_Alpha_DXT3,      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT / DXGI_FORMAT_BC2_UNORM_SRGB
        TextureFormat_SRGB_Alpha_DXT5,      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT / DXGI_FORMAT_BC3_UNORM_SRGB

        TextureFormat_R_RGTC,               //< GL_COMPRESSED_RED_RGTC1        / DXGI_FORMAT_BC4_UNORM
        TextureFormat_R_RGTC_SNORM,         //< GL_COMPRESSED_SIGNED_RED_RGTC1 / DXGI_FORMAT_BC4_SNORM
        TextureFormat_RG_RGTC,              //< GL_COMPRESSED_RG_RGTC2         / DXGI_FORMAT_BC5_UNORM
        TextureFormat_RG_RGTC_SNORM,        //< GL_COMPRESSED_SIGNED_RG_RGTC2  / DXGI_FORMAT_BC5_SNORM

        TextureFormat_RGBA_BPTC,            //< GL_COMPRESSED_RGBA_BPTC_UNORM_ARB         / DXGI_FORMAT_BC7_UNORM
        TextureFormat_SRGB_Alpha_BPTC,      //< GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB   / DXGI_FORMAT_BC7_UNORM_SRGB
        TextureFormat_RGB_UF16_BPTC,        //< GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB / DXGI_FORMAT_BC6H_UF16
        TextureFormat_RGB_SF16_BPTC,        //< GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB   / DXGI_FORMAT_BC6H_SF16
    };

    // Texture filters. All filters are named as TextureFilter_<minification>_<magnification>_<mipmap>
    enum TextureFilter
    {
        TextureFilter_Point_Point_Point = 0,
        TextureFilter_Point_Point_Linear,
        TextureFilter_Point_Linear_Point,
        TextureFitler_Point_Linear_Linear,
        TextureFilter_Linear_Point_Point,
        TextureFilter_Linear_Point_Linear,
        TextureFilter_Linear_Linear_Point,
        TextureFilter_Linear_Linear_Linear,
        TextureFilter_Anisotropic,
    };

    enum TextureAddressMode
    {
        TextureAddressMode_Wrap = 0,
        TextureAddressMode_Mirror,
        TextureAddressMode_Clamp,
        TextureAddressMode_Border,
        TextureAddressMode_Mirror_Once
    };

    enum BlendParameter
    {
        BlendParameter_Zero = 0,
        BlendParameter_One,

        BlendParameter_Src_Color,
        BlendParameter_Inv_Src_Color,
        BlendParameter_Src_Alpha,
        BlendParameter_Inv_Src_Alpha,
        BlendParameter_Src_Alpha_Saturate,
        
        BlendParameter_Dst_Color,
        BlendParameter_Inv_Dst_Color,
        BlendParameter_Dst_Alpha,
        BlendParameter_Inv_Dst_Alpha,
        
        BlendParameter_Src1_Color,
        BlendParameter_Inv_Src1_Color,
        BlendParameter_Src1_Alpha,
        BlendParameter_Inv_Src1_Alpha,

        BlendParameter_BlendFactor,
        BlendParameter_Inv_BlendFactor
    };

    enum BlendOp
    {
        BlendOp_Add = 0,
        BlendOp_Subtract,
        BlendOp_Reverse_Subtract,
        BlendOp_Min,
        BlendOp_Max
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


    /// Structure representing the blending parameters and function to use for a single render target.
    struct RenderTargetBlendStateDesc
    {
        RenderTargetBlendStateDesc()
            : enableBlending(false),
              srcBlendParameter(BlendParameter_One),
              dstBlendParameter(BlendParameter_Zero),
              blendOp(BlendOp_Add),
              srcAlphaBlendParameter(BlendParameter_One),
              dstAlphaBlendParameter(BlendParameter_Zero),
              blendOpAlpha(BlendOp_Add),
              writeMask(0xFF)
        {
        }

        bool           enableBlending;
        BlendParameter srcBlendParameter;
        BlendParameter dstBlendParameter;
        BlendOp        blendOp;
        BlendParameter srcAlphaBlendParameter;
        BlendParameter dstAlphaBlendParameter;
        BlendOp        blendOpAlpha;
        uint8_t        writeMask;   //< RGBA masking.
    };

    /// Structure describing the blend state.
    struct BlendStateDesc
    {
        BlendStateDesc()
            : enableAlphaToCoverage(false),
              enableIndependentBlend(false),
              renderTarget()
        {
        }

        bool enableAlphaToCoverage;
        bool enableIndependentBlend;
        RenderTargetBlendStateDesc renderTarget[8];
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

    /// Structure describing a texture.
    struct TextureDesc
    {
        TextureType   type;
        unsigned int  width;
        unsigned int  height;           //< For 1D textures, this is the number of items in the array.
        unsigned int  depth;            //< For 2D textures, this is the number of items in the array. For cubemaps this is a multiple of 6.
        unsigned int  mipmapLevels;     //< Mipmap levels
        TextureFormat format;
        TextureUsage  usage;
        unsigned int  sampleCount;      //< Only used by multisampled textures.
    };


    /// Structure describing a sampler object.
    struct SamplerDesc
    {
        SamplerDesc()
            : filter(TextureFilter_Linear_Linear_Linear),
              addressModeU(TextureAddressMode_Clamp),
              addressModeV(TextureAddressMode_Clamp),
              addressModeW(TextureAddressMode_Clamp),
              mipLODBias(0.0f),
              maxAnisotropy(1),
              borderColor(),
              minLOD(-FLT_MAX),
              maxLOD( FLT_MAX)
        {
            borderColor[0] = 1.0f;
            borderColor[1] = 1.0f;
            borderColor[2] = 1.0f;
            borderColor[3] = 1.0f;
        }

        TextureFilter filter;
        TextureAddressMode addressModeU;
        TextureAddressMode addressModeV;
        TextureAddressMode addressModeW;
        float mipLODBias;
        unsigned int maxAnisotropy;
        float borderColor[4];
        float minLOD;
        float maxLOD;
    };



    //////////////////////////////////////////
    // Mappings / Lookup Tables

    /// The list of integers representing the bits-per-pixel of a single pixel for each texture format.
    static unsigned int g_TextureFormatsBPPTable[] =
    {
        0,

        // Unsigned Normalized.
        8,
        16,
        32,
        16,
        32,
        64,
        32,
        32,     //< sRGB, linear alpha.

        // Signed Normalized.
        8,
        16,
        32,
        16,
        32,
        64,

        // Unsigned Integral.
        8,
        16,
        32,
        16,
        32,
        64,
        32,
        64,
        96,
        128,

        // Signed Integral.
        8,
        16,
        32,
        16,
        32,
        64,
        32,
        64,
        96,
        128,

        // Float
        16,
        32,
        64,
        32,
        64,
        96,
        128,
        32,

        // Special
        32,               //< GL_DEPTH24_STENCIL8 / DXGI_FORMAT_D24_UNORM_S8_UINT

        // Compressed.
        4,              //< GL_COMPRESSED_RGBA_S3TC_DXT1_EXT       / DXGI_FORMAT_BC1_UNORM
        8,              //< GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       / DXGI_FORMAT_BC2_UNORM
        8,              //< GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       / DXGI_FORMAT_BC3_UNORM
        4,              //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT / DXGI_FORMAT_BC1_UNORM_SRGB
        8,              //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT / DXGI_FORMAT_BC2_UNORM_SRGB
        8,              //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT / DXGI_FORMAT_BC3_UNORM_SRGB

        4,              //< GL_COMPRESSED_RED_RGTC1        / DXGI_FORMAT_BC4_UNORM
        4,              //< GL_COMPRESSED_SIGNED_RED_RGTC1 / DXGI_FORMAT_BC4_SNORM
        8,              //< GL_COMPRESSED_RG_RGTC2         / DXGI_FORMAT_BC5_UNORM
        8,              //< GL_COMPRESSED_SIGNED_RG_RGTC2  / DXGI_FORMAT_BC5_SNORM

        8,              //< GL_COMPRESSED_RGBA_BPTC_UNORM_ARB         / DXGI_FORMAT_BC7_UNORM
        8,              //< GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB   / DXGI_FORMAT_BC7_UNORM_SRGB
        8,              //< GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB / DXGI_FORMAT_BC6H_UF16
        8,              //< GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB   / DXGI_FORMAT_BC6H_SF16
    };
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
