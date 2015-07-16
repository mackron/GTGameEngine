// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/DefaultGraphicsWorld_OpenGL.hpp>
#include <GTGameEngine/Graphics/GraphicsAPI_OpenGL.hpp>

namespace GT
{
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




    static const uint32_t RTFlag_IsWindow   = (1U << 31);
    static const uint32_t RTFlag_IsTexture  = (1U << 30);
    static const uint32_t RTFlag_IsDisabled = (1U << 29);

    struct RenderTarget_OpenGL
    {
        RenderTarget_OpenGL(uint32_t flagsIn)
            : flags(flagsIn), priority(0), projection(), view(), hSurface(0)
        {
        }

        /// Flags describing the render target.
        uint32_t flags;

        /// The priority.
        int priority;

        /// The projection matrix to use when rendering to the render target.
        mat4 projection;

        /// The view matrix to use when rendering to the render target.
        mat4 view;

        /// The GUI surface to draw to this render target.
        HGUISurface hSurface;
    };

    struct WindowRenderTarget_OpenGL : public RenderTarget_OpenGL
    {
        WindowRenderTarget_OpenGL(HWND hWndIn, HDC hDCIn, uint32_t flagsIn)
            : RenderTarget_OpenGL(flagsIn),
              hWnd(hWndIn), hDC(hDCIn)
        {
        }

#if defined(GT_PLATFORM_WINDOWS)
        /// A handle to the window.
        HWND hWnd;

        /// The device context of the window.
        HDC hDC;
#endif
    };

    struct TextureRenderTarget_OpenGL : public RenderTarget_OpenGL
    {
        TextureRenderTarget_OpenGL(HGraphicsResource hTextureIn, uint32_t flagsIn)
            : RenderTarget_OpenGL(flagsIn),
              hTexture(hTextureIn)
        {
        }

        /// A handle to the texture resource.
        HGraphicsResource hTexture;
    };



    struct Texture_OpenGL
    {
        Texture_OpenGL(GLuint objectGLIn, unsigned int widthIn, unsigned int heightIn, TextureFormat formatIn)
            : objectGL(objectGLIn), width(widthIn), height(heightIn), format(formatIn)
        {
        }


        /// The OpenGL object.
        GLuint objectGL;

        /// The width of the texture.
        unsigned int width;

        /// The height of the texture.
        unsigned int height;

        /// The data format.
        TextureFormat format;
    };



    DefaultGraphicsWorld_OpenGL::DefaultGraphicsWorld_OpenGL(GUIContext &gui, GraphicsAPI_OpenGL &gl)
        : GraphicsWorld(gui),
          m_gl(gl)
    {
    }

    DefaultGraphicsWorld_OpenGL::~DefaultGraphicsWorld_OpenGL()
    {
    }


    bool DefaultGraphicsWorld_OpenGL::Startup()
    {
#if defined(GT_PLATFORM_WINDOWS)
        m_hRC = m_gl.CreateContext(m_gl.GetDummyDC());
        if (m_hRC != 0)
        {
            if (m_gl.MakeCurrent(m_gl.GetDummyDC(), m_hRC))
            {
                return true;
            }
            else
            {
                // Failed to make the context current.
                m_gl.DeleteContext(m_hRC);
                m_hRC = 0;
            }
        }
#endif

        return false;
    }

    void DefaultGraphicsWorld_OpenGL::Shutdown()
    {
        // Delete the context last.
#if defined(GT_PLATFORM_WINDOWS)
        m_gl.DeleteContext(m_hRC);
#endif
    }



    ////////////////////
    // Resources

    HGraphicsResource DefaultGraphicsWorld_OpenGL::CreateTextureResource(unsigned int width, unsigned int height, unsigned int depth, TextureFormat format, const void* pData)
    {
        assert(width  > 0);
        assert(height > 0);
        assert(depth  > 0);


        this->MakeOpenGLContextCurrent();

        GLuint objectGL;
        m_gl.GenTextures(1, &objectGL);
        
        GLenum internalFormatGL = g_GLTextureFormatsTable[format];
        GLenum formatGL         = g_GLTextureSubImageFormatsTable[format];
        GLenum typeGL           = g_GLTextureSubImageTypesTable[format];
        if (depth == 1)
        {
            if (height == 1)
            {
                // 1D
                m_gl.BindTexture(GL_TEXTURE_1D, objectGL);
                m_gl.TexImage1D(GL_TEXTURE_1D, 0, internalFormatGL, width, 0, formatGL, typeGL, pData);
            }
            else
            {
                // 2D
                m_gl.BindTexture(GL_TEXTURE_2D, objectGL);
                m_gl.TexImage2D(GL_TEXTURE_2D, 0, internalFormatGL, width, height, 0, formatGL, typeGL, pData);
            }
        }
        else
        {
            // 3D
            m_gl.BindTexture(GL_TEXTURE_3D, objectGL);
            m_gl.TexImage3D(GL_TEXTURE_3D, 0, internalFormatGL, width, height, depth, 0, formatGL, typeGL, pData);
        }



        return 0;
    }

