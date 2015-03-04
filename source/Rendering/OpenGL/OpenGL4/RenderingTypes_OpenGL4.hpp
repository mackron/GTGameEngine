// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_RenderingTypes_OpenGL4_hpp_
#define __GT_RenderingTypes_OpenGL4_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL4)
#include <GTGameEngine/Rendering/RenderingTypes.hpp>
#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>
#include <GTLib/ReferenceCountedObject.hpp>

namespace GT
{
    /// OpenGL 2.1 rasterizer state.
    struct RasterizerState_OpenGL4 : public ReferenceCountedObject, public GPURasterizerStateDesc
    {
        RasterizerState_OpenGL4(const GPURasterizerStateDesc &desc)
            : ReferenceCountedObject(), GPURasterizerStateDesc(desc)
        {
        }
    };

    /// OpenGL 2.1 depth/stencil state.
    struct DepthStencilState_OpenGL4 : public ReferenceCountedObject, public GPUDepthStencilStateDesc
    {
        DepthStencilState_OpenGL4(const GPUDepthStencilStateDesc &desc)
            : ReferenceCountedObject(), GPUDepthStencilStateDesc(desc)
        {
        }
    };



    /// Base class representing an OpenGL object.
    class OpenGLObject
    {
    public:

        /// Constructor.
        OpenGLObject(GLuint objectGL)
            : m_objectGL(objectGL)
        {
        }

        /// Retrieves the OpenGL object.
        GLuint GetOpenGLObject() const { return m_objectGL; }


    private:

        /// The OpenGL object.
        GLuint m_objectGL;
    };




    //////////////////////////////////////////////////
    // Shader_OpenGL4

    /// OpenGL 4 shader program.
    class Shader_OpenGL4 : public ReferenceCountedObject, public OpenGLObject
    {
    public:

        /// Constructor.
        Shader_OpenGL4(GLuint objectGL, GLenum typeGL)
            : ReferenceCountedObject(), OpenGLObject(objectGL),
              m_typeGL(typeGL)
        {
        }


        /// Retrieves the shader type.
        GLenum GetOpenGLShaderType() const { return m_typeGL; }


    private:

        /// The shader type as an OpenGL type.
        GLenum m_typeGL;
    };



    //////////////////////////////////////////////////
    // Buffer_OpenGL4

    /// Class representing an OpenGL buffer object.
    class Buffer_OpenGL4 : public ReferenceCountedObject, public OpenGLObject
    {
    public:

        /// Constructor.
        Buffer_OpenGL4(GLuint objectGL, GLbitfield flagsGL, GLsizeiptr sizeInBytes)
            : ReferenceCountedObject(), OpenGLObject(objectGL), m_flagsGL(flagsGL), m_sizeInBytes(sizeInBytes)
        {
        }


        /// Retrieves the flags that were used to create the buffer.
        GLenum GetOpenGLFlags() const { return m_flagsGL; }

        /// Retrieves the size of the buffer in bytes.
        GLsizeiptr GetSizeInBytes() const { return m_sizeInBytes; }



    private:

        /// The flags that were used to create the buffer.
        GLenum m_flagsGL;

        /// The size of the buffer in bytes. This is needed in order to know how many bytes to specify when mapping the entire buffer.
        GLsizeiptr m_sizeInBytes;
    };


    ///////////////////////////////////////////////////
    // Texture_OpenGL4

    class Texture_OpenGL4 : public ReferenceCountedObject, public OpenGLObject
    {
    public:

        Texture_OpenGL4(GLuint objectGL, TextureFormat format)
            : ReferenceCountedObject(), OpenGLObject(objectGL),
              m_format(format)
        {
        }

        TextureFormat GetFormat() const { return m_format; }


    private:

        /// The image format.
        TextureFormat m_format;
    };


    ///////////////////////////////////////////////////
    // TextureView_OpenGL4

    class TextureView_OpenGL4 : public ReferenceCountedObject, public OpenGLObject
    {
    public:

        TextureView_OpenGL4(GLuint objectGL)
            : ReferenceCountedObject(), OpenGLObject(objectGL)
        {
        }
    };


