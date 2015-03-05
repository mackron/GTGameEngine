// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_D3D11_hpp_
#define __GT_GE_GPURenderingDevice_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/windows.hpp>
#include <GTLib/Threading/Mutex.hpp>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <GTGameEngine/GPURenderingDevice.hpp>

typedef HRESULT (WINAPI *pD3DReflect) (LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector);

namespace GT
{
    /// GPU rendering device class using the D3D11 API.
    class GPURenderingDevice_D3D11 : public GPURenderingDevice
    {
    public:

        /// Constructor.
        GPURenderingDevice_D3D11(const GPURenderingDeviceInfo &info);

        /// Destructor.
        virtual ~GPURenderingDevice_D3D11();



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



        /////////////////////////////////////////////
        // Drawing

        /// GPURenderingDevice::ClearColor().
        void ClearColor(float r, float g, float b, float a);

        /// @copydoc GPURenderingDevice::ClearDepthStencil()
        void ClearDepthStencil(GPUClearFlag clearFlags, float depth, uint8_t stencil);

        /// GPURenderingDevice::Draw().
        void Draw(unsigned int indexCount, unsigned int startIndexLocation);



        /////////////////////////////////////////////
        // State

        /// @copydoc GPURenderingDevice::BindTextureView()
        void BindTexture(HTextureView hTextureView, unsigned int slotIndex);

        /// @copydoc GPURenderingDevice::BindSampler()
        void BindSampler(HSampler hSampler, unsigned int slotIndex);



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
        void IASetInputLayout(HInputLayout hinputLayout);

        /// @copydoc GPURenderingDevice::IASetVertexBuffer()
        void IASetVertexBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t stride, size_t offset);

        /// @copydoc GPURenderingDevice::IASetIndexBuffer()
        void IASetIndexBuffer(HBuffer hBuffer, GPUIndexFormat format, size_t offset);


        /////////////////////////////////////////////
        // Rasterization Stage

        /// @copydoc GPURenderingDevice::RSSetState()
        void RSSetState(HRasterizerState hState);

        /// @copydoc GPURenderingDevice::RSSetViewports()
        void RSSetViewports(GPUViewport* viewports, size_t viewportCount);


        /////////////////////////////////////////////
        // Output Merger Stage

        /// @copydoc GPURenderingDevice::OMSetDepthStencilState()
        void OMSetDepthStencilState(HDepthStencilState hState, unsigned int stencilRef);


        /////////////////////////////////////////////
        // Vertex Shader Stage

        /// @copydoc GPURenderingDevice::FSSetShader()
        void VSSetShader(HVertexShader hShader);


        /////////////////////////////////////////////
        // Fragment Shader Stage

        /// @copydoc GPURenderingDevice::FSSetShader()
        void FSSetShader(HFragmentShader hShader);


        /////////////////////////////////////////////////////////////////////////////
        //
        // Object Creation and Deletion
        //
        /////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////
        // State Objects

        /// @copydoc GPURenderingDevice::CreateRasterizerState()
        HRasterizerState CreateRasterizerState(const GPURasterizerStateDesc &desc);

        /// @copydoc GPURenderingDevice::ReleaseRasterizerState()
        void ReleaseRasterizerState(HRasterizerState hState);

        /// @copydoc GPURenderingDevice::HoldRasterizerState()
        void HoldRasterizerState(HRasterizerState hState);


        /// @copydoc GPURenderingDevice::CreateDepthStencilState()
        HDepthStencilState CreateDepthStencilState(const GPUDepthStencilStateDesc &desc);

        /// @copydoc GPURenderingDevice::ReleaseDepthStencilState().
        void DeleteDepthStencilState(HDepthStencilState hState);

        /// @copydoc GPURenderingDevice::HoldDepthStencilState()
        void HoldDepthStencilState(HDepthStencilState hState);



        ////////////////////////////////////////////
        // Input Layout

