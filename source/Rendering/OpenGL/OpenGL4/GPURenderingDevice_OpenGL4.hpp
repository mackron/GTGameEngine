// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_RenderingDevice_OpenGL4_hpp_
#define __GT_RenderingDevice_OpenGL4_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL4)
#include <GTGameEngine/GPURenderingDevice.hpp>
#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

#include <GTLib/Map.hpp>

namespace GT
{
    /// OpenGL 4.x implementation of the GPU rendering device.
    class GPURenderingDevice_OpenGL4 : public GPURenderingDevice
    {
        public:

        /// Constructor.
        GPURenderingDevice_OpenGL4(const GPURenderingDeviceInfo &info);

        /// Destructor.
        virtual ~GPURenderingDevice_OpenGL4();


        /// GPURenderingDevice::Startup().
        ResultCode Startup();

        /// GPURenderingDevice::Shutdown().
        void Shutdown();


        /// GPURenderingDevice::GetRenderingAPI().
        RenderingAPI GetRenderingAPI() const;

        /// GPURenderingDevice::GetHandedness().
        GraphicsHandedness GetHandedness() const;



        /// GPURenderingDevice::SetSwapInterval().
        void SetSwapInterval(int swapInterval);


        ////////////////////////////////////////////
        // Drawing

        /// GPURenderingDevice::Draw().
        void Draw(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex);

        /// GPURenderingDevice::DrawInstanced().
        void DrawInstanced(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex, unsigned int instanceCount, unsigned int baseInstance);


        /////////////////////////////////////////////
        // State

        /// @copydoc GPURenderingDevice::BindTextureView()
        void BindTexture(unsigned int slotIndex, HTextureView hTextureView);

        /// @copydoc GPURenderingDevice::BindSampler()
        void BindSampler(unsigned int slotIndex, HSampler hSampler);

        /// @copydoc GPURenderingDevice::BindConstantBuffer(unsigned int, HBuffer)
        void BindConstantBuffer(unsigned int slotIndex, HBuffer hBuffer);

        /// @copydoc GPURenderingDevice::BindConstantBuffer(unsigned int, HBuffer, size_t, size_t)
        void BindConstantBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t offset, size_t size);

        /// @copydoc GPURenderingDevice::BindConstantBuffer(unsigned int, size_t, HBuffer*, size_t*, size_t*)
        void BindConstantBuffer(unsigned int slotIndex, size_t count, HBuffer* hBuffers, size_t* offsets, size_t* sizes);

        /// @copydoc GPURenderingDevice::BindFramebuffer()
        void BindFramebuffer(HFramebuffer hFramebuffer);


        /// @copydoc GPURenderingDevice::SetVertexShader()
        void SetVertexShader(HShader hShader);

        /// @copydoc GPURenderingDevice::SetTessellationControlShader()
        void SetTessellationControlShader(HShader hShader);

        /// @copydoc GPURenderingDevice::SetTessellationEvaluationShader()
        void SetTessellationEvaluationShader(HShader hShader);

        /// @copydoc GPURenderingDevice::SetGeometryShader()
        void SetGeometryShader(HShader hShader);

        /// @copydoc GPURenderingDevice::SetFragmentShader()
        void SetFragmentShader(HShader hShader);

        /// @copydoc GPURenderingDevice::SetComputeShader()
        void SetComputeShader(HShader hShader);



        /////////////////////////////////////////////////////////////////////////////
        //
        // Stages
        //
        /////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////
        // Input-Assembler Stage

        /// @copydoc GPURenderingDevice::IASetPrimitiveTopology()
        void IASetPrimitiveTopology(PrimitiveTopology topology);

        /// @copydoc GPURenderingDevice::IASetInputLayout()
        void IASetInputLayout(HVSInputLayout hInputLayout);