    //////////////////////////////////////////////////
    // Sampler_OpenGL4

    class Sampler_OpenGL4 : public ReferenceCountedObject, public OpenGLObject
    {
    public:

        Sampler_OpenGL4(GLuint objectGL)
            : ReferenceCountedObject(), OpenGLObject(objectGL)
        {
        }
    };



    //////////////////////////////////////////////////
    // InputLayout_OpenGL4

    /// OpenGL input layout object.
    class InputLayout_OpenGL4 : public ReferenceCountedObject
    {
    public:

        /// Structure describing an attribute.
        struct AttributeDesc
        {
            unsigned int           slotIndex;
            GLint                  attribLocation;
            GLint                  attribComponentCount;
            GLenum                 attribComponentType;
            GLsizei                attribOffset;
            GPUInputClassification attributeClass;            // Per-Vertex or Per-Instance
            unsigned int           instanceStepRate;
        };



    public:

        /// Constructor.
        InputLayout_OpenGL4(AttributeDesc* const attributes, size_t attributeCount, unsigned int slotAttributeCounts[GT_GE_MAX_VERTEX_BUFFER_SLOTS]);

        /// Destructor.
        ~InputLayout_OpenGL4();


        /// Retrieves the number of vertex attribute count.
        ///
        /// @return The number of attributes making up this vertex input layout.
        size_t GetAttributeCount() const;

        /// Retrieves a reference to the attribute descriptor for the attribute at the given index.
        ///
        /// @param attributeIndex [in] The index of the attribute descriptor to retrieve.
        ///
        /// @return A reference to the attribute descriptor at the given index.
        const AttributeDesc & GetAttribute(size_t attributeIndex) const;

        
        /// Retrieves the range of indices that make up attributes that are bound to the buffer on the given slot.
        ///
        /// @param slotIndex     [in]  The index of the slot whose attribute range is being retrieved.
        /// @param rangeStartOut [out] The index of the first element in the range.
        /// @param rangeEndOut   [out] The index of the last attribute in the range, plus 1.
        ///
        /// @remarks
        ///     If rangeStart and rangeEnd are the same value, it means there are no attributes bound to the given slot.
        void GetSlotAttributeRange(unsigned int slotIndex, size_t &rangeStartOut, size_t &rangeEndOut) const;


    private:

        /// The list of attribute descriptors. This is immutable.
        AttributeDesc* const m_attributes;

        /// The number of attributes containing within this layout object. This is immutable.
        size_t m_attributeCount;

        /// m_attributes will contain a list of vertex attributes, sorted by slot. Each item in this list corresponds to a slot, and contains the index of the
        /// first attribute for the respective slot, and the index + 1 of the last attribute for the slot.
        uint32_t m_slotAttributeRanges[GT_GE_MAX_VERTEX_BUFFER_SLOTS];


    private:    // No copying.
        InputLayout_OpenGL4(const InputLayout_OpenGL4 &);
        InputLayout_OpenGL4 & operator=(const InputLayout_OpenGL4 &);
    };




