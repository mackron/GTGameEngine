// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_OpenGL21_hpp_
#define __GT_GE_GPURenderingDevice_OpenGL21_hpp_

#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTGameEngine/GPURenderingDevice_Gen1.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    /// OpenGL 2.1 implementation of the rendering device.
    class GPURenderingDevice_OpenGL21 : public GPURenderingDevice_Gen1
    {
    public:

        /// Constructor.
        GPURenderingDevice_OpenGL21(const GPURenderingDeviceInfo &info);

        /// Destructor.
        virtual ~GPURenderingDevice_OpenGL21();


        /// GPURenderingDevice::Startup().
        ResultCode Startup();

        /// GPURenderingDevice::Shutdown().
        void Shutdown();


        /// GPURenderingDevice::GetRenderingAPI().
        RenderingAPI GetRenderingAPI() const;

        /// GPURenderingDevice::GetHandedness().
        GPUHandedness GetHandedness() const;



        /// GPURenderingDevice::SetSwapInterval().
        void SetSwapInterval(int swapInterval);


        ////////////////////////////////////////////
        // Drawing

        /// GPURenderingDevice::ClearColor().
        void ClearColor(float r, float g, float b, float a);

        /// @copydoc GPURenderingDevice::ClearDepthStencil()
        void ClearDepthStencil(GPUClearFlag clearFlags, float depth, uint8_t stencil);

        /// GPURenderingDevice::Draw().
        void Draw(unsigned int indexCount, unsigned int startIndexLocation);


        /////////////////////////////////////////////
        // State

        /// @copydoc GPURenderingDevice::SetCurrentShaderProgram()
        void SetCurrentShaderProgram(GPUShaderProgram* shaderProgram);




        /////////////////////////////////////////////////////////////////////////////
        //
        // Stages
        //
        /////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////
        // Input-Assembler Stage

        /// @copydoc GPURenderingDevice::IASetPrimitiveTopology()
        void IASetPrimitiveTopology(GPUPrimitiveTopology topology);

        /// @copydoc GPURenderingDevice::IASetInputLayout()
        void IASetInputLayout(GPUInputLayout* inputLayout);

        /// @copydoc GPURenderingDevice::IASetVertexBuffer()
        void IASetVertexBuffer(unsigned int slotIndex, GPUBuffer* buffer, size_t stride, size_t offset);

        /// @copydoc GPURenderingDevice::IASetIndexBuffer()
        void IASetIndexBuffer(GPUBuffer* buffer, GPUIndexFormat format, size_t offset);


        /////////////////////////////////////////////
        // Rasterization Stage

        /// @copydoc GPURenderingDevice::RSSetState()
        void RSSetState(GPURasterizerState* state);

        /// @copydoc GPURenderingDevice::RSSetViewports()
        void RSSetViewports(GPUViewport* viewports, size_t viewportCount);


        /////////////////////////////////////////////
        // Output Merger Stage

        /// @copydoc GPURenderingDevice::OMSetDepthStencilState()
        void OMSetDepthStencilState(GPUDepthStencilState* state, unsigned int stencilRef);



        /////////////////////////////////////////////////////////////////////////////
        //
        // Object Creation and Deletion
        //
        /////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////
        // State Objects

        /// @copydoc GPURenderingDevice::CreateRasterizerState()
        ResultCode CreateRasterizerState(const GPURasterizerStateDesc &desc, GPURasterizerState* &rasterizerStateOut);

        /// @copydoc GPURenderingDevice::DeleteRasterizerState()
        void DeleteRasterizerState(GPURasterizerState* state);


        /// @copydoc GPURenderingDevice::CreateDepthStencilState()
        ResultCode CreateDepthStencilState(const GPUDepthStencilStateDesc &desc, GPUDepthStencilState* &depthStencilStateOut);

        /// @copydoc GPURenderingDevice::DeleteDepthStencilState().
        void DeleteDepthStencilState(GPUDepthStencilState* state);


        ////////////////////////////////////////////
        // Input Layout

        /// GPURenderingDevice::CreateVertexInputLayout().
        ResultCode CreateInputLayout(GPUShaderProgram* shaderProgram, const GPUInputLayoutAttribDesc* attribDesc, size_t attribDescCount, GPUInputLayout* &vertexInputLayoutOut);

        /// GPURenderingDevice::DeleteVertexInputLayout().
        void DeleteInputLayout(GPUInputLayout* vertexInputLayout);


        ////////////////////////////////////////////
        // Shaders

        /// GPURenderingDevice::CompileShader().
        ResultCode CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);

        /// GPURenderingDevice::IsShaderTargetSupported().
        bool IsShaderTargetSupported(GPUShaderTarget target) const;

        /// GPURenderingDevice::CreateShaderProgram().
        ResultCode CreateShaderProgram(const void* vertexShaderData, size_t vertexShaderDataSize, const void* fragmentShaderData, size_t fragmentShaderDataSize, GT::BasicBuffer &messagesOut, GPUShaderProgram* &shaderProgramOut);

        /// GPURenderingDevice::DeleteShaderProgram().
        void DeleteShaderProgram(GPUShaderProgram* shaderProgram);


        ////////////////////////////////////////////
        // Buffers

        /// GPURenderingDevice::CreateBuffer().
        ResultCode CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data, GPUBuffer* &bufferOut);

        /// GPURenderingDevice::DeleteBuffer().
        void DeleteBuffer(GPUBuffer* buffer);

        /// GPURenderingDevice::MapBuffer().
        ResultCode MapBuffer(GPUBuffer* buffer, GPUBufferMapType mapType, void* &dataOut);

        /// GPURenderingDevice::UnmapBuffer().
        void UnmapBuffer(GPUBuffer* buffer);

        /// GPURenderingDevice::SetBufferData().
        ResultCode SetBufferData(GPUBuffer* buffer, size_t offsetInBytes, size_t sizeInBytes, const void* data);



        ///////////////////////////////////////////
        // Framebuffers

        /// GPURenderingDevice::CreateFramebuffer().
        ResultCode CreateFramebuffer(GPUFramebuffer* &framebuffer);



        //////////////////////////////////////////
        // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
        /// GPURenderingDevice::SwapBuffers().
        void SwapBuffers();

        /// GPURenderingDevice::InitWindowFramebuffer().
        ResultCode InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil);

        /// GPURenderingDevice::UninitWindowFramebuffer().
        void UninitWindowFramebuffer(HWND hWnd);

        /// GPURenderingDevice::SetCurrentWindow().
        ResultCode SetCurrentWindow(HWND hWnd);

        /// @copydoc GPURenderingDevice::ResizeWindowFramebuffer()
        void ResizeWindowFramebuffer(HWND hWnd);
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
        void UpdateSlotVertexAttributePointers(unsigned int slotIndex);


        /// Helper for compiling a GLSL shader.
        ResultCode CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);
        ResultCode CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &messagesOut, GLuint &objectGLOut);

        /// Helper for compiler an ARB shader program.
        ResultCode CompileShader_ARB(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);



    private:

        /// The OpenGL context object.
        OpenGLContext m_gl;


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
        GTLib::Vector<GPUShaderTarget> m_supportedShaderTargets;


        ////////////////////////////////////////////////////
        // State

        /// Boolean state flags.
        uint32_t m_stateFlags;

        /// The current primitive topology for use by the OpenGL API. 
        GLenum m_currentTopologyGL;


        struct IAVertexBufferSlot
        {
            /// A pointer to the vertex buffer object that is bound to the slot.
            GPUBuffer* buffer;

            /// The stride to use with the buffer that is bound to the slot.
            size_t stride;

            /// The offset to use with the buffer that is bound to the slot.
            size_t offset;
        };

        /// The state of each vertex buffer slot.
        IAVertexBufferSlot m_vertexBufferSlots[GT_GE_MAX_VERTEX_BUFFER_SLOTS];

        /// A set of flags that specify which buffer slots are invalid and need to have glVertexAttribPointer() called against them. Each bit represents one slot.
        uint32_t m_invalidVertexBufferSlots;


        /// A pointer to the current index buffer.
        GPUBuffer* m_currentIndexBuffer;

        /// The format of the indices inside the currently bound index buffer.
        GLenum m_indexBufferFormat;

        /// The size in bytes of a single vertex index based on the index buffer format.
        size_t m_indexBufferFormatSize;

        /// The offset into the index buffer to use when drawing.
        size_t m_indexBufferOffset;


        /// The current vertex input layout object.
        GPUInputLayout* m_currentInputLayout;


            
        ///////////////////////////////////////////
        // State Flags

        static const uint32_t StageFlag_IsWindowFramebufferCurrent = (1 << 0);     //< Is the current window's framebuffer the current render target?

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
        GPURenderingDevice_OpenGL21(const GPURenderingDevice_OpenGL21 &);
        GPURenderingDevice_OpenGL21 & operator=(const GPURenderingDevice_OpenGL21 &);
    };
}

#endif

#endif
