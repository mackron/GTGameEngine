// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GraphicsTypes
#define GT_GraphicsTypes

#include <cstdint>
#include <cfloat>

#define GT_MAX_VERTEX_BUFFER_SLOTS          32
#define GT_MAX_CONSTANT_BUFFER_SLOTS        14
#define GT_MAX_FRAMEBUFFER_RENDER_TARGETS   8
#define GT_MAX_VIEWPORTS                    16

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    // APIs.
    typedef uint32_t GraphicsAPIType;

    static const GraphicsAPIType GraphicsAPIType_Unknown = 0x00000000;
    static const GraphicsAPIType GraphicsAPIType_Null    = 0x00000001;
    static const GraphicsAPIType GraphicsAPIType_OpenGL  = 0x00000002;
    static const GraphicsAPIType GraphicsAPIType_Vulkan  = 0x00000003;
    static const GraphicsAPIType GraphicsAPIType_D3D12   = 0x00000004;



    //////////////////////////////////////////
    // Enumerators

    /// The different types for material variables. Some of these are invalid in certain contexts, which are specific to the underlying rendering API. All
    /// of these match up with drmtl_type.
    enum class GraphicsMaterialVariableType
    {
        Unknown     = 0,

        Float       = 1,
        Float2      = 2,
        Float3      = 3,
        Float4      = 4,

        Integer     = 5,
        Integer2    = 6,
        Integer3    = 7,
        Integer4    = 8,

        Texture1D   = 9,
        Texture2D   = 10,
        Texture3D   = 11,
        TextureCube = 12,

        Boolean     = 13
    };


    enum VertexAttribFormat
    {
        VertexAttribFormat_Float  = 0,
        VertexAttribFormat_Int32,
        VertexAttribFormat_UInt32,
        VertexAttribFormat_Int16,
        VertexAttribFormat_UInt16,
        VertexAttribFormat_Int8,
        VertexAttribFormat_UInt8
    };

    enum IndexFormat
    {
        IndexFormat_UInt8  = 0,
        IndexFormat_UInt16 = 1,
        IndexFormat_UInt32 = 2
    };

    enum PrimitiveTopologyType
    {
        PrimitiveTopologyType_Undefined = 0,
        PrimitiveTopologyType_Point,
        PrimitiveTopologyType_Line,
        PrimitiveTopologyType_Triangle,
        PrimitiveTopologyType_Patch
    };



    enum ComparisonFunc
    {
        ComparisonFunc_Never         = 0,
        ComparisonFunc_Less          = 1,
        ComparisonFunc_Equal         = 2,
        ComparisonFunc_Less_Equal    = 3,
        ComparisonFunc_Greater       = 4,
        ComparisonFunc_Not_Equal     = 5,
        ComparisonFunc_Greater_Eqaul = 6,
        ComparisonFunc_Always        = 7
    };

    enum StencilOp
    {
        StencilOp_Keep            = 0,
        StencilOp_Zero            = 1,
        StencilOp_Replace         = 2,
        StencilOp_Increment_Clamp = 3,
        StencilOp_Decrement_Clamp = 4,
        StencilOp_Invert          = 5,
        StencilOp_Increment       = 6,
        StencilOp_Decrement       = 7
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
        TextureFormat_SRGB8_A8,     ///< sRGB, linear alpha.

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
        TextureFormat_D24_S8,               ///< GL_DEPTH24_STENCIL8 / DXGI_FORMAT_D24_UNORM_S8_UINT

        // Compressed.
        TextureFormat_RGBA_DXT1,            ///< GL_COMPRESSED_RGBA_S3TC_DXT1_EXT       / DXGI_FORMAT_BC1_UNORM
        TextureFormat_RGBA_DXT3,            ///< GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       / DXGI_FORMAT_BC2_UNORM
        TextureFormat_RGBA_DXT5,            ///< GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       / DXGI_FORMAT_BC3_UNORM
        TextureFormat_SRGB_Alpha_DXT1,      ///< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT / DXGI_FORMAT_BC1_UNORM_SRGB
        TextureFormat_SRGB_Alpha_DXT3,      ///< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT / DXGI_FORMAT_BC2_UNORM_SRGB
        TextureFormat_SRGB_Alpha_DXT5,      ///< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT / DXGI_FORMAT_BC3_UNORM_SRGB

        TextureFormat_R_RGTC,               ///< GL_COMPRESSED_RED_RGTC1        / DXGI_FORMAT_BC4_UNORM
        TextureFormat_R_RGTC_SNORM,         ///< GL_COMPRESSED_SIGNED_RED_RGTC1 / DXGI_FORMAT_BC4_SNORM
        TextureFormat_RG_RGTC,              ///< GL_COMPRESSED_RG_RGTC2         / DXGI_FORMAT_BC5_UNORM
        TextureFormat_RG_RGTC_SNORM,        ///< GL_COMPRESSED_SIGNED_RG_RGTC2  / DXGI_FORMAT_BC5_SNORM

        TextureFormat_RGBA_BPTC,            ///< GL_COMPRESSED_RGBA_BPTC_UNORM_ARB         / DXGI_FORMAT_BC7_UNORM
        TextureFormat_SRGB_Alpha_BPTC,      ///< GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB   / DXGI_FORMAT_BC7_UNORM_SRGB
        TextureFormat_RGB_UF16_BPTC,        ///< GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB / DXGI_FORMAT_BC6H_UF16
        TextureFormat_RGB_SF16_BPTC         ///< GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB   / DXGI_FORMAT_BC6H_SF16
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
        TextureFilter_Anisotropic
    };

    enum VertexAttribSemantic
    {
        VertexAttribSemantic_Generic = 0,
        VertexAttribSemantic_Position,
        VertexAttribSemantic_TexCoord,
        VertexAttribSemantic_Normal,
        VertexAttribSemantic_Tangent,
        VertexAttribSemantic_Binormal
    };

    enum AAType
    {
        None,
        SSAA,   // Super-sampling AA
        MSAA,
        FXAA
    };



    //////////////////////////////////////////
    // Structures

    /// Structure representing the layout of a vertex attribute in a mesh.
    struct VertexAttribLayout
    {
        VertexAttribLayout()
            : attributeComponentType(VertexAttribFormat_Float),
              attributeComponentCount(4),
              attributeOffset(0),
              attributeSemantic(VertexAttribSemantic_Generic)
        {
        }

        VertexAttribLayout(VertexAttribFormat componentType, unsigned int componentCount, unsigned int offset, VertexAttribSemantic semantic)
            : attributeComponentType(componentType),
              attributeComponentCount(componentCount),
              attributeOffset(offset),
              attributeSemantic(semantic)
        {
        }


        GT::VertexAttribFormat   attributeComponentType;        // Float, Signed Int, etc.
        unsigned int             attributeComponentCount;       // float = 1, float2 = 2, etc.
        unsigned int             attributeOffset;               // The attribute's offset.
        GT::VertexAttribSemantic attributeSemantic;             // The attribute's semantic (generic, position, texcoord, etc.)
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
        32,     ///< sRGB, linear alpha.

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
        32,               ///< GL_DEPTH24_STENCIL8 / DXGI_FORMAT_D24_UNORM_S8_UINT

        // Compressed.
        4,              ///< GL_COMPRESSED_RGBA_S3TC_DXT1_EXT       / DXGI_FORMAT_BC1_UNORM
        8,              ///< GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       / DXGI_FORMAT_BC2_UNORM
        8,              ///< GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       / DXGI_FORMAT_BC3_UNORM
        4,              ///< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT / DXGI_FORMAT_BC1_UNORM_SRGB
        8,              ///< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT / DXGI_FORMAT_BC2_UNORM_SRGB
        8,              ///< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT / DXGI_FORMAT_BC3_UNORM_SRGB

        4,              ///< GL_COMPRESSED_RED_RGTC1        / DXGI_FORMAT_BC4_UNORM
        4,              ///< GL_COMPRESSED_SIGNED_RED_RGTC1 / DXGI_FORMAT_BC4_SNORM
        8,              ///< GL_COMPRESSED_RG_RGTC2         / DXGI_FORMAT_BC5_UNORM
        8,              ///< GL_COMPRESSED_SIGNED_RG_RGTC2  / DXGI_FORMAT_BC5_SNORM

        8,              ///< GL_COMPRESSED_RGBA_BPTC_UNORM_ARB         / DXGI_FORMAT_BC7_UNORM
        8,              ///< GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB   / DXGI_FORMAT_BC7_UNORM_SRGB
        8,              ///< GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB / DXGI_FORMAT_BC6H_UF16
        8               ///< GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB   / DXGI_FORMAT_BC6H_SF16
    };
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