    static const GLenum g_GLTextureFormatsTable[] =
    {
        0,                                              // TextureFormat_Unknown = 0

        // Unsigned Normalized.
        GL_R8,                                          // TextureFormat_R8
        GL_RG8,                                         // TextureFormat_RG8
        GL_RGBA8,                                       // TextureFormat_RGBA8
        GL_R16,                                         // TextureFormat_R16
        GL_RG16,                                        // TextureFormat_RG16
        GL_RGBA16,                                      // TextureFormat_RGBA16
        GL_RGB10_A2,                                    // TextureFormat_RGB10_A2
        GL_SRGB_ALPHA,                                  // TextureFormat_SRGB8_A8

        // Signed Normalized.
        GL_R8_SNORM,                                    // TextureFormat_R8_SNORM
        GL_RG8_SNORM,                                   // TextureFormat_RG8_SNORM
        GL_RGBA8_SNORM,                                 // TextureFormat_RGBA8_SNORM
        GL_R16_SNORM,                                   // TextureFormat_R16_SNORM
        GL_RG16_SNORM,                                  // TextureFormat_RG16_SNORM
        GL_RGBA16_SNORM,                                // TextureFormat_RGBA16_SNORM

        // Unsigned Integral.
        GL_R8UI,                                        // TextureFormat_R8UI
        GL_RG8UI,                                       // TextureFormat_RG8UI
        GL_RGBA8UI,                                     // TextureFormat_RGBA8UI
        GL_R16UI,                                       // TextureFormat_R16UI
        GL_RG16UI,                                      // TextureFormat_RG16UI
        GL_RGBA16UI,                                    // TextureFormat_RGBA16UI
        GL_R32UI,                                       // TextureFormat_R32UI
        GL_RG32UI,                                      // TextureFormat_RG32UI
        GL_RGB32UI,                                     // TextureFormat_RGB32UI
        GL_RGBA32UI,                                    // TextureFormat_RGBA32UI

        // Signed Integral.
        GL_R8I,                                         // TextureFormat_RBSI
        GL_RG8I,                                        // TextureFormat_RG8SI
        GL_RGBA8I,                                      // TextureFormat_RGBA8SI
        GL_R16I,                                        // TextureFormat_R16SI
        GL_RG16I,                                       // TextureFormat_RG16SI
        GL_RGBA16I,                                     // TextureFormat_RGBA16SI
        GL_R32I,                                        // TextureFormat_R32SI
        GL_RG32I,                                       // TextureFormat_RG32SI
        GL_RGB32I,                                      // TextureFormat_RGB32SI
        GL_RGBA32I,                                     // TextureFormat_RGBA32SI

        // Float
        GL_R16F,                                        // TextureFormat_R16F
        GL_RG16F,                                       // TextureFormat_RG16F
        GL_RGBA16F,                                     // TextureFormat_RGBA16F
        GL_R32F,                                        // TextureFormat_R32F
        GL_RG32F,                                       // TextureFormat_RG32F
        GL_RGB32F,                                      // TextureFormat_RGB32F
        GL_RGBA32F,                                     // TextureFormat_RGBA32F
        GL_R11F_G11F_B10F,                              // TextureFormat_R11G11B10F

        // Special
        GL_DEPTH24_STENCIL8,                            // TextureFormat_D24_S8               //< GL_DEPTH24_STENCIL8 / DXGI_FORMAT_D24_UNORM_S8_UINT

        // Compressed.
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,               // TextureFormat_RGBA_DXT1            //< GL_COMPRESSED_RGBA_S3TC_DXT1_EXT       / DXGI_FORMAT_BC1_UNORM
        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,               // TextureFormat_RGBA_DXT3            //< GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       / DXGI_FORMAT_BC2_UNORM
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,               // TextureFormat_RGBA_DXT5            //< GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       / DXGI_FORMAT_BC3_UNORM
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,         // TextureFormat_SRGB_Alpha_DXT1      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT / DXGI_FORMAT_BC1_UNORM_SRGB
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,         // TextureFormat_SRGB_Alpha_DXT3      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT / DXGI_FORMAT_BC2_UNORM_SRGB
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,         // TextureFormat_SRGB_Alpha_DXT5      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT / DXGI_FORMAT_BC3_UNORM_SRGB

        GL_COMPRESSED_RED_RGTC1,                        // TextureFormat_R_RGTC               //< GL_COMPRESSED_RED_RGTC1        / DXGI_FORMAT_BC4_UNORM
        GL_COMPRESSED_SIGNED_RED_RGTC1,                 // TextureFormat_R_RGTC_SNORM         //< GL_COMPRESSED_SIGNED_RED_RGTC1 / DXGI_FORMAT_BC4_SNORM
        GL_COMPRESSED_RG_RGTC2,                         // TextureFormat_RG_RGTC              //< GL_COMPRESSED_RG_RGTC2         / DXGI_FORMAT_BC5_UNORM
        GL_COMPRESSED_SIGNED_RG_RGTC2,                  // TextureFormat_RG_RGTC_SNORM        //< GL_COMPRESSED_SIGNED_RG_RGTC2  / DXGI_FORMAT_BC5_SNORM

        GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,              // TextureFormat_RGBA_BPTC            //< GL_COMPRESSED_RGBA_BPTC_UNORM_ARB         / DXGI_FORMAT_BC7_UNORM
        GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,        // TextureFormat_SRGB_Alpha_BPTC      //< GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB   / DXGI_FORMAT_BC7_UNORM_SRGB
        GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,      // TextureFormat_RGB_UF16_BPTC        //< GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB / DXGI_FORMAT_BC6H_UF16
        GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB         // TextureFormat_RGB_SF16_BPTC        //< GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB   / DXGI_FORMAT_BC6H_SF16
    };