        /// @copydoc GPURenderingDevice::IASetVertexBuffer()
        void IASetVertexBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t stride, size_t offset);

        /// @copydoc GPURenderingDevice::IASetIndexBuffer()
        void IASetIndexBuffer(HBuffer hBuffer, IndexFormat format, size_t offset);


        /////////////////////////////////////////////
        // Rasterization Stage

        /// @copydoc GPURenderingDevice::RSSetState()
        void RSSetState(HRasterizerState hState);

        /// @copydoc GPURenderingDevice::RSSetViewports()
        void RSSetViewports(GraphicsViewport* viewports, size_t viewportCount);

        /// @copydoc GPURenderingDevice::RSSetScissorRects()
        void RSSetScissorRects(ScissorRect* scissorRects, size_t scissorCounts);


        /////////////////////////////////////////////
        // Output Merger Stage

        /// @copydoc GPURenderingDevice::OMSetDepthStencilState()
        void OMSetDepthStencilState(HDepthStencilState hState, unsigned int stencilRef);

        /// @copydoc GPURenderingDevice::OMSetBlendState()
        void OMSetBlendState(HBlendState hState);

        /// @copydoc GPURenderingDevice::OMSetBlendFactor()
        void OMSetBlendFactor(float blendFactor[4]);

        /// @copydoc GPURenderingDevice::OMSetSampleMask()
        void OMSetSampleMask(uint32_t sampleMask);



        /////////////////////////////////////////////////////////////////////////////
        //
        // Object Creation and Deletion
        //
        /////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////
        // State Objects

        /// @copydoc GPURenderingDevice::CreateRasterizerState()
        HRasterizerState CreateRasterizerState(const RasterizerStateDesc &desc);

        /// @copydoc GPURenderingDevice::DeleteRasterizerState()
        void DeleteRasterizerState(HRasterizerState hState);


        /// @copydoc GPURenderingDevice::CreateDepthStencilState()
        HDepthStencilState CreateDepthStencilState(const DepthStencilStateDesc &desc);

        /// @copydoc GPURenderingDevice::DeleteDepthStencilState().
        void DeleteDepthStencilState(HDepthStencilState hState);


        /// @copydoc GPURenderingDevice::CreateBlendState()
        HBlendState CreateBlendState(const BlendStateDesc &desc);

        /// @copydoc GPURenderingDevice::DeleteBlendState()
        void DeleteBlendState(HBlendState hState);



        ////////////////////////////////////////////
        // Input Layout

        /// GPURenderingDevice::CreateVSInputLayout().
        HVSInputLayout CreateVSInputLayout(HShader hVertexShader, const VSInputAttribFormat* attribDesc, size_t attribDescCount);

        /// GPURenderingDevice::DeleteInputLayout().
        void DeleteVSInputLayout(HVSInputLayout hInputLayout);


        ////////////////////////////////////////////
        // Shaders

        /// GPURenderingDevice::CompileShader().
        ResultCode CompileShader(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);

        /// GPURenderingDevice::IsShaderLanguageSupported().
        bool IsShaderLanguageSupported(ShaderLanguage language) const;


        /// @copydoc GPURenderingDevice::CreateShader()
        HShader CreateShader(const void* shaderData, size_t shaderDataSize, ShaderType shaderType);

        /// @copydoc GPURenderingDevice::DeleteShader()
        void DeleteShader(HShader hShader);



        ////////////////////////////////////////////
        // Buffers

        /// @copydoc GPURenderingDevice::CreateBuffer()
        HBuffer CreateBuffer(BufferType type, BufferUsage usage, BufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data);

        /// @copydoc GPURenderingDevice::DeleteBuffer()
        void DeleteBuffer(HBuffer hBuffer);

        /// @copydoc GPURenderingDevice::MapBuffer()
        void* MapBuffer(HBuffer hBuffer, BufferMapType mapType);

        /// @copydoc GPURenderingDevice::UnmapBuffer()
        void UnmapBuffer(HBuffer hBuffer);

        /// @copydoc GPURenderingDevice::SetBufferData()
        void SetBufferData(HBuffer hBuffer, size_t offsetInBytes, size_t sizeInBytes, const void* data);



        ///////////////////////////////////////////
        // Textures

        /// @copydoc GPURenderingDevice::CreateTexture()
        HTexture CreateTexture(const TextureDesc &desc);

        /// @copydoc GPURenderingDevice::DeleteTexture()
        void DeleteTexture(HTexture hTexture);

        /// @copydoc GPURenderingDevice::UpdateTexture()
        void UpdateTexture(HTexture hTexture, int x, int y, int z, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmapLevel, const void* srcData);


        /// @copydoc GPURenderingDevice::CreateTextureView()
        HTextureView CreateTextureView(HTexture hOriginalTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers);
        
        /// @copydoc GPURenderingDevice::DeleteTextureView()
        void DeleteTextureView(HTextureView hTextureView);


        ///////////////////////////////////////////
        // Samplers

        /// @copydoc GPURenderingDevice::CreateSampler()
        HSampler CreateSampler(const SamplerDesc &desc);

        /// @copydoc GPURenderingDevice::DeleteSampler()
        void DeleteSampler(HSampler hSampler);



        ///////////////////////////////////////////
        // Framebuffers

        /// @copydoc GPURenderingDevice::CreateFramebuffer()
        HFramebuffer CreateFramebuffer();

        /// @copydoc GPURenderingDevice::DeleteFramebuffer()
        void DeleteFramebuffer(HFramebuffer hFramebuffer);

        /// @copydoc GPURenderingDevice::AttachFramebufferRenderTarget()
        void AttachFramebufferRenderTarget(HFramebuffer hFramebuffer, unsigned int attachmentIndex, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer);

        /// @copydoc GPURenderingDevice::AttachFramebufferDepthStencilTarget()
        void AttachFramebufferDepthStencilTarget(HFramebuffer hFramebuffer, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer);

        /// @copydoc GPURenderingDevice::ClearFramebufferColor()
        void ClearFramebufferColor(HFramebuffer hFramebuffer, unsigned int attachmentIndex, float color[4]);

        /// @copydoc GPUREnderingDevice::ClearFramebufferDepth()
        void ClearFramebufferDepth(HFramebuffer hFramebuffer, float depth);

        /// @copydoc GPUREnderingDevice::ClearFramebufferStencil()
        void ClearFramebufferStencil(HFramebuffer hFramebuffer, uint8_t stencil);

        /// @copydoc GPUREnderingDevice::ClearFramebufferDepthStencil()
        void ClearFramebufferDepthStencil(HFramebuffer hFramebuffer, float depth, uint8_t stencil);



        //////////////////////////////////////////
        // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
        /// GPURenderingDevice::SwapBuffers().
        void SwapWindowBuffers(HWND hWnd, int swapInterval);

        /// GPURenderingDevice::InitWindowFramebuffer().
        ResultCode InitWindowBuffers(HWND hWnd, bool includeDepthStencil);

        /// GPURenderingDevice::UninitWindowFramebuffer().
        void UninitWindowBuffers(HWND hWnd);

        /// GPURenderingDevice::SetCurrentWindow().
        ResultCode SetCurrentWindow(HWND hWnd);

        /// @copydoc GPURenderingDevice::ResizeWindowFramebuffer()
        void ResizeWindowBuffers(HWND hWnd);
