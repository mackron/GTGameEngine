// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)
#include "GPUBuffer_D3D11.hpp"
#include "GPUVertexShader_D3D11.hpp"
#include "GPUFragmentShader_D3D11.hpp"
#include "GPUShaderProgram_D3D11.hpp"

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
              m_swapInterval(0),
              m_currentPrimitiveTopology(GPUPrimitiveTopology_Triangle),
              m_D3DCompile(nullptr)
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
                            // Initialize the API.
                            m_D3DCompile = reinterpret_cast<pD3DCompile>(GetProcAddress(m_hD3DCompiler, "D3DCompile"));
                            if (m_D3DCompile == nullptr)
                            {
                                return FailedToLoadD3D11API;
                            }



                            // Triangles by default.
                            m_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                            return 0;   // No error.
                        }
                        else
                        {
                            // Failed to create device.
                            return FailedToCreateD3D11Device;
                        }
                    }
                    else
                    {
                        // Failed to load D3D11CreateDevice().
                        return FailedToLoadD3D11API;
                    }
                }
                else
                {
                    // Failed to load D3dcompiler.dll
                    return FailedToLoadD3DCompilerDLL;
                }
            }
            else
            {
                // Failed to load d3d11.dll
                return FailedToLoadD3D11DLL;
            }
        }

        void GPURenderingDevice_D3D11::Shutdown()
        {
            while (m_windowFramebuffers.GetCount() > 0)
            {
                auto iFramebuffer = m_windowFramebuffers.GetItemByIndex(0);
                if (iFramebuffer != nullptr)
                {
                    assert(m_windowFramebuffers.Exists(iFramebuffer->key));
                    {
                        this->UninitWindowFramebuffer(iFramebuffer->key);   // <-- This will remove the framebuffer from the list.
                    }
                }
            }


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


        ///////////////////////////////////////////
        // Drawing

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

        void GPURenderingDevice_D3D11::Draw(unsigned int indexCount, unsigned int startIndexLocation)
        {
            m_context->DrawIndexed(indexCount, startIndexLocation, 0);
        }



        ///////////////////////////////////////////
        // State

        void GPURenderingDevice_D3D11::SetCurrentShaderProgram(GPUShaderProgram* shaderProgram)
        {
            (void)shaderProgram;
        }



        /////////////////////////////////////////////////////////////////////////////
        //
        // Stages
        //
        /////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////
        // Input-Assembler Stage

        void GPURenderingDevice_D3D11::IASetPrimitiveTopology(GPUPrimitiveTopology topology)
        {
            D3D11_PRIMITIVE_TOPOLOGY topologiesD3D11[] =
            {
                D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
                D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
                D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
                D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
                D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
            };

            m_context->IASetPrimitiveTopology(topologiesD3D11[topology]);
        }

        void GPURenderingDevice_D3D11::IASetInputLayout(GPUInputLayout* vertexInputLayout)
        {
            (void)vertexInputLayout;
        }

        void GPURenderingDevice_D3D11::IASetVertexBuffer(unsigned int slotIndex, GPUBuffer* buffer, size_t stride, size_t offset)
        {
            auto bufferD3D11 = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
            if (bufferD3D11 != nullptr)
            {
                //ID3D11Buffer* d3d11Buffer = bufferD3D11->GetD3D11Buffer();
            }
            else
            {
                m_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
            }
        }

        void GPURenderingDevice_D3D11::IASetIndexBuffer(GPUBuffer* buffer)
        {
            auto bufferD3D11 = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
            if (bufferD3D11 != nullptr)
            {
            }
            else
            {
                m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
            }
        }


        /////////////////////////////////////////////
        // Rasterization Stage

        void GPURenderingDevice_D3D11::RSSetViewports(GPUViewport* viewports, size_t viewportCount)
        {
            if (viewports != nullptr && viewportCount > 0)
            {
                auto viewportsD3D11 = reinterpret_cast<D3D11_VIEWPORT*>(malloc(sizeof(D3D11_VIEWPORT) * viewportCount));
                if (viewportsD3D11 != nullptr)
                {
                    for (size_t iViewport = 0; iViewport < viewportCount; ++iViewport)
                    {
                        viewportsD3D11[iViewport].TopLeftX = viewports[iViewport].x;
                        viewportsD3D11[iViewport].TopLeftY = viewports[iViewport].y;
                        viewportsD3D11[iViewport].Width    = viewports[iViewport].width;
                        viewportsD3D11[iViewport].Height   = viewports[iViewport].height;
                        viewportsD3D11[iViewport].MinDepth = viewports[iViewport].depthRangeNear;
                        viewportsD3D11[iViewport].MaxDepth = viewports[iViewport].depthRangeFar;
                    }

                    m_context->RSSetViewports(static_cast<UINT>(viewportCount), viewportsD3D11);
                }
            }
        }



        /////////////////////////////////////////////////////////////////////////////
        //
        // Object Creation and Deletion
        //
        /////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////
        // Input Layout

        ResultCode GPURenderingDevice_D3D11::CreateInputLayout(GPUShaderProgram* shaderProgram, const GPUInputLayoutAttribDesc* attribDesc, size_t attribDescCount, GPUInputLayout* &vertexInputLayoutOut)
        {
            (void)shaderProgram;
            (void)attribDesc;
            (void)attribDescCount;
            (void)vertexInputLayoutOut;

            return -1;
        }

        void GPURenderingDevice_D3D11::DeleteInputLayout(GPUInputLayout* vertexInputLayout)
        {
            (void)vertexInputLayout;
        }


        ////////////////////////////////////////////
        // Shaders

        ResultCode GPURenderingDevice_D3D11::CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
        {
            if (target >= GPUShaderTarget_HLSL_50_VS && target <= GPUShaderTarget_HLSL_50_CS)
            {
                const char* targetStrs[] =
                {
                    "vs_5_0",       // GPUShaderTarget_HLSL_50_VS
                    "hs_5_0",       // GPUShaderTarget_HLSL_50_HS
                    "ds_5_0",       // GPUShaderTarget_HLSL_50_DS
                    "gs_5_0",       // GPUShaderTarget_HLSL_50_GS
                    "ps_5_0",       // GPUShaderTarget_HLSL_50_PS
                    "cs_5_0",       // GPUShaderTarget_HLSL_50_CS
                };
                const char* targetD3D11 = targetStrs[target - GPUShaderTarget_HLSL_50_VS];


                ID3DBlob* shaderCompiled = NULL;
                ID3DBlob* shaderErrors   = NULL;
                HRESULT hr = m_D3DCompile(source, (sourceLength > 0) ? sourceLength : strlen(source), nullptr, nullptr, nullptr, "main", targetD3D11, D3DCOMPILE_ENABLE_STRICTNESS, 0, &shaderCompiled, &shaderErrors);
                if (SUCCEEDED(hr))
                {
                    if (shaderCompiled != nullptr)
                    {
                        ResultCode result = this->CreateShaderBinaryData(source, sourceLength, defines, target, shaderCompiled->GetBufferPointer(), shaderCompiled->GetBufferSize(), 0, byteCodeOut);

                        shaderCompiled->Release();
                        return result;
                    }
                    else
                    {
                        return FailedToCompileD3D11Shader;
                    }
                }
                else
                {
                    if (shaderErrors != nullptr)
                    {
                        void* messagesPtr = messagesOut.Allocate(shaderErrors->GetBufferSize());
                        if (messagesPtr != nullptr)
                        {
                            memcpy(messagesPtr, shaderErrors->GetBufferPointer(), shaderErrors->GetBufferSize());
                        }

                        shaderErrors->Release();
                    }

                    return FailedToCompileD3D11Shader;
                }
            }
            else
            {
                return ShaderTargetNotSupported;
            }
        }

        bool GPURenderingDevice_D3D11::IsShaderTargetSupported(GPUShaderTarget target) const
        {
            return (target >= GPUShaderTarget_HLSL_50_VS && target <= GPUShaderTarget_HLSL_50_CS);
        }


        ResultCode GPURenderingDevice_D3D11::CreateShaderProgram(const void* vertexShaderData, size_t vertexShaderDataSize, const void* fragmentShaderData, size_t fragmentShaderDataSize, GT::BasicBuffer &messagesOut, GPUShaderProgram* &shaderProgramOut)
        {
            (void)messagesOut;

            ResultCode result = 0;

            GPUVertexShader* vertexShader;
            result = this->CreateVertexShader(vertexShaderData, vertexShaderDataSize, vertexShader);
            if (GT::Succeeded(result))
            {
                GPUFragmentShader* fragmentShader;
                result = this->CreateFragmentShader(fragmentShaderData, fragmentShaderDataSize, fragmentShader);
                if (GT::Succeeded(result))
                {
                    shaderProgramOut = new GPUShaderProgram_D3D11(reinterpret_cast<GPUVertexShader_D3D11*>(vertexShader), reinterpret_cast<GPUFragmentShader_D3D11*>(fragmentShader));
                }
            }

            return result;
        }

        void GPURenderingDevice_D3D11::DeleteShaderProgram(GPUShaderProgram* shaderProgram)
        {
            auto shaderProgramD3D = reinterpret_cast<GPUShaderProgram_D3D11*>(shaderProgram);
            if (shaderProgramD3D != nullptr)
            {
                this->DeleteVertexShader(shaderProgramD3D->GetVertexShader());
                this->DeleteFragmentShader(shaderProgramD3D->GetFragmentShader());

                delete shaderProgramD3D;
            }
        }


        ResultCode GPURenderingDevice_D3D11::CreateVertexShader(const void* shaderData, size_t shaderDataSize, GPUVertexShader* &shaderOut)
        {
            const void* binaryData;
            size_t binaryDataSize;
            ResultCode result = this->ExtractShaderBinaryData(shaderData, shaderDataSize, binaryData, binaryDataSize);
            if (GT::Succeeded(result))
            {
                ID3D11VertexShader* vertexShaderD3D;
                if (SUCCEEDED(m_device->CreateVertexShader(binaryData, binaryDataSize, nullptr, &vertexShaderD3D)))
                {
                    shaderOut = new GPUVertexShader_D3D11(vertexShaderD3D, binaryData, binaryDataSize);
                    return 0;
                }
                else
                {
                    return FailedToCompileD3D11Shader;
                }
            }
            else
            {
                return result;
            }
        }

        void GPURenderingDevice_D3D11::DeleteVertexShader(GPUVertexShader* shader)
        {
            auto shaderD3D = reinterpret_cast<GPUVertexShader_D3D11*>(shader);
            if (shaderD3D != nullptr)
            {
                assert(shaderD3D->GetD3D11VertexShader() != nullptr);
                {
                    shaderD3D->GetD3D11VertexShader()->Release();
                }

                delete shader;
            }
        }


        ResultCode GPURenderingDevice_D3D11::CreateFragmentShader(const void* shaderData, size_t shaderDataSize, GPUFragmentShader* &shaderOut)
        {
            const void* binaryData;
            size_t binaryDataSize;
            ResultCode result = this->ExtractShaderBinaryData(shaderData, shaderDataSize, binaryData, binaryDataSize);
            if (GT::Succeeded(result))
            {
                ID3D11PixelShader* pixelShaderD3D;
                if (SUCCEEDED(m_device->CreatePixelShader(binaryData, binaryDataSize, nullptr, &pixelShaderD3D)))
                {
                    shaderOut = new GPUFragmentShader_D3D11(pixelShaderD3D, binaryData, binaryDataSize);
                    return 0;
                }
                else
                {
                    return FailedToCompileD3D11Shader;
                }
            }
            else
            {
                return result;
            }
        }

        void GPURenderingDevice_D3D11::DeleteFragmentShader(GPUFragmentShader* shader)
        {
            auto shaderD3D = reinterpret_cast<GPUFragmentShader_D3D11*>(shader);
            if (shaderD3D != nullptr)
            {
                assert(shaderD3D->GetD3D11PixelShader() != nullptr);
                {
                    shaderD3D->GetD3D11PixelShader()->Release();
                }

                delete shader;
            }
        }



        ///////////////////////////////////////////
        // Buffers

        ResultCode GPURenderingDevice_D3D11::CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data, GPUBuffer* &bufferOut)
        {
            bufferOut = nullptr;

            if (usage == GPUBufferUsage_Immutable && data == nullptr)
            {
                return NoDataSpecifiedForImmutableBuffer;
            }


            return -1;
        }

        void GPURenderingDevice_D3D11::DeleteBuffer(GPUBuffer* buffer)
        {
            auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
            assert(bufferD3D != nullptr);
            {
                delete buffer;
            }
        }

        ResultCode GPURenderingDevice_D3D11::MapBuffer(GPUBuffer* buffer, GPUBufferMapType mapType, void* &dataOut)
        {
            auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
            assert(bufferD3D != nullptr);
            {
                return -1;
            }
        }

        void GPURenderingDevice_D3D11::UnmapBuffer(GPUBuffer* buffer)
        {
            auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
            assert(bufferD3D != nullptr);
            {
            }
        }

        ResultCode GPURenderingDevice_D3D11::SetBufferData(GPUBuffer* buffer, size_t offsetInBytes, size_t sizeInBytes, const void* data)
        {
            auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
            assert(bufferD3D != nullptr);
            {
                if (bufferD3D->GetBufferUsage() != GPUBufferUsage_Immutable)
                {
                    return -1;
                }
                else
                {
                    return GPUBufferIsImmutable;
                }
            }
        }



        ///////////////////////////////////////////
        // Framebuffers

        ResultCode GPURenderingDevice_D3D11::CreateFramebuffer(GPUFramebuffer* &framebufferOut)
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
                    return FailedToRetrieveIDXGIDevice;
                }

                IDXGIAdapter* pDXGIAdapter;
                hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDXGIAdapter));
                if (FAILED(hr))
                {
                    pDXGIDevice->Release();

                    return FailedToRetrieveIDXGIAdapter;
                }

                IDXGIFactory* pDXGIFactory;
                hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory),reinterpret_cast<void**>(&pDXGIFactory));
                if (FAILED(hr))
                {
                    pDXGIAdapter->Release();
                    pDXGIDevice->Release();

                    return FailedToRetrieveIDXGIFactory;
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

                                        result = FailedToCreateDepthStencilView;
                                    }
                                }
                                else
                                {
                                    framebuffer.renderTargetView->Release();
                                    framebuffer.swapChain->Release();

                                    result = FailedToCreateDepthStencilTexture;
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
                            result = FailedToCreateSwapChainRenderTargetView;
                        }


                        // We don't need to keep this one around.
                        pBackBuffer->Release();
                    }
                    else
                    {
                        framebuffer.swapChain->Release();

                        result = FailedToCreateSwapChainBackBuffer;
                    }
                }
                else
                {
                    result = FailedToCreateSwapChain;
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
                        return InvalidWindowRenderTarget;
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