        /// GPURenderingDevice::CreateVertexInputLayout().
        HInputLayout CreateInputLayout(HVertexShader hVertexShader, const GPUInputLayoutAttribDesc* attribDesc, size_t attribDescCount);

        /// GPURenderingDevice::ReleaseInputLayout().
        void ReleaseInputLayout(HInputLayout hInputLayout);

        /// @copydoc GPURenderingDevice::HoldInputLayout()
        void HoldInputLayout(HInputLayout hInputLayout);



        ////////////////////////////////////////////
        // Shaders

        /// GPURenderingDevice::CompileShader().
        ResultCode CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);

        /// GPURenderingDevice::IsShaderLanguageSupported().
        bool IsShaderLanguageSupported(ShaderLanguage language) const;


        /// @copydoc GPURenderingDevice_Gen2::CreateVertexShader()
        HVertexShader CreateVertexShader(const void* shaderData, size_t shaderDataSize);

        /// @copydoc GPURenderingDevice_Gen2::ReleaseVertexShader()
        void ReleaseVertexShader(HVertexShader hShader);

        /// @copydoc GPURenderingDevice_Gen2::HoldVertexShader()
        void HoldVertexShader(HVertexShader hShader);


        /// @copydoc GPURenderingDevice_Gen2::CreateFragmentShader()
        HFragmentShader CreateFragmentShader(const void* shaderData, size_t shaderDataSize);

        /// @copydoc GPURenderingDevice_Gen2::ReleaseFragmentShader()
        void ReleaseFragmentShader(HFragmentShader hShader);

        /// @copydoc GPURenderingDevice_Gen2::HoldFragmentShader()
        void HoldFragmentShader(HFragmentShader hShader);




        ////////////////////////////////////////////
        // Buffers

        /// @copydoc GPURenderingDevice::CreateBuffer()
        HBuffer CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data);

        /// @copydoc GPURenderingDevice::ReleaseBuffer()
        void ReleaseBuffer(HBuffer hBuffer);

        /// @copydoc GPURenderingDevice::HoldBuffer()
        void HoldBuffer(HBuffer hBuffer);

        /// @copydoc GPURenderingDevice::MapBuffer()
        void* MapBuffer(HBuffer hBuffer, GPUBufferMapType mapType);

        /// @copydoc GPURenderingDevice::UnmapBuffer()
        void UnmapBuffer(HBuffer hBuffer);

        /// @copydoc GPURenderingDevice::SetBufferData()
        void SetBufferData(HBuffer hBuffer, size_t offsetInBytes, size_t sizeInBytes, const void* data);



        ///////////////////////////////////////////
        // Textures

        /// @copydoc GPURenderingDevice::CreateTexture()
        HTexture CreateTexture(const TextureDesc &desc);

        /// @copydoc GPURenderingDevice::ReleaseTexture()
        void ReleaseTexture(HTexture hTexture);

        /// @copydoc GPURenderingDevice::HoldTexture()
        void HoldTexture(HTexture hTexture);

        /// @copydoc GPURenderingDevice::UpdateTexture()
        void UpdateTexture(HTexture hTexture, int x, int y, int z, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmapLevel, const void* srcData);


        /// @copydoc GPURenderingDevice::CreateTextureView()
        HTextureView CreateTextureView(HTexture hOriginalTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers);
        
        /// @copydoc GPURenderingDevice::ReleaseTextureView()
        void ReleaseTextureView(HTextureView hTextureView);

        /// @copydoc GPURenderingDevice::HoldTextureview()
        void HoldTextureView(HTextureView hTextureView);


        ///////////////////////////////////////////
        // Samplers

        /// @copydoc GPURenderingDevice::CreateSampler()
        HSampler CreateSampler(const SamplerDesc &desc);

        /// @copydoc GPURenderingDevice::ReleaseSampler()
        void ReleaseSampler(HSampler hSampler);

        /// @copydoc GPURenderingDevice::HoldSampler()
        void HoldSampler(HSampler hSampler);


        ///////////////////////////////////////////
        // Framebuffers



        //////////////////////////////////////////
        // Platform-Specific Methods

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



    private:

        /// A handle representing the D3D11 DLL.
        HMODULE m_hD3D11;

        /// A handle representing the D3DCompiler DLL.
        HMODULE m_hD3DCompiler;

        /// The D3D11 device object.
        ID3D11Device* m_device;

        /// The D3D11 context.
        ID3D11DeviceContext* m_context;


        /// Structure containing the elements that make up the framebuffer for a given window.
        struct HWNDFramebuffer
        {
            /// A pointer to the swap-chain object for the framebuffer.
            IDXGISwapChain* swapChain;

            /// The render target view.
            ID3D11RenderTargetView* renderTargetView;

            /// The depth/stencil buffer texture.
            ID3D11Texture2D* depthStencil;

            /// The depth/stencil buffer view.
            ID3D11DepthStencilView* depthStencilView;
        };

        /// A list of HWNDs mapped to their framebuffer object.
        GTLib::Map<HWND, HWNDFramebuffer> m_windowFramebuffers;


        /// A handle to the current window where rendering operations will be output to.
        HWND m_currentHWND;

        /// A pointer to the current swap-chain. This is not strictly required, but it would otherwise require a lookup into m_windowFramebuffers each time SwapBuffers() is called, which is not ideal.
        IDXGISwapChain* m_currentSwapChain;


        ////////////////////////////////////////////////////
        // State

        /// Boolean state flags.
        uint32_t m_stateFlags;

        /// The current swap interval.
        UINT m_swapInterval;

        /// The current primitive topology.
        GPUPrimitiveTopology m_currentPrimitiveTopology;

            
        ///////////////////////////////////////////
        // State Flags

        static const uint32_t StageFlag_IsWindowFramebufferCurrent = (1 << 0);     //< Is the current window's framebuffer the current render target?


        ///////////////////////////////////////////////////////////////
        // API

        pD3DCompile m_D3DCompile;
        pD3DReflect m_D3DReflect;


        public:

        //////////////////////////////////////////////////////
        // Error Codes

        static const ResultCode FailedToCreateD3D11Device               = (1 << 31) | 50;
        static const ResultCode FailedToRetrieveIDXGIDevice             = (1 << 31) | 51;
        static const ResultCode FailedToRetrieveIDXGIAdapter            = (1 << 31) | 52;
        static const ResultCode FailedToRetrieveIDXGIFactory            = (1 << 31) | 53;
        static const ResultCode FailedToCreateSwapChain                 = (1 << 31) | 54;
        static const ResultCode FailedToCreateSwapChainBackBuffer       = (1 << 31) | 55;
        static const ResultCode FailedToCreateSwapChainRenderTargetView = (1 << 31) | 56;
        static const ResultCode FailedToCreateDepthStencilTexture       = (1 << 31) | 57;
        static const ResultCode FailedToCreateDepthStencilView          = (1 << 31) | 58;
        static const ResultCode FailedToLoadD3D11DLL                    = (1 << 31) | 59;
        static const ResultCode FailedToLoadD3DCompilerDLL              = (1 << 31) | 60;
        static const ResultCode FailedToLoadD3D11API                    = (1 << 31) | 61;
        static const ResultCode FailedToCompileD3D11Shader              = (1 << 31) | 62;


        //////////////////////////////////////////////////////
        // GUIDs for the D3D11 API
        static const GUID CustomDataGUID_Generic;
        static const GUID CustomDataGUID_ShaderBinary;


    private:    // No copying.
        GPURenderingDevice_D3D11(const GPURenderingDevice_D3D11 &);
        GPURenderingDevice_D3D11 & operator=(const GPURenderingDevice_D3D11 &);
    };
}

#endif

#endif