#endif

#if defined(GT_PLATFORM_LINUX)
#endif


    private:

        /// Updates the vertex attribute pointers for the attributes that use the buffer that is bound to the given slot.
        ///
        /// @param slotIndex [in] The slot whose associated vertex attribute pointers are being updated.
        ///
        /// @remarks
        ///     If the buffer is null, the vertex attribute will be disabled.
        void UpdateSlotVertexAttributeState(unsigned int slotIndex);

        /// Helper for compiling a GLSL shader.
        ResultCode CompileShader_GLSL(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);
        ResultCode CompileShader_GLSL(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &messagesOut, GLuint &objectGLOut, GLenum &typeGLOut);


    private:

        /// The OpenGL context object.
        OpenGL4Context m_gl;

#if defined(GT_PLATFORM_WINDOWS)
        /// Structure containing the elements that make up the framebuffer for a given window.
        struct HWNDFramebuffer
        {
            /// A pointer to the swap-chain object for the framebuffer.
            HDC m_hDC;
        };

        /// A list of HWNDs mapped to their framebuffer object.
        GTLib::Map<HWND, HWNDFramebuffer> m_windowFramebuffers;


        /// A handle to the current window where rendering operations will be output to.
        HWND m_currentHWND;

        /// A handle to the DC of the current window.
        HDC m_currentDC;