    HGraphicsResource DefaultGraphicsWorld_OpenGL::CreateMaterialResource()
    {
        this->MakeOpenGLContextCurrent();

        return 0;
    }

    HGraphicsResource DefaultGraphicsWorld_OpenGL::CreateMeshResource()
    {
        this->MakeOpenGLContextCurrent();

        return 0;
    }

    void DefaultGraphicsWorld_OpenGL::DeleteResource(HGraphicsResource hResource)
    {
        this->MakeOpenGLContextCurrent();

        (void)hResource;
    }



    ////////////////////
    // Objects

    HGraphicsObject DefaultGraphicsWorld_OpenGL::CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        (void)hMeshResource;
        (void)position;
        (void)rotation;
        (void)scale;

        return 0;
    }

    void DefaultGraphicsWorld_OpenGL::DeleteObject(HGraphicsObject hObject)
    {
        (void)hObject;
    }



    ////////////////////
    // Render Targets

#if defined(GT_PLATFORM_WINDOWS)
    HGraphicsRenderTarget DefaultGraphicsWorld_OpenGL::CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags)
    {
        if (hWnd != 0)
        {
            HDC hDC = ::GetDC(hWnd);
            if (hDC != 0)
            {
                if (::SetPixelFormat(hDC, m_gl.GetPixelFormat(), &m_gl.GetPFD()))
                {
                    HGraphicsRenderTarget hRT = reinterpret_cast<HGraphicsRenderTarget>(new WindowRenderTarget_OpenGL(hWnd, hDC, flags | RTFlag_IsWindow));
                    if (hRT != 0)
                    {
                        // Insert the render target into the list and sort.
                        //
                        // TODO: Optimize this so that we just find the appropriate insertion index rather than doing an explicit sort.
                        m_windowRTs.PushBack(hRT);
                        this->SortWindowRenderTargets();

                        return hRT;
                    }
                }
            }
        }

        return 0;
    }
