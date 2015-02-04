// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)
namespace GT
{
    namespace GE
    {
        GPURenderingDevice_D3D11::GPURenderingDevice_D3D11(const GPURenderingDeviceInfo &info)
            : GPURenderingDevice_Gen2(info),
              m_hD3D11(NULL),
              m_hD3DCompiler(NULL),
              m_device(nullptr),
              m_context(nullptr),
              m_windowFramebuffers(),
              m_currentHWND(0),
              m_currentSwapChain(nullptr),
              m_stateFlags(0),
              m_swapInterval(0)
        {
            m_stateFlags |= StageFlag_IsWindowFramebufferCurrent;       // TODO: Remove this from the constructor once we get the framebuffer system working properly.
        }

        GPURenderingDevice_D3D11::~GPURenderingDevice_D3D11()
        {
        }


        ResultCode GPURenderingDevice_D3D11::Startup()
        {
            m_hD3D11 = LoadLibraryW(L"d3d11.dll");
            if (m_hD3D11 != 0)
            {
                const wchar_t* compilerDLLs[] = {
                    L"d3dcompiler_47.dll",
                    L"d3dcompiler_46.dll",
                    L"d3dcompiler_45.dll",
                    L"d3dcompiler_44.dll",
                    L"d3dcompiler_43.dll",
                    L"d3dcompiler_42.dll",
                    L"d3dcompiler_41.dll",
                    L"d3dcompiler_40.dll",
                    L"d3dcompiler_39.dll",
                    L"d3dcompiler_38.dll",
                    L"d3dcompiler_37.dll",
                    L"d3dcompiler_36.dll",
                    L"d3dcompiler_35.dll",
                    L"d3dcompiler_34.dll",
                    L"d3dcompiler_33.dll",
                };

                int compilerDLLCount = sizeof(compilerDLLs) / sizeof(compilerDLLs[0]);
                for (int i = 0; i < compilerDLLCount; ++i)
                {
                    m_hD3DCompiler = LoadLibraryW(compilerDLLs[i]);
                    if (m_hD3DCompiler != 0)
                    {
                        break;
                    }
                }


                if (m_hD3DCompiler != 0)
                {
                    PFN_D3D11_CREATE_DEVICE _D3D11CreateDevice = reinterpret_cast<PFN_D3D11_CREATE_DEVICE>(GetProcAddress(m_hD3D11, "D3D11CreateDevice"));
                    if (_D3D11CreateDevice != nullptr)
                    {
                        D3D_FEATURE_LEVEL    featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
                        D3D_FEATURE_LEVEL    featureLevel    = D3D_FEATURE_LEVEL_11_0;
                        HRESULT hr = _D3D11CreateDevice(reinterpret_cast<IDXGIAdapter1*>(m_info.identifier_D3D11), D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, featureLevels, sizeof(featureLevels) / sizeof(featureLevels[0]), D3D11_SDK_VERSION, &m_device, &featureLevel, &m_context);
                        if (SUCCEEDED(hr) && featureLevel == D3D_FEATURE_LEVEL_11_0)
                        {
                            return 0;   // No error.
                        }
                        else
                        {
                            // Failed to create device.
                            return -4;
                        }
                    }
                    else
                    {
                        // Failed to load retrieve D3D11CreateDevice.
                        return -3;
                    }
                }
                else
                {
                    // Failed to load D3dcompiler.dll
                    return -2;
                }
            }
            else
            {
                // Failed to load d3d11.dll
                return -1;
            }
        }

        void GPURenderingDevice_D3D11::Shutdown()
        {
            if (m_device  != nullptr) { m_device->Release();  }
            if (m_context != nullptr) { m_context->Release(); }

            m_device  = nullptr;
            m_context = nullptr;


            FreeLibrary(m_hD3D11);
            FreeLibrary(m_hD3DCompiler);

            m_hD3D11       = 0;
            m_hD3DCompiler = 0;
        }



        RenderingAPI GPURenderingDevice_D3D11::GetRenderingAPI() const
        {
            return RenderingAPI_D3D11;
        }



        void GPURenderingDevice_D3D11::SetSwapInterval(int swapInterval)
        {
            m_swapInterval = swapInterval;
        }


