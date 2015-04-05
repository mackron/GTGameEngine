// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_RenderingTypes_hpp_
#define __GT_RenderingTypes_hpp_

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
	typedef uint32_t GraphicsInterfaceClass;
	typedef uint32_t GraphicsInterfaceType;


	// Graphics Interface Class Ranges
	//  0x00000000 - 0x000000FF : Reserved by the engine.
	//  0x00000100 - 0xFFFFFFFF : Custom

	static const GraphicsInterfaceClass GraphicsInterfaceClass_Unknown = 0x00000000;
	static const GraphicsInterfaceClass GraphicsInterfaceClass_MT      = 0x00000001;		// Modern multi-threaded APIs - Vulkan/D3D12/Metal style APIs.
	static const GraphicsInterfaceClass GraphicsInterfaceClass_Legacy  = 0x00000002;		// Shader Model 2/3 APIs - OpenGL2/D3D9 style APIs. 


	// Graphics Interface Type Ranges
	//  0x00000000 - 0x0000FFFF : Reserved by the engine
	//  0x00010000 - 0xFFFFFFFF : Custom

	// Special - 0x00000000 - 0x000000FF
	static const GraphicsInterfaceType GraphicsInterfaceType_Unknown = 0x00000000;

	// Default Multi-Threaded - 0x00000100 - 0x000001FF
	static const GraphicsInterfaceType GraphicsInterfaceType_Vulkan  = 0x00000100;
	static const GraphicsInterfaceType GraphicsInterfaceType_D3D12   = 0x00000101;
	static const GraphicsInterfaceType GraphicsInterfaceType_Metal   = 0x00000102;

	// Default Legacy - 0x00000200 - 0x000002FF
	static const GraphicsInterfaceType GraphicsInterfaceType_OpenGL  = 0x000000200;




    //////////////////////////////////////////
    // Object Handle Types

    typedef size_t HGeneric;        //< A generic handle whose purpose is to cast to another handle type.
    
    typedef size_t HGraphicsDevice;
    typedef size_t HCommandAllocator;
    typedef size_t HCommandQueue;
    typedef size_t HCommandList;
    typedef size_t HPipelineState;
    typedef size_t HDescriptorHeap;
    typedef size_t HHeap;
    typedef size_t HFence;
    
    typedef size_t HBuffer;
    typedef size_t HVSInputLayout;
    typedef size_t HShader;
    typedef size_t HTexture;
    typedef size_t HTextureView;
    typedef size_t HSampler;
    typedef size_t HFramebuffer;
    typedef size_t HRasterizerState;
    typedef size_t HDepthStencilState;
    typedef size_t HBlendState;




    //////////////////////////////////////////
    // Enumerators

    enum CommandListType
    {
        CommandListType_Direct = 0,
        CommandListType_Bundle,
        CommandListType_Compute,
        CommandListType_Copy,
    };

    enum HeapType
    {
        HeapType_Generic = 0,
        HeapType_Sampler,
        HeapType_RTV,
        HeapType_DSV
    };

    enum GraphicsResourceUsage
    {
        GraphicsResourceUsage_NoAccess,
        GraphicsResourceUsage_DefaultRead,
        GraphicsResourceUsage_RenderTarget,
        GraphicsResourceUsage_UnorderedAccess,
        GraphicsResourceUsage_Depth,
        GraphicsResourceUsage_NonPixelShaderResource,
        GraphicsResourceUsage_PixelShaderResource,
        GraphicsResourceUsage_StreamOut,
        GraphicsResourceUsage_IndirectArgument,
        GraphicsResourceUsage_CopyDest,
        GraphicsResourceUsage_CopySource,
        GraphicsResourceUsage_ResolveDest,
        GraphicsResourceUsage_ResolveSource,
        GraphicsResourceUsage_GenericRead,
        GraphicsResourceUsage_Present,
        GraphicsResourceUsage_Initial
    };

    enum FenceFlag
    {
        FenceFlag_None               = 0x00,
        FenceFlag_Shared             = 0x01,
        FenceFlag_SharedCrossAdapter = 0x02
    };




    enum BufferCPUAccessFlags
    {
        BufferCPUAccess_None      = 0x00,
        BufferCPUAccess_Read      = 0x01,
        BufferCPUAccess_Write     = 0x02,
        BufferCPUAccess_ReadWrite = BufferCPUAccess_Read | BufferCPUAccess_Write
    };

    enum BufferMapType
    {
        BufferMapType_Read              = 0,
        BufferMapType_Write             = 1,
        BufferMapType_ReadWrite         = 2,
        BufferMapType_Write_Discard     = 3,
        BufferMapType_Write_NoOverwrite = 4      //< Cannot be used with constant buffer.
    };

    enum BufferType
    {
        BufferType_Vertex   = 0,
        BufferType_Index    = 1,
        BufferType_Constant = 2
    };

    enum BufferUsage
    {
        BufferUsage_Default   = 0,
        BufferUsage_Immutable = 1,
        BufferUsage_Dynamic   = 2,
        BufferUsage_Staging   = 3
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
        IndexFormat_UInt32 = 2,
    };

    enum PrimitiveTopologyType
    {
        PrimitiveTopologyType_Undefined = 0,
        PrimitiveTopologyType_Point,
        PrimitiveTopologyType_Line,
        PrimitiveTopologyType_Triangle,
        PrimitiveTopologyType_Patch
    };

    enum PrimitiveTopology
    {
        PrimitiveTopology_Point                  = 0,
        PrimitiveTopology_Line                   = 1,
        PrimitiveTopology_LineStrip              = 2,
        PrimitiveTopology_Triangle               = 3,
        PrimitiveTopology_TriangleStrip          = 4,
        PrimitiveTopology_Line_Adjacency         = 5,
        PrimitiveTopology_LineList_Adjacency     = 6,
        PrimitiveTopology_Triangle_Adjacency     = 7,
        PrimitiveTopology_TriangleList_Adjacency = 8
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


    enum FillMode
    {
        FillMode_Wireframe = 0,
        FillMode_Solid     = 1
    };

    enum CullMode
    {
        CullMode_None  = 0,
        CullMode_Front = 1,
        CullMode_Back  = 2
    };

    enum PolygonWinding
    {
        PolygonWinding_CCW = 0,
        PolygonWinding_CW  = 1
    };

    enum DepthWriteMask
    {
        DepthWriteMask_Zero = 0,
        DepthWriteMask_All  = 1
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

    enum ClearFlag
    {
        ClearFlag_Depth   = 0x01,
        ClearFlag_Stencil = 0x02,
    };

    enum GraphicsHandedness
    {
        GraphicsHandedness_Left,     //< Forward direction = -Z
        GraphicsHandedness_Right     //< Forward direction = +Z
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

    struct GraphicsDeviceInfo
    {
        /// The description of the device.
        char description[256];
    };

    struct ShaderBytecode
    {
        ShaderBytecode()
            : pData(nullptr),
              dataSizeInBytes(0)
        {
        }

        /// A pointer to the shader bytecode.
        const void* pData;

        /// The size in bytes of the shader bytecode.
        size_t dataSizeInBytes;
    };

    struct StreamOutputDeclarationEntry
    {
        StreamOutputDeclarationEntry()
            : stream(0),
              attributeIndex(0),
              startComponent(0),
              componentCount(0),
              outputSlot(0)
        {
        }

        unsigned int stream;
        unsigned int attributeIndex;
        uint8_t startComponent;
        uint8_t componentCount;
        uint8_t outputSlot;
    };

    struct StreamOutputDesc
    {
        StreamOutputDesc()
            : pSODeclaration(nullptr),
              numEntries(0),
              pBufferStrides(nullptr),
              numStrides(0),
              rasterizedStream(0)
        {
        }

        const StreamOutputDeclarationEntry *pSODeclaration;
        unsigned int numEntries;
        const unsigned int *pBufferStrides;
        unsigned int numStrides;
        unsigned int rasterizedStream;
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

    /// Structure describing a rasterization state.
    ///
    /// This is used when creating a rasterization state object.
    struct RasterizerStateDesc
    {
        RasterizerStateDesc()
            : fillMode(FillMode_Solid),
              cullMode(CullMode_Back),
              polygonWinding(PolygonWinding_CW),
              depthBias(0),
              depthBiasClamp(0.0f),
              slopeScaledDepthBias(0.0f),
              enableDepthClip(true),
              enableScissor(false),
              enableMultisample(false),
              enableAntialiasedLine(false)
        {
        }

        FillMode       fillMode;
        CullMode       cullMode;
        PolygonWinding polygonWinding;
        int            depthBias;
        float          depthBiasClamp;
        float          slopeScaledDepthBias;
        bool           enableDepthClip;
        bool           enableScissor;
        bool           enableMultisample;
        bool           enableAntialiasedLine;
    };

    struct StencilOpDesc
    {
        StencilOpDesc()
            : stencilFailOp(StencilOp_Keep),
              stencilDepthFailOp(StencilOp_Keep),
              stencilPassOp(StencilOp_Keep),
              stencilFunc(ComparisonFunc_Always)
        {
        }

        StencilOp stencilFailOp;
        StencilOp stencilDepthFailOp;
        StencilOp stencilPassOp;
        ComparisonFunc stencilFunc;
    };

    /// Structure describing a depth/stencil state.
    ///
    /// This is used when creating a depth/stencil state object.
    struct DepthStencilStateDesc
    {
        DepthStencilStateDesc()
            : enableDepthTest(true),
              depthWriteMask(DepthWriteMask_All),
              depthFunc(ComparisonFunc_Less),
              enableStencilTest(false),
              stencilReadMask(0xFF),
              stencilWriteMask(0xFF),
              stencilFrontFaceOp(),
              stencilBackFaceOp()
        {
        }

        bool              enableDepthTest;
        DepthWriteMask    depthWriteMask;
        ComparisonFunc    depthFunc;
        bool              enableStencilTest;
        uint8_t           stencilReadMask;
        uint8_t           stencilWriteMask;
        StencilOpDesc     stencilFrontFaceOp;
        StencilOpDesc     stencilBackFaceOp;
    };

    struct VSInputAttribFormat  // TODO: This is the old one. Remove this later. The difference is that the index instead of the name is used to identify the attribute.
    {
        unsigned int       slotIndex;
        const char*        attributeName;               //< The name of the attribute variable inside the shader, or the semantic name in the case of Direct3D.
        VertexAttribFormat attributeComponentType;      //< Float, Signed Int, etc.
        unsigned int       attributeComponentCount;     //< float = 1, float2 = 2, etc.
        unsigned int       attributeOffset;             //< The attribute's offset within the buffer.
        unsigned int       instanceStepRate;            //< The rate at which the vertex attributes advance for per-instance data. When this is 0, it is advanced once per vertex, otherwise it is advanced by this value for each instance. This will usually be set to 0 for non-instance vertex data, and 1 for instanced data.
    };

    struct VSInputAttribFormat2
    {
        unsigned int       slotIndex;
        unsigned int       attributeIndex;              //< The index of the attribute variable inside the shader.
        VertexAttribFormat attributeComponentType;      //< Float, Signed Int, etc.
        unsigned int       attributeComponentCount;     //< float = 1, float2 = 2, etc.
        unsigned int       attributeOffset;             //< The attribute's offset within the buffer.
        unsigned int       instanceStepRate;            //< The rate at which the vertex attributes advance for per-instance data. When this is 0, it is advanced once per vertex, otherwise it is advanced by this value for each instance. This will usually be set to 0 for non-instance vertex data, and 1 for instanced data.
    };

    struct VSInputLayoutDesc
    {
        const VSInputAttribFormat2* attribFormats;
        unsigned int attribFormatsCount;
    };

    /// Structure describing multisampling.
    struct SampleDesc
    {
        SampleDesc()
            : count(1),
              quality(0)
        {
        }

        unsigned int count;
        unsigned int quality;
    };

    struct GraphicsPipelineStateDesc
    {
        GraphicsPipelineStateDesc()
            : shaderBytecodeVS(),
              shaderBytecodeTCS(),
              shaderBytecodeTES(),
              shaderBytecodeGS(),
              shaderBytecodeFS(),
              streamOutput(),
              blendState(),
              sampleMask(0),
              rasterizerState(),
              depthStencilState(),
              inputLayout(),
              primitiveTopologyType(PrimitiveTopologyType_Triangle),
              renderTargetCount(0),
              renderTargetFormats(),
              depthStencilFormat(TextureFormat_D24_S8),
              sampleDesc(),
              nodeMask(0)
        {
            renderTargetFormats[0] = TextureFormat_RGBA8;
            renderTargetFormats[1] = TextureFormat_RGBA8;
            renderTargetFormats[2] = TextureFormat_RGBA8;
            renderTargetFormats[3] = TextureFormat_RGBA8;
            renderTargetFormats[4] = TextureFormat_RGBA8;
            renderTargetFormats[5] = TextureFormat_RGBA8;
            renderTargetFormats[6] = TextureFormat_RGBA8;
            renderTargetFormats[7] = TextureFormat_RGBA8;
        }

        ShaderBytecode   shaderBytecodeVS;
        ShaderBytecode   shaderBytecodeTCS;
        ShaderBytecode   shaderBytecodeTES;
        ShaderBytecode   shaderBytecodeGS;
        ShaderBytecode   shaderBytecodeFS;
        StreamOutputDesc streamOutput;
        BlendStateDesc   blendState;
        unsigned int     sampleMask;
        RasterizerStateDesc rasterizerState;
        DepthStencilStateDesc depthStencilState;
        VSInputLayoutDesc inputLayout;
        PrimitiveTopologyType primitiveTopologyType;
        unsigned int renderTargetCount;
        TextureFormat renderTargetFormats[8];
        TextureFormat depthStencilFormat;
        SampleDesc sampleDesc;
        unsigned int nodeMask;
    };


    

    struct ShaderDefine
    {
        const char* name;
        const char* value;
    };


 

    /// Structure representing a viewport.
    struct GraphicsViewport
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

    /// Structure representing a scissor rectangle.
    struct ScissorRect
    {
        /// The position of the scissor rectangle on the X axis, relative to the top left corner.
        int x;

        /// The position of the scissor rectangle on the Y axis, relative to the top left corner.
        int y;

        /// The width of the scissor rectangle.
        unsigned int width;

        /// The height of the scissor rectangle.
        unsigned int height;
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
        unsigned int  usage;
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