#endif

    HGraphicsRenderTarget DefaultGraphicsWorld_OpenGL::CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags)
    {
        this->MakeOpenGLContextCurrent();

        HGraphicsRenderTarget hRT = reinterpret_cast<HGraphicsRenderTarget>(new TextureRenderTarget_OpenGL(hTextureResource, flags | RTFlag_IsTexture));
        if (hRT != 0)
        {
            // Insert the render target into the list and sort.
            //
            // TODO: Optimize this so that we just find the appropriate insertion index rather than doing an explicit sort.
            m_textureRTs.PushBack(hRT);
            this->SortTextureRenderTargets();

            return hRT;
        }
        

        return 0;
    }

    void DefaultGraphicsWorld_OpenGL::DeleteRenderTarget(HGraphicsResource hRT)
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            if ((pRT->flags & RTFlag_IsWindow) != 0)
            {
                this->DeleteWindowRenderTarget(hRT);
            }
            else if ((pRT->flags & RTFlag_IsTexture) != 0)
            {
                this->DeleteTextureRenderTarget(hRT);
            }
        }
    }


    void DefaultGraphicsWorld_OpenGL::SetRenderTargetPriority(HGraphicsRenderTarget hRT, int priority)
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            if (pRT->priority != priority)
            {
                pRT->priority = priority;

                if ((pRT->flags & RTFlag_IsWindow) != 0)
                {
                    this->SortWindowRenderTargets();
                }
                else if ((pRT->flags & RTFlag_IsTexture) != 0)
                {
                    this->SortTextureRenderTargets();
                }
            }
        }
    }

    int DefaultGraphicsWorld_OpenGL::GetRenderTargetPriority(HGraphicsRenderTarget hRT) const
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            return pRT->priority;
        }

        return 0;
    }


    void DefaultGraphicsWorld_OpenGL::EnableRenderTarget(HGraphicsRenderTarget hRT)
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            pRT->flags &= ~RTFlag_IsDisabled;
        }
    }

    void DefaultGraphicsWorld_OpenGL::DisableRenderTarget(HGraphicsRenderTarget hRT)
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            pRT->flags |= RTFlag_IsDisabled;
        }
    }

    bool DefaultGraphicsWorld_OpenGL::IsRenderTargetEnabled(HGraphicsRenderTarget hRT) const
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            return (pRT->flags & RTFlag_IsDisabled) == 0;
        }

        return false;
    }


    void DefaultGraphicsWorld_OpenGL::SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const mat4 &projection, const mat4 &view)
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            pRT->projection = projection;
            pRT->view       = view;
        }
    }

    void DefaultGraphicsWorld_OpenGL::GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            projectionOut = pRT->projection;
            viewOut       = pRT->view;
        }
    }


    void DefaultGraphicsWorld_OpenGL::SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface)
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            pRT->hSurface = hSurface;
        }
    }

    HGUISurface DefaultGraphicsWorld_OpenGL::GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            return pRT->hSurface;
        }

        return 0;
    }



    ////////////////////
    // Rendering

    bool DefaultGraphicsWorld_OpenGL::IsCommandBuffersSupported() const
    {
        // Command buffers are not supported.
        return false;
    }

    void DefaultGraphicsWorld_OpenGL::BuildCommandBuffers()
    {
        // Command buffers are not supported.
        assert(false);
    }

    void DefaultGraphicsWorld_OpenGL::ExecuteCommandBuffers()
    {
        // Command buffers are not supported.
        assert(false);
    }
        
    void DefaultGraphicsWorld_OpenGL::ExecuteRenderingCommands()
    {
        // We need to iterate over each render target and render the world from the perspective of each of the enabled ones. Render targets are
        // rendered based on their priorities. Texture render targets are always rendered before window render targets.
        
        // Texture RTs
        for (size_t iRT = 0; iRT < m_textureRTs.GetCount(); ++iRT)
        {
            HGraphicsRenderTarget hRT = m_textureRTs[iRT];
            assert(hRT != 0);
            {
                if (this->IsRenderTargetEnabled(hRT))
                {
                    this->ExecuteTextureRTRenderingCommands(hRT);
                }
            }
        }

        // Window RTs
        for (size_t iRT = 0; iRT < m_windowRTs.GetCount(); ++iRT)
        {
            HGraphicsRenderTarget hRT = m_windowRTs[iRT];
            assert(hRT != 0);
            {
                if (this->IsRenderTargetEnabled(hRT))
                {
                    this->ExecuteWindowRTRenderingCommands(hRT);
                }
            }
        }
    }



    ////////////////////////////////////////////////
    // Private

    void DefaultGraphicsWorld_OpenGL::MakeOpenGLContextCurrent()
    {
#if defined(GT_PLATFORM_WINDOWS)
        HDC hDC = m_gl.GetCurrentDC();
        if (hDC == 0) {
            hDC = m_gl.GetDummyDC();
        }

        m_gl.MakeCurrent(hDC, m_hRC);
#endif
    }

    void DefaultGraphicsWorld_OpenGL::DeleteWindowRenderTarget(HGraphicsRenderTarget hRT)
    {
        auto pRT = reinterpret_cast<WindowRenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            m_windowRTs.RemoveFirstOccuranceOf(hRT);
            delete pRT;
        }
    }

    void DefaultGraphicsWorld_OpenGL::DeleteTextureRenderTarget(HGraphicsRenderTarget hRT)
    {
        auto pRT = reinterpret_cast<TextureRenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            m_textureRTs.RemoveFirstOccuranceOf(hRT);
            delete pRT;
        }
    }

    void DefaultGraphicsWorld_OpenGL::SortWindowRenderTargets()
    {
        m_windowRTs.Sort([](const HGraphicsRenderTarget &a, const HGraphicsRenderTarget &b) -> bool {
            return reinterpret_cast<const RenderTarget_OpenGL*>(a)->priority > reinterpret_cast<const RenderTarget_OpenGL*>(b)->priority;
        });
    }

    void DefaultGraphicsWorld_OpenGL::SortTextureRenderTargets()
    {
        m_textureRTs.Sort([](const HGraphicsRenderTarget &a, const HGraphicsRenderTarget &b) -> bool {
            return reinterpret_cast<const RenderTarget_OpenGL*>(a)->priority > reinterpret_cast<const RenderTarget_OpenGL*>(b)->priority;
        });
    }

    void DefaultGraphicsWorld_OpenGL::ExecuteWindowRTRenderingCommands(HGraphicsRenderTarget hRT)
    {
        auto pRT = reinterpret_cast<WindowRenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            assert((pRT->flags & RTFlag_IsWindow) != 0);

            if (m_gl.MakeCurrent(pRT->hDC, m_hRC))
            {
                m_gl.ClearColor(0.75f, 0.25f, 0.25f, 1.0f);
                m_gl.Clear(GL_COLOR_BUFFER_BIT);

                SwapBuffers(pRT->hDC);
            }
        }
    }

    void DefaultGraphicsWorld_OpenGL::ExecuteTextureRTRenderingCommands(HGraphicsRenderTarget hRT)
    {
        auto pRT = reinterpret_cast<TextureRenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            assert((pRT->flags & RTFlag_IsTexture) != 0);
        }
    }
}