        void GPURenderingDevice_D3D11::ClearColor(float r, float g, float b, float a)
        {
            // TODO: Better define the behaviour of this function regarding the current render targets. This is currently quick-and-dirty to get something working.

            auto iFramebuffer = m_windowFramebuffers.Find(m_currentHWND);
            if (iFramebuffer != nullptr)
            {
                float color[4];
                color[0] = r;
                color[1] = g;
                color[2] = b;
                color[3] = a;
                m_context->ClearRenderTargetView(iFramebuffer->value.renderTargetView, color);
            }
        }



        ///////////////////////////////////////////
        // Framebuffers

        ResultCode GPURenderingDevice_D3D11::CreateFramebuffer(Framebuffer* &framebufferOut)
        {
            framebufferOut = nullptr;
            return -1;
        }



        //////////////////////////////////////////
        // Platform-Specific Methods

        void GPURenderingDevice_D3D11::SwapBuffers()
        {
            if (m_currentSwapChain != nullptr)
            {
                m_currentSwapChain->Present(this->m_swapInterval, 0);
            }
        }

#if defined(GT_PLATFORM_WINDOWS)
        ResultCode GPURenderingDevice_D3D11::InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil)
        {
            assert(m_device != nullptr);

            if (!m_windowFramebuffers.Exists(hWnd))
            {
                HRESULT hr = 0;

                // 1) Retrieve the relevant interfaces for creating the swap buffer.
                IDXGIDevice* pDXGIDevice;
                hr = m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDXGIDevice));
                if (FAILED(hr))
                {
                    return D3D11_FailedToRetrieveIDXGIDevice;
                }