#endif

        /// The GPU vendor (NVIDIA, AMD, etc.)
        GPUVendor m_vendor;

        /// The list of supported shader targets.
        GTLib::Vector<ShaderLanguage> m_supportedShaderLanguages;

        /// The support flags.
        uint32_t m_supportFlags;


        ////////////////////////////////////////////////////
        // State

        /// Boolean state flags.
        uint32_t m_stateFlags;

        /// The current primitive topology for use by the OpenGL API. 
        GLenum m_currentTopologyGL;

        /// The global VAO object.
        GLuint m_globalVAO;

        /// The global shader pipeline object.
        GLuint m_globalShaderPipeline;


        struct IAVertexBufferSlot
        {
            /// A handle to the vertex buffer object that is bound to the slot.
            HBuffer buffer;

            /// The stride to use with the buffer that is bound to the slot.
            size_t stride;

            /// The offset to use with the buffer that is bound to the slot.
            size_t offset;
        };

        /// The state of each vertex buffer slot.
        IAVertexBufferSlot m_vertexBufferSlots[GT_MAX_VERTEX_BUFFER_SLOTS];

        /// A set of flags that specify which buffer slots are invalid and need to have glVertexAttribPointer() called against them. Each bit represents one slot.
        uint32_t m_invalidVertexBufferSlots;


        /// A handle to the current index buffer.
        HBuffer m_currentIndexBuffer;

        /// The format of the indices inside the currently bound index buffer.
        GLenum m_indexBufferFormatGL;

        /// The size in bytes of a single vertex index based on the index buffer format.
        size_t m_indexBufferFormatSize;

        /// The offset into the index buffer to use when drawing.
        size_t m_indexBufferOffset;


        /// The current vertex input layout object.
        HVSInputLayout m_currentInputLayout;

        /// The current framebuffer.
        HFramebuffer m_currentFramebuffer;



        ///////////////////////////////////////////
        // Support Flags

        static const uint32_t SupportFlag_MultiBind = (1 << 0);     //< Is OpenGL 4.5 or ARB_multi_bind supported? If not, it can be emulated.

    public:

        //////////////////////////////////////////////////////
        // Error Codes

        static const ResultCode FailedToCreateOpenGLShaderObject  = (1 << 31) | 0x000000A0;
        static const ResultCode FailedToCreateOpenGLProgramObject = (1 << 31) | 0x000000A1;
        static const ResultCode FailedToCompileShader             = (1 << 31) | 0x000000A2;
        static const ResultCode FailedToLinkProgram               = (1 << 31) | 0x000000A3;

        // Win32 Errors
#if defined(GT_PLATFORM_WINDOWS)
#endif

        // Linux/X11 Errors
#if defined(GT_PLATFORM_LINUX)
#endif


    private:    // No copying.
        GPURenderingDevice_OpenGL4(const GPURenderingDevice_OpenGL4 &);
        GPURenderingDevice_OpenGL4 & operator=(const GPURenderingDevice_OpenGL4 &);
    };
}


#endif

#endif