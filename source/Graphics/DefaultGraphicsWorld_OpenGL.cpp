// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_OPENGL)
#include <GTGameEngine/Graphics/DefaultGraphicsWorld_OpenGL.hpp>

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
            : flags(flagsIn),
              viewportX(0), viewportY(0), viewportWidth(1), viewportHeight(1),
              priority(0), projection(), view(), hSurface(0)
        {
        }

        /// Flags describing the render target.
        uint32_t flags;

        /// The position of the viewport on the x axis.
        int viewportX;

        /// The position of the viewport on the y axis.
        int viewportY;

        /// The width of the viewport.
        unsigned int viewportWidth;

        /// The height of the viewport.
        unsigned int viewportHeight;

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




    /// Base class for resources.
    struct Resource_OpenGL
    {
        Resource_OpenGL(GraphicsResourceType typeIn)
            : type(typeIn)
        {
        }

        /// The resource type.
        GraphicsResourceType type;
    };

    /// Structure representing a texture resource.
    struct TextureResource_OpenGL : public Resource_OpenGL
    {
        TextureResource_OpenGL(GLuint objectGLIn, GLenum targetGLIn, unsigned int widthIn, unsigned int heightIn, unsigned int depthIn, TextureFormat formatIn)
            : Resource_OpenGL(GraphicsResourceType::Texture), 
              objectGL(objectGLIn), targetGL(targetGLIn),
              width(widthIn), height(heightIn), depth(depthIn),
              format(formatIn)
        {
        }


        /// The OpenGL object.
        GLuint objectGL;

        /// The texture target to use for OpenGL texture operations.
        GLenum targetGL;

        /// The width of the texture.
        unsigned int width;

        /// The height of the texture.
        unsigned int height;

        /// The depth of the texture.
        unsigned int depth;

        /// The data format.
        TextureFormat format;
    };

    /// Structure representing a material resource.
    struct MaterialResource_OpenGL : public Resource_OpenGL
    {
        MaterialResource_OpenGL()
            : Resource_OpenGL(GraphicsResourceType::Material)
        {
        }
    };

    /// Structure representing a mesh resource.
    struct MeshResource_OpenGL : public Resource_OpenGL
    {
        MeshResource_OpenGL(GLuint vertexBufferGLIn, GLuint indexBufferGLIn)
            : Resource_OpenGL(GraphicsResourceType::Mesh),
              vertexBufferGL(vertexBufferGLIn), indexBufferGL(indexBufferGLIn)
        {
        }

        /// The buffer object for the vertex data.
        GLuint vertexBufferGL;

        /// The buffer object for the index data.
        GLuint indexBufferGL;


        /// Structure representing a material slot in the mesh.
        struct MaterialSlot
        {
            /// The offset in the mesh's index buffer the polygons using this material slot begin at.
            GLsizei indexOffset;

            /// The number of indices to draw with this material.
            GLsizei indexCount;

            /// The default material.
            HGraphicsResource hDefaultMaterial;
        };

        /// The list of material slots.
        GTLib::Vector<MaterialSlot> materialSlots;
    };



    /// Structure representing an object in OpenGL.
    struct Object_OpenGL
    {
        Object_OpenGL(GraphicsObjectType typeIn)
            : type(typeIn),
              position(), rotation(), scale(1, 1, 1, 1)
        {
        }

        /// The type of the object.
        GraphicsObjectType type;

        /// The position of the object.
        vec4 position;

        /// The rotation of the object.
        quat rotation;

        /// The scale of the object.
        vec4 scale;
    };

    /// Structure representing a mesh object in OpenGL.
    struct MeshObject_OpenGL : public Object_OpenGL
    {
        MeshObject_OpenGL(HGraphicsResource hMeshResourceIn)
            : Object_OpenGL(GraphicsObjectType::Mesh),
              hMeshResource(hMeshResourceIn)
        {
        }

        /// The mesh resource this object is attached. to.
        HGraphicsResource hMeshResource;

        /// The list of materials to use for each material slot. The number of items in this array is the same as the number of material slots
        /// in the mesh resource. By default this is filled with the same materials as specified by the mesh definition, but they can be
        /// overwritten on a per-object basis.
        GTLib::Vector<HGraphicsResource> materials;
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
                // Default state.
                m_gl.FrontFace(GL_CW);
                m_gl.DepthFunc(GL_LEQUAL);
                m_gl.Enable(GL_DEPTH_TEST);
                m_gl.Enable(GL_CULL_FACE);

                // Testing Program.
                const char* vertexShaderSource =
                    "attribute vec3 VS_Position;\n"
                    "attribute vec2 VS_TexCoord;\n"
                    "attribute vec3 VS_Normal;\n"
                    "uniform mat4 Projection;\n"
                    "uniform mat4 View;\n"
                    "uniform mat4 Model;\n"
                    "varying vec2 FS_TexCoord;\n"
                    "varying vec3 FS_Normal;\n"
                    "void main()\n"
                    "{\n"
                    "    FS_TexCoord = VS_TexCoord;\n"
                    "    FS_Normal   = VS_Normal;\n"
                    "    \n"
                    "    gl_Position = Projection * View * Model * vec4(VS_Position, 1.0);\n"
                    "}";

                const char* fragmentShaderSource =
                    "varying vec2 FS_TexCoord;\n"
                    "varying vec3 FS_Normal;\n"
                    "void main()\n"
                    "{\n"
                    "    gl_FragColor = vec4(FS_Normal, 1.0);\n"
                    "}";

                GLuint vertexShader = m_gl.CreateShader(GL_VERTEX_SHADER);
                m_gl.ShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
                m_gl.CompileShader(vertexShader);

                GLint logLengthInBytesVertex;
                m_gl.GetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLengthInBytesVertex);
                if (logLengthInBytesVertex > 1)
                {
                    void* messageDst = malloc(logLengthInBytesVertex);
                    if (messageDst != nullptr)
                    {
                        m_gl.GetShaderInfoLog(vertexShader, logLengthInBytesVertex, &logLengthInBytesVertex, reinterpret_cast<GLchar*>(messageDst));
                        printf("%s\n", messageDst);
                    }
                    free(messageDst);
                }

                GLuint fragmentShader = m_gl.CreateShader(GL_FRAGMENT_SHADER);
                m_gl.ShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
                m_gl.CompileShader(fragmentShader);

                GLint logLengthInBytesFragment;
                m_gl.GetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLengthInBytesFragment);
                if (logLengthInBytesFragment > 1)
                {
                    void* messageDst = malloc(logLengthInBytesFragment);
                    if (messageDst != nullptr)
                    {
                        m_gl.GetShaderInfoLog(fragmentShader, logLengthInBytesFragment, &logLengthInBytesFragment, reinterpret_cast<GLchar*>(messageDst));
                        printf("%s\n", messageDst);
                    }
                    free(messageDst);
                }


                m_testProgramObjectGL = m_gl.CreateProgram();
                m_gl.AttachShader(m_testProgramObjectGL, vertexShader);
                m_gl.AttachShader(m_testProgramObjectGL, fragmentShader);
                m_gl.BindAttribLocation(m_testProgramObjectGL, 0, "VS_Position");
                m_gl.BindAttribLocation(m_testProgramObjectGL, 1, "VS_TexCoord");
                m_gl.BindAttribLocation(m_testProgramObjectGL, 2, "VS_Normal");
                m_gl.LinkProgram(m_testProgramObjectGL);

                GLint logLengthInBytesProgram;
                m_gl.GetProgramiv(m_testProgramObjectGL, GL_INFO_LOG_LENGTH, &logLengthInBytesProgram);
                if (logLengthInBytesProgram > 1)
                {
                    void* messageDst = malloc(logLengthInBytesProgram);
                    if (messageDst != nullptr)
                    {
                        m_gl.GetProgramInfoLog(m_testProgramObjectGL, logLengthInBytesProgram, &logLengthInBytesProgram, reinterpret_cast<GLchar*>(messageDst));
                        printf("%s\n", messageDst);
                    }
                    free(messageDst);
                }


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
        
        GLenum targetGL         = 0;
        GLenum internalFormatGL = g_GLTextureFormatsTable[format];
        GLenum formatGL         = g_GLTextureSubImageFormatsTable[format];
        GLenum typeGL           = g_GLTextureSubImageTypesTable[format];
        if (depth == 1)
        {
            if (height == 1)
            {
                targetGL = GL_TEXTURE_1D;
            }
            else
            {
                targetGL = GL_TEXTURE_2D;
            }
        }
        else
        {
            targetGL = GL_TEXTURE_3D;
        }


        m_gl.BindTexture(targetGL, objectGL);

        auto pTexture = new TextureResource_OpenGL(objectGL, targetGL, width, height, depth, format);
        switch (targetGL)
        {
        case GL_TEXTURE_1D:
            {
                m_gl.TexImage1D(targetGL, 0, internalFormatGL, width, 0, formatGL, typeGL, pData);
                break;
            }

        case GL_TEXTURE_2D:
            {
                m_gl.TexImage2D(targetGL, 0, internalFormatGL, width, height, 0, formatGL, typeGL, pData);
                break;
            }

        case GL_TEXTURE_3D:
            {
                m_gl.TexImage3D(targetGL, 0, internalFormatGL, width, height, depth, 0, formatGL, typeGL, pData);
                break;
            }

        default:
            {
                break;
            }
        }


        return reinterpret_cast<HGraphicsResource>(pTexture);
    }

    HGraphicsResource DefaultGraphicsWorld_OpenGL::CreateMaterialResource()
    {
        this->MakeOpenGLContextCurrent();

        return 0;
    }

    HGraphicsResource DefaultGraphicsWorld_OpenGL::CreateMeshResource(GraphicsMeshResourceDesc &meshDesc)
    {
        this->MakeOpenGLContextCurrent();

        // TODO: Normalize the format of the mesh so that every mesh is the same format.

        GLuint vertexBufferGL;
        m_gl.GenBuffers(1, &vertexBufferGL);
        m_gl.BindBuffer(GL_ARRAY_BUFFER, vertexBufferGL);
        m_gl.BufferData(GL_ARRAY_BUFFER, meshDesc.vertexDataSize, meshDesc.pVertexData, GL_STATIC_DRAW);

        GLuint indexBufferGL;
        m_gl.GenBuffers(1, &indexBufferGL);
        m_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferGL);
        m_gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, meshDesc.indexDataSize, meshDesc.pIndexData, GL_STATIC_DRAW);


        auto pMeshResource = new MeshResource_OpenGL(vertexBufferGL, indexBufferGL);

        for (unsigned int iMaterial = 0; iMaterial < meshDesc.materialCount; ++iMaterial)
        {
            const uint32_t* materialOffsetCountPair = meshDesc.materialIndexOffsetCountPairs + (iMaterial*2);
            assert(materialOffsetCountPair != nullptr);
            {
                MeshResource_OpenGL::MaterialSlot materialSlot;
                materialSlot.indexOffset      = materialOffsetCountPair[0];
                materialSlot.indexCount       = materialOffsetCountPair[1];
                materialSlot.hDefaultMaterial = 0;
                pMeshResource->materialSlots.PushBack(materialSlot);
            }
        }


        return reinterpret_cast<HGraphicsResource>(pMeshResource);
    }

    void DefaultGraphicsWorld_OpenGL::DeleteResource(HGraphicsResource hResource)
    {
        this->MakeOpenGLContextCurrent();

        auto pResource = reinterpret_cast<Resource_OpenGL*>(hResource);
        if (pResource != nullptr)
        {
            switch (pResource->type)
            {
            case GraphicsResourceType::Texture:
                {
                    this->DeleteTextureResource(hResource);
                    break;
                }

            case GraphicsResourceType::Material:
                {
                    this->DeleteMaterialResource(hResource);
                    break;
                }

            case GraphicsResourceType::Mesh:
                {
                    this->DeleteMeshResource(hResource);
                    break;
                }

            default: break;
            }
        }
    }



    ////////////////////
    // Objects

    HGraphicsObject DefaultGraphicsWorld_OpenGL::CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        auto pMeshResource = reinterpret_cast<MeshResource_OpenGL*>(hMeshResource);
        if (pMeshResource != nullptr)
        {
            auto pMeshObject = new MeshObject_OpenGL(hMeshResource);
            pMeshObject->position = position;
            pMeshObject->rotation = rotation;
            pMeshObject->scale    = scale;

            for (size_t iMaterial = 0; iMaterial < pMeshResource->materialSlots.GetCount(); ++iMaterial)
            {
                pMeshObject->materials.PushBack(pMeshResource->materialSlots[iMaterial].hDefaultMaterial);
            }


            HGraphicsObject hMeshObject = reinterpret_cast<HGraphicsObject>(pMeshObject);
            m_meshObjects.PushBack(hMeshObject);

            return hMeshObject;
        }

        return 0;
    }

    void DefaultGraphicsWorld_OpenGL::DeleteObject(HGraphicsObject hObject)
    {
        auto pObject = reinterpret_cast<Object_OpenGL*>(hObject);
        if (pObject != nullptr)
        {
            switch (pObject->type)
            {
            case GraphicsObjectType::Mesh:
                {
                    this->DeleteMeshObject(hObject);
                    break;
                }

            default:
                {
                    break;
                }
            }
        }
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
                    auto pRT = new WindowRenderTarget_OpenGL(hWnd, hDC, flags | RTFlag_IsWindow);
                    assert(pRT != nullptr);
                    {
                        RECT windowRect;
                        ::GetClientRect(hWnd, &windowRect);
                        pRT->viewportWidth  = windowRect.right  - windowRect.left;
                        pRT->viewportHeight = windowRect.bottom - windowRect.top;

                        HGraphicsRenderTarget hRT = reinterpret_cast<HGraphicsRenderTarget>(pRT);

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

    HGraphicsRenderTarget DefaultGraphicsWorld_OpenGL::GetRenderTargetByWindow(HWND hWnd) const
    {
        if (hWnd != 0)
        {
            for (size_t iRT = 0; iRT < m_windowRTs.GetCount(); ++iRT)
            {
                auto pRT = reinterpret_cast<WindowRenderTarget_OpenGL*>(m_windowRTs[iRT]);
                assert(pRT != nullptr);
                {
                    if (pRT->hWnd == hWnd)
                    {
                        return m_windowRTs[iRT];
                    }
                }
            }
        }

        return 0;
    }
#endif

    HGraphicsRenderTarget DefaultGraphicsWorld_OpenGL::CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags)
    {
        auto pTextureResource = reinterpret_cast<TextureResource_OpenGL*>(hTextureResource);
        if (pTextureResource != nullptr)
        {
            this->MakeOpenGLContextCurrent();

            auto pRT = new TextureRenderTarget_OpenGL(hTextureResource, flags | RTFlag_IsTexture);
            if (pRT != 0)
            {
                pRT->viewportWidth  = pTextureResource->width;
                pRT->viewportHeight = pTextureResource->height;

                HGraphicsRenderTarget hRT = reinterpret_cast<HGraphicsRenderTarget>(pRT);

                // Insert the render target into the list and sort.
                //
                // TODO: Optimize this so that we just find the appropriate insertion index rather than doing an explicit sort.
                m_textureRTs.PushBack(hRT);
                this->SortTextureRenderTargets();

                return hRT;
            }
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


    void DefaultGraphicsWorld_OpenGL::SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height)
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            pRT->viewportX      = x;
            pRT->viewportY      = y;
            pRT->viewportWidth  = width;
            pRT->viewportHeight = height;
        }
    }

    void DefaultGraphicsWorld_OpenGL::GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto pRT = reinterpret_cast<RenderTarget_OpenGL*>(hRT);
        if (pRT != nullptr)
        {
            xOut      = pRT->viewportX;
            yOut      = pRT->viewportY;
            widthOut  = pRT->viewportWidth;
            heightOut = pRT->viewportHeight;
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
                m_gl.Viewport(pRT->viewportX, pRT->viewportY, pRT->viewportWidth, pRT->viewportHeight);
                m_gl.ClearColor(0.25f, 0.25f, 0.75f, 1.0f);
                m_gl.ClearDepth(1.0f);
                m_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                m_gl.UseProgram(m_testProgramObjectGL);
                m_gl.UniformMatrix4fv(m_gl.GetUniformLocation(m_testProgramObjectGL, "Projection"), 1, GL_FALSE, &pRT->projection[0][0]);
                m_gl.UniformMatrix4fv(m_gl.GetUniformLocation(m_testProgramObjectGL, "View"),       1, GL_FALSE, &pRT->view[0][0]);
                

                for (size_t iMesh = 0; iMesh < m_meshObjects.GetCount(); ++iMesh)
                {
                    auto pMeshObject = reinterpret_cast<MeshObject_OpenGL*>(m_meshObjects[iMesh]);
                    assert(pMeshObject != nullptr);
                    {
                        auto pMeshResource = reinterpret_cast<MeshResource_OpenGL*>(pMeshObject->hMeshResource);
                        assert(pMeshResource != nullptr);
                        {
                            mat4 model = CalculateTransformMatrix(pMeshObject->position, pMeshObject->rotation, pMeshObject->scale);
                            m_gl.UniformMatrix4fv(m_gl.GetUniformLocation(m_testProgramObjectGL, "Model"), 1, GL_FALSE, &model[0][0]);

                            GLsizei stride = sizeof(float)*3 + sizeof(float)*2 + sizeof(float)*3;

                            m_gl.BindBuffer(GL_ARRAY_BUFFER,         pMeshResource->vertexBufferGL);
                            m_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMeshResource->indexBufferGL);

                            m_gl.EnableVertexAttribArray(0);
                            m_gl.EnableVertexAttribArray(1);
                            m_gl.EnableVertexAttribArray(2);
                            m_gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(0));
                            m_gl.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(sizeof(float)*3));
                            m_gl.VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(sizeof(float)*3 + sizeof(float)*2));
                         
                            for (size_t iMaterial = 0; iMaterial < pMeshResource->materialSlots.GetCount(); ++iMaterial)
                            {
                                auto &materialSlot = pMeshResource->materialSlots[iMaterial];

                                //auto pMaterialResource = reinterpret_cast<MaterialResource_OpenGL*>(pMeshObject->materials[iMaterial]);
                                //if (pMaterialResource != nullptr)
                                {
                                    // TODO: Make the material current and set the input variables.

                                    m_gl.DrawElements(GL_TRIANGLES, materialSlot.indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(materialSlot.indexOffset));
                                }
                            }
                        }
                    }
                }
                

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


    void DefaultGraphicsWorld_OpenGL::DeleteTextureResource(HGraphicsResource hTextureResource)
    {
        auto pTexture = reinterpret_cast<TextureResource_OpenGL*>(hTextureResource);
        if (pTexture != nullptr)
        {
            assert(pTexture->type == GraphicsResourceType::Texture);

            m_gl.DeleteTextures(1, &pTexture->objectGL);
            delete pTexture;
        }
    }

    void DefaultGraphicsWorld_OpenGL::DeleteMaterialResource(HGraphicsResource hMaterialResource)
    {
        auto pMaterial = reinterpret_cast<MaterialResource_OpenGL*>(hMaterialResource);
        if (pMaterial != nullptr)
        {
            assert(pMaterial->type == GraphicsResourceType::Material);

            delete pMaterial;
        }
    }

    void DefaultGraphicsWorld_OpenGL::DeleteMeshResource(HGraphicsResource hMeshResource)
    {
        auto pMesh = reinterpret_cast<MeshResource_OpenGL*>(hMeshResource);
        if (pMesh != nullptr)
        {
            assert(pMesh->type == GraphicsResourceType::Mesh);

            delete pMesh;
        }
    }


    void DefaultGraphicsWorld_OpenGL::DeleteMeshObject(HGraphicsObject hMeshObject)
    {
        auto pMesh = reinterpret_cast<MeshObject_OpenGL*>(hMeshObject);
        if (pMesh != nullptr)
        {
            assert(pMesh->type == GraphicsObjectType::Mesh);

            m_meshObjects.RemoveFirstOccuranceOf(hMeshObject);
            delete pMesh;
        }
    }
}

#endif