                IDXGIAdapter* pDXGIAdapter;
                hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDXGIAdapter));
                if (FAILED(hr))
                {
                    pDXGIDevice->Release();

                    return D3D11_FailedToRetrieveIDXGIAdapter;
                }

                IDXGIFactory* pDXGIFactory;
                hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory),reinterpret_cast<void**>(&pDXGIFactory));
                if (FAILED(hr))
                {
                    pDXGIAdapter->Release();
                    pDXGIDevice->Release();

                    return D3D11_FailedToRetrieveIDXGIFactory;
                }



                // 2) Create the swap buffer and the associated views.
                ResultCode result = 0;

                HWNDFramebuffer framebuffer;
                framebuffer.swapChain        = nullptr;
                framebuffer.renderTargetView = nullptr;
                framebuffer.depthStencil     = nullptr;
                framebuffer.depthStencilView = nullptr;

                RECT windowClientRect;
                GetClientRect(hWnd, &windowClientRect);

                UINT windowWidth  = windowClientRect.right - windowClientRect.left;
                UINT windowHeight = windowClientRect.bottom - windowClientRect.top;

                DXGI_SWAP_CHAIN_DESC sd;
                ZeroMemory(&sd, sizeof(sd));
                sd.BufferCount        = 1;
                sd.BufferDesc.Width   = windowWidth;
                sd.BufferDesc.Height  = windowHeight;
                sd.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
                sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                sd.SampleDesc.Count   = 1;
                sd.SampleDesc.Quality = 0;
                sd.Windowed           = TRUE;
                sd.OutputWindow = hWnd;

                hr = pDXGIFactory->CreateSwapChain(m_device, &sd, &framebuffer.swapChain);
                if (SUCCEEDED(hr))
                {
                    ID3D11Texture2D* pBackBuffer = NULL;
                    hr = framebuffer.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
                    if (SUCCEEDED(hr))
                    {
                        hr = m_device->CreateRenderTargetView(pBackBuffer, NULL, &framebuffer.renderTargetView);
                        if (SUCCEEDED(hr))
                        {
                            // Create the depth/stencil buffer if applicable.
                            if (includeDepthStencil)
                            {
                                // Create the texture.
                                D3D11_TEXTURE2D_DESC descDepth;
                                ZeroMemory(&descDepth, sizeof(descDepth));
                                descDepth.Width              = windowWidth;
                                descDepth.Height             = windowHeight;
                                descDepth.MipLevels          = 1;
                                descDepth.ArraySize          = 1;
                                descDepth.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
                                descDepth.SampleDesc.Count   = 1;
                                descDepth.SampleDesc.Quality = 0;
                                descDepth.Usage              = D3D11_USAGE_DEFAULT;
                                descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
                                descDepth.CPUAccessFlags     = 0;
                                descDepth.MiscFlags          = 0;
                                hr = m_device->CreateTexture2D(&descDepth, NULL, &framebuffer.depthStencil);
                                if (SUCCEEDED(hr))
                                {
                                    // Create the view from the texture.
                                    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
                                    ZeroMemory(&descDSV, sizeof(descDSV));
                                    descDSV.Format             = descDepth.Format;
                                    descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
                                    descDSV.Texture2D.MipSlice = 0;
                                    hr = m_device->CreateDepthStencilView(framebuffer.depthStencil, &descDSV, &framebuffer.depthStencilView);
                                    if (FAILED(hr))
                                    {
                                        framebuffer.depthStencil->Release();
                                        framebuffer.renderTargetView->Release();
                                        framebuffer.swapChain->Release();

                                        result = D3D11_FailedToCreateDepthStencilView;
                                    }
                                }
                                else
                                {
                                    framebuffer.renderTargetView->Release();
                                    framebuffer.swapChain->Release();

                                    result = D3D11_FailedToCreateDepthStencilTexture;
                                }
                            }


                            // If we haven't yet failed at this point we have successfully created the framebuffer for the given window.
                            if (GT::Succeeded(result))
                            {
                                m_windowFramebuffers.Add(hWnd, framebuffer);
                            }
                        }
                        else
                        {
                            result = D3D11_FailedToCreateSwapChainRenderTargetView;
                        }


                        // We don't need to keep this one around.
                        pBackBuffer->Release();
                    }
                    else
                    {
                        framebuffer.swapChain->Release();

                        result = D3D11_FailedToCreateSwapChainBackBuffer;
                    }
                }
                else
                {
                    result = D3D11_FailedToCreateSwapChain;
                }

            

                pDXGIFactory->Release();
                pDXGIAdapter->Release();
                pDXGIDevice->Release();

                return result;
            }
            else
            {
                // Window is already registered. This is not considered an error.
                return 0;
            }
        }

        void GPURenderingDevice_D3D11::UninitWindowFramebuffer(HWND hWnd)
        {
            auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
            if (iFramebuffer != nullptr)
            {
                auto &framebuffer = iFramebuffer->value;

                // If this window is the current one, it needs to be unset.
                if (m_currentHWND == hWnd)
                {
                    // If the current render target is the window's framebuffer
                    this->SetCurrentWindow(NULL);
                }


                if (framebuffer.depthStencilView != nullptr)
                {
                    framebuffer.depthStencilView->Release();
                    framebuffer.depthStencilView = nullptr;
                }

                if (framebuffer.depthStencil != nullptr)
                {
                    framebuffer.depthStencil->Release();
                    framebuffer.depthStencil = nullptr;
                }

                if (framebuffer.renderTargetView != nullptr)
                {
                    framebuffer.renderTargetView->Release();
                    framebuffer.renderTargetView = nullptr;
                }
                
                if (framebuffer.swapChain != nullptr)
                {
                    framebuffer.swapChain->Release();
                    framebuffer.swapChain = nullptr;
                }


                m_windowFramebuffers.RemoveByIndex(iFramebuffer->index);
            }
        }

        ResultCode GPURenderingDevice_D3D11::SetCurrentWindow(HWND hWnd)
        {
            if (m_currentHWND != hWnd)
            {
                if (hWnd != NULL)
                {
                    auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
                    if (iFramebuffer != nullptr)
                    {
                        auto &framebuffer = iFramebuffer->value;

                        // If the current render target is another window, we want that to be unbound, and then have the new window bound.
                        if ((m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
                        {
                            m_context->OMSetRenderTargets(1, &framebuffer.renderTargetView, framebuffer.depthStencilView);
                        }

                        m_currentHWND      = hWnd;
                        m_currentSwapChain = framebuffer.swapChain;
                    }
                    else
                    {
                        // The window can not be used as a render target.
                        return -1;
                    }
                }
                else
                {
                    // No window was specified.
                    if ((m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
                    {
                        m_context->OMSetRenderTargets(0, nullptr, nullptr);
                    }

                    m_currentHWND      = NULL;
                    m_currentSwapChain = nullptr;
                }
            }

            return 0;
        }
#endif

#if defined(GT_PLATFORM_WINDOWS)
#endif
    }
}

#endif
