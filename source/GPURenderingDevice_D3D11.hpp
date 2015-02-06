// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_D3D11_hpp_
#define __GT_GE_GPURenderingDevice_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/windows.hpp>
#include <d3d11.h>
#include <GTGameEngine/GPURenderingDevice_Gen2.hpp>

namespace GT
{
    namespace GE
    {
        /// Class representing a rendering D3D9-generation GPU device.
        class GPURenderingDevice_D3D11 : public GPURenderingDevice_Gen2
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


            /// GPURenderingDevice::SetSwapInterval().
            void SetSwapInterval(int swapInterval);


            /// GPURenderingDevice::ClearColor().
            void ClearColor(float r, float g, float b, float a);




            ///////////////////////////////////////////
            // Framebuffers

            /// GPURenderingDevice::CreateFramebuffer().
            ResultCode CreateFramebuffer(Framebuffer* &framebuffer);



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

            
            ///////////////////////////////////////////
            // State Flags

            static const uint32_t StageFlag_IsWindowFramebufferCurrent = (1 << 0);     //< Is the current window's framebuffer the current render target?



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


        private:    // No copying.
            GPURenderingDevice_D3D11(const GPURenderingDevice_D3D11 &);
            GPURenderingDevice_D3D11 & operator=(const GPURenderingDevice_D3D11 &);
        };
    }
}

#endif

#endif