    static const GLenum g_GLTextureSubImageFormatsTable[] =
    {
        0,                                              // TextureFormat_Unknown = 0

        // Unsigned Normalized.
        GL_RED,                                          // TextureFormat_R8
        GL_RG,                                         // TextureFormat_RG8
        GL_RGBA,                                       // TextureFormat_RGBA8
        GL_RED,                                         // TextureFormat_R16
        GL_RG,                                        // TextureFormat_RG16
        GL_RGBA,                                      // TextureFormat_RGBA16
        GL_RGBA,                                    // TextureFormat_RGB10_A2
        GL_RGBA,                                  // TextureFormat_SRGB8_A8

        // Signed Normalized.
        GL_RED,                                    // TextureFormat_R8_SNORM
        GL_RG,                                   // TextureFormat_RG8_SNORM
        GL_RGBA,                                 // TextureFormat_RGBA8_SNORM
        GL_RED,                                   // TextureFormat_R16_SNORM
        GL_RG,                                  // TextureFormat_RG16_SNORM
        GL_RGBA,                                // TextureFormat_RGBA16_SNORM

        // Unsigned Integral.
        GL_RED_INTEGER,                                        // TextureFormat_R8UI
        GL_RG_INTEGER,                                       // TextureFormat_RG8UI
        GL_RGBA_INTEGER,                                     // TextureFormat_RGBA8UI
        GL_RED_INTEGER,                                       // TextureFormat_R16UI
        GL_RG_INTEGER,                                      // TextureFormat_RG16UI
        GL_RGBA_INTEGER,                                    // TextureFormat_RGBA16UI
        GL_RED_INTEGER,                                       // TextureFormat_R32UI
        GL_RG_INTEGER,                                      // TextureFormat_RG32UI
        GL_RGB_INTEGER,                                     // TextureFormat_RGB32UI
        GL_RGBA_INTEGER,                                    // TextureFormat_RGBA32UI

        // Signed Integral.
        GL_RED_INTEGER,                                         // TextureFormat_RBSI
        GL_RG_INTEGER,                                        // TextureFormat_RG8SI
        GL_RGBA_INTEGER,                                      // TextureFormat_RGBA8SI
        GL_RED_INTEGER,                                        // TextureFormat_R16SI
        GL_RG_INTEGER,                                       // TextureFormat_RG16SI
        GL_RGBA_INTEGER,                                     // TextureFormat_RGBA16SI
        GL_RED_INTEGER,                                        // TextureFormat_R32SI
        GL_RG_INTEGER,                                       // TextureFormat_RG32SI
        GL_RGB_INTEGER,                                      // TextureFormat_RGB32SI
        GL_RGBA_INTEGER,                                     // TextureFormat_RGBA32SI

        // Float
        GL_RED,                                        // TextureFormat_R16F
        GL_RG,                                       // TextureFormat_RG16F
        GL_RGBA,                                     // TextureFormat_RGBA16F
        GL_RED,                                        // TextureFormat_R32F
        GL_RG,                                       // TextureFormat_RG32F
        GL_RGB,                                      // TextureFormat_RGB32F
        GL_RGBA,                                     // TextureFormat_RGBA32F
        GL_RGB,                              // TextureFormat_R11G11B10F


        // Special
        GL_DEPTH_STENCIL,                            // TextureFormat_D24_S8               //< GL_DEPTH24_STENCIL8 / DXGI_FORMAT_D24_UNORM_S8_UINT

        // Compressed.
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,               // TextureFormat_RGBA_DXT1            //< GL_COMPRESSED_RGBA_S3TC_DXT1_EXT       / DXGI_FORMAT_BC1_UNORM
        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,               // TextureFormat_RGBA_DXT3            //< GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       / DXGI_FORMAT_BC2_UNORM
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,               // TextureFormat_RGBA_DXT5            //< GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       / DXGI_FORMAT_BC3_UNORM
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,         // TextureFormat_SRGB_Alpha_DXT1      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT / DXGI_FORMAT_BC1_UNORM_SRGB
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,         // TextureFormat_SRGB_Alpha_DXT3      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT / DXGI_FORMAT_BC2_UNORM_SRGB
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,         // TextureFormat_SRGB_Alpha_DXT5      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT / DXGI_FORMAT_BC3_UNORM_SRGB

        GL_COMPRESSED_RED_RGTC1,                        // TextureFormat_R_RGTC               //< GL_COMPRESSED_RED_RGTC1        / DXGI_FORMAT_BC4_UNORM
        GL_COMPRESSED_SIGNED_RED_RGTC1,                 // TextureFormat_R_RGTC_SNORM         //< GL_COMPRESSED_SIGNED_RED_RGTC1 / DXGI_FORMAT_BC4_SNORM
        GL_COMPRESSED_RG_RGTC2,                         // TextureFormat_RG_RGTC              //< GL_COMPRESSED_RG_RGTC2         / DXGI_FORMAT_BC5_UNORM
        GL_COMPRESSED_SIGNED_RG_RGTC2,                  // TextureFormat_RG_RGTC_SNORM        //< GL_COMPRESSED_SIGNED_RG_RGTC2  / DXGI_FORMAT_BC5_SNORM

        GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,              // TextureFormat_RGBA_BPTC            //< GL_COMPRESSED_RGBA_BPTC_UNORM_ARB         / DXGI_FORMAT_BC7_UNORM
        GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,        // TextureFormat_SRGB_Alpha_BPTC      //< GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB   / DXGI_FORMAT_BC7_UNORM_SRGB
        GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,      // TextureFormat_RGB_UF16_BPTC        //< GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB / DXGI_FORMAT_BC6H_UF16
        GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB         // TextureFormat_RGB_SF16_BPTC        //< GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB   / DXGI_FORMAT_BC6H_SF16
    };

    static const GLenum g_GLTextureSubImageTypesTable[] =
    {
        0,                                              // TextureFormat_Unknown = 0

        // Unsigned Normalized.
        GL_UNSIGNED_BYTE,                                          // TextureFormat_R8
        GL_UNSIGNED_BYTE,                                         // TextureFormat_RG8
        GL_UNSIGNED_BYTE,                                       // TextureFormat_RGBA8
        GL_UNSIGNED_SHORT,                                         // TextureFormat_R16
        GL_UNSIGNED_SHORT,                                        // TextureFormat_RG16
        GL_UNSIGNED_SHORT,                                      // TextureFormat_RGBA16
        GL_UNSIGNED_INT_10_10_10_2,                                    // TextureFormat_RGB10_A2
        GL_UNSIGNED_BYTE,                                  // TextureFormat_SRGB8_A8

        // Signed Normalized.
        GL_BYTE,                                    // TextureFormat_R8_SNORM
        GL_BYTE,                                   // TextureFormat_RG8_SNORM
        GL_BYTE,                                 // TextureFormat_RGBA8_SNORM
        GL_SHORT,                                   // TextureFormat_R16_SNORM
        GL_SHORT,                                  // TextureFormat_RG16_SNORM
        GL_SHORT,                                // TextureFormat_RGBA16_SNORM

        // Unsigned Integral.
        GL_UNSIGNED_BYTE,                                        // TextureFormat_R8UI
        GL_UNSIGNED_BYTE,                                       // TextureFormat_RG8UI
        GL_UNSIGNED_BYTE,                                     // TextureFormat_RGBA8UI
        GL_UNSIGNED_SHORT,                                       // TextureFormat_R16UI
        GL_UNSIGNED_SHORT,                                      // TextureFormat_RG16UI
        GL_UNSIGNED_SHORT,                                    // TextureFormat_RGBA16UI
        GL_UNSIGNED_INT,                                       // TextureFormat_R32UI
        GL_UNSIGNED_INT,                                      // TextureFormat_RG32UI
        GL_UNSIGNED_INT,                                     // TextureFormat_RGB32UI
        GL_UNSIGNED_INT,                                    // TextureFormat_RGBA32UI

        // Signed Integral.
        GL_BYTE,                                         // TextureFormat_RBSI
        GL_BYTE,                                        // TextureFormat_RG8SI
        GL_BYTE,                                      // TextureFormat_RGBA8SI
        GL_SHORT,                                        // TextureFormat_R16SI
        GL_SHORT,                                       // TextureFormat_RG16SI
        GL_SHORT,                                     // TextureFormat_RGBA16SI
        GL_INT,                                        // TextureFormat_R32SI
        GL_INT,                                       // TextureFormat_RG32SI
        GL_INT,                                      // TextureFormat_RGB32SI
        GL_INT,                                     // TextureFormat_RGBA32SI

        // Float
        GL_HALF_FLOAT,                                        // TextureFormat_R16F
        GL_HALF_FLOAT,                                       // TextureFormat_RG16F
        GL_HALF_FLOAT,                                     // TextureFormat_RGBA16F
        GL_HALF_FLOAT,                                        // TextureFormat_R32F
        GL_FLOAT,                                       // TextureFormat_RG32F
        GL_FLOAT,                                      // TextureFormat_RGB32F
        GL_FLOAT,                                     // TextureFormat_RGBA32F
        GL_UNSIGNED_INT_10F_11F_11F_REV,                              // TextureFormat_R11G11B10F

        // Special
        0,                            // TextureFormat_D24_S8               //< GL_DEPTH24_STENCIL8 / DXGI_FORMAT_D24_UNORM_S8_UINT

        // Compressed.
        0,               // TextureFormat_RGBA_DXT1            //< GL_COMPRESSED_RGBA_S3TC_DXT1_EXT       / DXGI_FORMAT_BC1_UNORM
        0,               // TextureFormat_RGBA_DXT3            //< GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       / DXGI_FORMAT_BC2_UNORM
        0,               // TextureFormat_RGBA_DXT5            //< GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       / DXGI_FORMAT_BC3_UNORM
        0,         // TextureFormat_SRGB_Alpha_DXT1      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT / DXGI_FORMAT_BC1_UNORM_SRGB
        0,         // TextureFormat_SRGB_Alpha_DXT3      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT / DXGI_FORMAT_BC2_UNORM_SRGB
        0,         // TextureFormat_SRGB_Alpha_DXT5      //< GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT / DXGI_FORMAT_BC3_UNORM_SRGB

        0,                        // TextureFormat_R_RGTC               //< GL_COMPRESSED_RED_RGTC1        / DXGI_FORMAT_BC4_UNORM
        0,                 // TextureFormat_R_RGTC_SNORM         //< GL_COMPRESSED_SIGNED_RED_RGTC1 / DXGI_FORMAT_BC4_SNORM
        0,                         // TextureFormat_RG_RGTC              //< GL_COMPRESSED_RG_RGTC2         / DXGI_FORMAT_BC5_UNORM
        0,                  // TextureFormat_RG_RGTC_SNORM        //< GL_COMPRESSED_SIGNED_RG_RGTC2  / DXGI_FORMAT_BC5_SNORM

        0,              // TextureFormat_RGBA_BPTC            //< GL_COMPRESSED_RGBA_BPTC_UNORM_ARB         / DXGI_FORMAT_BC7_UNORM
        0,        // TextureFormat_SRGB_Alpha_BPTC      //< GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB   / DXGI_FORMAT_BC7_UNORM_SRGB
        0,      // TextureFormat_RGB_UF16_BPTC        //< GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB / DXGI_FORMAT_BC6H_UF16
        0         // TextureFormat_RGB_SF16_BPTC        //< GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB   / DXGI_FORMAT_BC6H_SF16
    };
}

#endif

#endif
