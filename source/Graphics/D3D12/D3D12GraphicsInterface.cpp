// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "D3D12GraphicsInterface.hpp"
#include <cassert>
#include <GTLib/Strings/Copy.hpp>

namespace GT
{
    const GUID D3D12GraphicsInterface::CustomDataGUID_Generic = {0xD74A88E1, 0xDA90, 0x05AA, {0xDD, 0x01, 0xEA, 0xD3, 0x31, 0xB3, 0xC2, 0x00}};

    static const DXGI_FORMAT g_D3DTextureFormatsTable[] =
    {
        DXGI_FORMAT_UNKNOWN,

        // Unsigned Normalized.
        DXGI_FORMAT_R8_UNORM,
        DXGI_FORMAT_R8G8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R16_UNORM,
        DXGI_FORMAT_R16G16_UNORM,
        DXGI_FORMAT_R16G16B16A16_UNORM,
        DXGI_FORMAT_R10G10B10A2_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,

        // Signed Normalized.
        DXGI_FORMAT_R8_SNORM,
        DXGI_FORMAT_R8G8_SNORM,
        DXGI_FORMAT_R8G8B8A8_SNORM,
        DXGI_FORMAT_R16_SNORM,
        DXGI_FORMAT_R16G16_SNORM,
        DXGI_FORMAT_R16G16B16A16_SNORM,

        // Unsigned Integral.
        DXGI_FORMAT_R8_UINT,
        DXGI_FORMAT_R8G8_UINT,
        DXGI_FORMAT_R8G8B8A8_UINT,
        DXGI_FORMAT_R16_UINT,
        DXGI_FORMAT_R16G16_UINT,
        DXGI_FORMAT_R16G16B16A16_UINT,
        DXGI_FORMAT_R32_UINT,
        DXGI_FORMAT_R32G32_UINT,
        DXGI_FORMAT_R32G32B32_UINT,
        DXGI_FORMAT_R32G32B32A32_UINT,

        // Signed Integral.
        DXGI_FORMAT_R8_SINT,
        DXGI_FORMAT_R8G8_SINT,
        DXGI_FORMAT_R8G8B8A8_SINT,
        DXGI_FORMAT_R16_SINT,
        DXGI_FORMAT_R16G16_SINT,
        DXGI_FORMAT_R16G16B16A16_SINT,
        DXGI_FORMAT_R32_SINT,
        DXGI_FORMAT_R32G32_SINT,
        DXGI_FORMAT_R32G32B32_SINT,
        DXGI_FORMAT_R32G32B32A32_SINT,

        // Float
        DXGI_FORMAT_R16_FLOAT,
        DXGI_FORMAT_R16G16_FLOAT,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R11G11B10_FLOAT,

        // Special
        DXGI_FORMAT_D24_UNORM_S8_UINT,

        // Compressed
        DXGI_FORMAT_BC1_UNORM,
        DXGI_FORMAT_BC2_UNORM,
        DXGI_FORMAT_BC3_UNORM,
        DXGI_FORMAT_BC1_UNORM_SRGB,
        DXGI_FORMAT_BC2_UNORM_SRGB,
        DXGI_FORMAT_BC3_UNORM_SRGB,

        DXGI_FORMAT_BC4_UNORM,
        DXGI_FORMAT_BC4_SNORM,
        DXGI_FORMAT_BC5_UNORM,
        DXGI_FORMAT_BC5_SNORM,

        DXGI_FORMAT_BC7_UNORM,
        DXGI_FORMAT_BC7_UNORM_SRGB,
        DXGI_FORMAT_BC6H_UF16,
        DXGI_FORMAT_BC6H_SF16
    };

    D3D12_COMMAND_LIST_TYPE g_D3DCommandListTypesTable[] =
    {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_TYPE_BUNDLE,
        D3D12_COMMAND_LIST_TYPE_COMPUTE,
        D3D12_COMMAND_LIST_TYPE_COPY
    };

    D3D12_RESOURCE_USAGE g_D3DResourceUsagesTable[] =
    {
        D3D12_RESOURCE_USAGE_NO_ACCESS,
        D3D12_RESOURCE_USAGE_DEFAULT_READ,
        D3D12_RESOURCE_USAGE_RENDER_TARGET,
        D3D12_RESOURCE_USAGE_UNORDERED_ACCESS,
        D3D12_RESOURCE_USAGE_DEPTH,
        D3D12_RESOURCE_USAGE_NON_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_USAGE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_USAGE_STREAM_OUT,
        D3D12_RESOURCE_USAGE_INDIRECT_ARGUMENT,
        D3D12_RESOURCE_USAGE_COPY_DEST,
        D3D12_RESOURCE_USAGE_COPY_SOURCE,
        D3D12_RESOURCE_USAGE_RESOLVE_DEST,
        D3D12_RESOURCE_USAGE_RESOLVE_SOURCE,
        D3D12_RESOURCE_USAGE_GENERIC_READ,
        D3D12_RESOURCE_USAGE_PRESENT,
        D3D12_RESOURCE_USAGE_INITIAL
    };


    D3D12GraphicsInterface::D3D12GraphicsInterface(HMODULE hDXGI, HMODULE hD3D12, HMODULE hD3DCompiler)
        : GraphicsInterface_MT(GraphicsInterfaceType_D3D12),
          m_hDXGI(hDXGI), m_hD3D12(hD3D12), m_hD3DCompiler(hD3DCompiler),
          m_pFactory(nullptr),
          m_windowBufferData(),
          m_CreateDXGIFactory2(nullptr),
          m_D3D12CreateDevice(nullptr)
    {
        assert(m_hDXGI        != NULL);
        assert(m_hD3D12       != NULL);
        assert(m_hD3DCompiler != NULL);

        
    }

    D3D12GraphicsInterface::~D3D12GraphicsInterface()
    {
    }


    ResultCode D3D12GraphicsInterface::Startup()
    {
        m_CreateDXGIFactory2 = reinterpret_cast<PFN_CreateDXGIFactory2>(GetProcAddress(m_hDXGI,  "CreateDXGIFactory2"));
        if (m_CreateDXGIFactory2 != nullptr)
        {
            UINT factoryFlags = 0;
#if _DEBUG
            factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

            if (SUCCEEDED(m_CreateDXGIFactory2(factoryFlags, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(&m_pFactory))))
            {
                m_D3D12CreateDevice = reinterpret_cast<PFN_D3D12_CREATE_DEVICE>(GetProcAddress(m_hD3D12, "D3D12CreateDevice"));
                if (m_D3D12CreateDevice != nullptr)
                {
                    return GT::NoError;
                }
                else
                {
                    // Failed to initialize the D3D12 API.
                    this->Shutdown();
                    return GT::UnknownError;
                }
            }
            else
            {
                // Failed to create factory.
                this->Shutdown();
                return GT::UnknownError;
            }
        }
        else
        {
            // Failed to initialize the DXGI API.
            return GT::UnknownError;
        }
    }

    void D3D12GraphicsInterface::Shutdown()
    {
        if (m_pFactory != nullptr)
        {
            m_pFactory->Release();
            m_pFactory = nullptr;
        }

        m_CreateDXGIFactory2 = nullptr;
        m_D3D12CreateDevice  = nullptr;
    }



    bool D3D12GraphicsInterface::EnumDevices(unsigned int iDevice, GraphicsDeviceInfo &infoOut) const
    {
        // We only return true for D3D12 compatible devices.

        unsigned int iDeviceD3D = 0;
        IDXGIAdapter1* pAdapterD3D;
        while (m_pFactory->EnumAdapters1(iDeviceD3D, &pAdapterD3D))
        {
            ID3D12Device* pDeviceD3D = nullptr;
            if (SUCCEEDED(m_D3D12CreateDevice(pAdapterD3D, D3D_DRIVER_TYPE_UNKNOWN, D3D12_CREATE_DEVICE_NONE, D3D_FEATURE_LEVEL_11_1, D3D12_SDK_VERSION, __uuidof(ID3D12Device), reinterpret_cast<void**>(&pDeviceD3D))))
            {
                // D3D12 is supported. If iDevice is 0, we set the output structure and return true. Otherwise, we decrement iDevice and continue the loop.
                if (iDevice == 0)
                {
                    DXGI_ADAPTER_DESC dxgiDesc;
                    pAdapterD3D->GetDesc(&dxgiDesc);

                    ZeroMemory(&infoOut, sizeof(infoOut));
                    GTLib::Strings::Copy(infoOut.description, dxgiDesc.Description);

                    pDeviceD3D->Release();
                    pAdapterD3D->Release();

                    return true;
                }
                else
                {
                    iDevice -= 1;
                }

                pDeviceD3D->Release();
            }

            pAdapterD3D->Release();
        }

        return false;
    }

    HGraphicsDevice D3D12GraphicsInterface::CreateDevice(unsigned int iDevice)
    {
        D3D12_CREATE_DEVICE_FLAG flags = D3D12_CREATE_DEVICE_NONE;
#if _DEBUG
        //flags |= D3D12_CREATE_DEVICE_DEBUG;
#endif

        unsigned int iDeviceD3D = 0;
        IDXGIAdapter1* pAdapterD3D;
        while (SUCCEEDED(m_pFactory->EnumAdapters1(iDeviceD3D, &pAdapterD3D)))
        {
            ID3D12Device* pDeviceD3D = nullptr;
            if (SUCCEEDED(m_D3D12CreateDevice(pAdapterD3D, D3D_DRIVER_TYPE_UNKNOWN, flags, D3D_FEATURE_LEVEL_11_1, D3D12_SDK_VERSION, __uuidof(ID3D12Device), reinterpret_cast<void**>(&pDeviceD3D))))
            {
                // D3D12 is supported. If iDevice is 0, we set the output structure and return the device. Otherwise, we decrement iDevice and continue the loop.
                if (iDevice == 0)
                {
                    pAdapterD3D->Release();
                    return reinterpret_cast<HGraphicsDevice>(pDeviceD3D);
                }
                else
                {
                    iDevice -= 1;
                }

                pDeviceD3D->Release();
            }

            pAdapterD3D->Release();
            iDeviceD3D += 1;
        }

        return 0;
    }

    void D3D12GraphicsInterface::DeleteDevice(HGraphicsDevice hDevice)
    {
        auto pD3D12Device = reinterpret_cast<ID3D12Device*>(hDevice);
        if (pD3D12Device != nullptr)
        {
            pD3D12Device->Release();
        }
    }



    void D3D12GraphicsInterface::SwapWindowBuffers(HWND hWnd, int swapInterval)
    {
        auto iWindowBufferData = m_windowBufferData.Find(hWnd);
        if (iWindowBufferData != nullptr)
        {
            iWindowBufferData->value.pSwapChain->Present(static_cast<UINT>(swapInterval), 0);
        }
    }

    ResultCode D3D12GraphicsInterface::InitWindowBuffers(HWND hWnd, bool includeDepthStencil)
    {
        return 0;
    }
    ResultCode D3D12GraphicsInterface::InitWindowBuffers(HGraphicsDevice hDevice, HCommandQueue hCommandQueue, HDescriptorHeap hDescriptorHeap, unsigned int descriptorIndex, HWND hWnd, bool includeDepthStencil)
    {
        auto pDeviceD3D = reinterpret_cast<ID3D12Device*>(hDevice);
        if (pDeviceD3D != nullptr)
        {
            auto pCommandQueue = reinterpret_cast<ID3D12CommandQueue*>(hCommandQueue);
            if (pCommandQueue != nullptr)
            {
                auto pDescriptorHeap = reinterpret_cast<ID3D12DescriptorHeap*>(hDescriptorHeap);
                if (pDescriptorHeap != nullptr)
                {
                    auto iWindowBufferData = m_windowBufferData.Find(hWnd);
                    if (iWindowBufferData == nullptr)
                    {
                        D3D12_DESCRIPTOR_HEAP_DESC descriptHeapDesc = pDescriptorHeap->GetDesc();

                        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
                        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
                        swapChainDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
                        swapChainDesc.SampleDesc.Count   = 1;
                        swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                        swapChainDesc.BufferCount        = 2;
                        swapChainDesc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
                        swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
                        

                        IDXGISwapChain3* pSwapChain = nullptr;
                        if (SUCCEEDED(m_pFactory->CreateSwapChainForHwnd(pCommandQueue, hWnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&pSwapChain))))  // <-- First argument is not an error - it needs to be the command queue and NOT the device.
                        {
                            ID3D12Resource* pSwapChainRT0 = nullptr;
                            ID3D12Resource* pSwapChainRT1 = nullptr;
                            if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&pSwapChainRT0))) &&
                                SUCCEEDED(pSwapChain->GetBuffer(1, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&pSwapChainRT1))))
                            {
                                WindowBufferData windowData;
                                windowData.pDeviceD3D            = pDeviceD3D;
                                windowData.pSwapChain            = pSwapChain;
                                windowData.pRenderTargets[0]     = pSwapChainRT0;
                                windowData.pRenderTargets[1]     = pSwapChainRT1;
                                windowData.hDescriptorHandles[0] = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart().MakeOffsetted(descriptorIndex + 0, pDeviceD3D->GetDescriptorHandleIncrementSize(descriptHeapDesc.Type));
                                windowData.hDescriptorHandles[1] = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart().MakeOffsetted(descriptorIndex + 1, pDeviceD3D->GetDescriptorHandleIncrementSize(descriptHeapDesc.Type));
                                m_windowBufferData.Add(hWnd, windowData);

                                pDeviceD3D->CreateRenderTargetView(pSwapChainRT0, nullptr, windowData.hDescriptorHandles[0]);
                                pDeviceD3D->CreateRenderTargetView(pSwapChainRT1, nullptr, windowData.hDescriptorHandles[1]);

                                return 0;
                            }
                        }
                    }
                    else
                    {
                        // Window buffers have already been initialized, so just return success.
                        return 0;
                    }
                }
            }
        }

        return GT::UnknownError;
    }

    void D3D12GraphicsInterface::UninitWindowBuffers(HWND hWnd)
    {
    }

    void D3D12GraphicsInterface::ResizeWindowBuffers(HWND hWnd)
    {
    }


    //////////////////////////////////////////
    // State

    HPipelineState D3D12GraphicsInterface::CreateGraphicsPipelineState(HGraphicsDevice hDevice, const GraphicsPipelineStateDesc &desc)
    {
        // TODO: Some of this is a direct copy/paste from D3D11 - this needs to be validated.

        auto pDeviceD3D = reinterpret_cast<ID3D12Device*>(hDevice);
        assert(pDeviceD3D != nullptr);
        {
            const char* semanticName = "TEXCOORD";


            D3D12_GRAPHICS_PIPELINE_STATE_DESC descD3D;
            ZeroMemory(&descD3D, sizeof(descD3D));

            descD3D.pRootSignature = nullptr;
            descD3D.VS.pShaderBytecode = desc.shaderBytecodeVS.pData;
            descD3D.VS.BytecodeLength  = desc.shaderBytecodeVS.dataSizeInBytes;
            descD3D.HS.pShaderBytecode = desc.shaderBytecodeTCS.pData;
            descD3D.HS.BytecodeLength  = desc.shaderBytecodeTCS.dataSizeInBytes;
            descD3D.DS.pShaderBytecode = desc.shaderBytecodeTES.pData;
            descD3D.DS.BytecodeLength  = desc.shaderBytecodeTES.dataSizeInBytes;
            descD3D.GS.pShaderBytecode = desc.shaderBytecodeGS.pData;
            descD3D.GS.BytecodeLength  = desc.shaderBytecodeGS.dataSizeInBytes;
            descD3D.PS.pShaderBytecode = desc.shaderBytecodeFS.pData;
            descD3D.PS.BytecodeLength  = desc.shaderBytecodeFS.dataSizeInBytes;
        
            /////////////////////////////////
            // Stream Output

            D3D12_SO_DECLARATION_ENTRY* pSODeclarationTemp = nullptr;
            if (desc.streamOutput.pSODeclaration != nullptr && desc.streamOutput.numEntries > 0)
            {
                pSODeclarationTemp = new D3D12_SO_DECLARATION_ENTRY[desc.streamOutput.numEntries];
                for (unsigned int iEntry = 0; iEntry < desc.streamOutput.numEntries; ++iEntry)
                {
                    pSODeclarationTemp[iEntry].Stream         = desc.streamOutput.pSODeclaration[iEntry].stream;
                    pSODeclarationTemp[iEntry].SemanticName   = semanticName;
                    pSODeclarationTemp[iEntry].SemanticIndex  = desc.streamOutput.pSODeclaration[iEntry].attributeIndex;
                    pSODeclarationTemp[iEntry].StartComponent = desc.streamOutput.pSODeclaration[iEntry].startComponent;
                    pSODeclarationTemp[iEntry].ComponentCount = desc.streamOutput.pSODeclaration[iEntry].componentCount;
                    pSODeclarationTemp[iEntry].OutputSlot     = desc.streamOutput.pSODeclaration[iEntry].outputSlot;
                }
            }

            descD3D.StreamOutput.pSODeclaration   = pSODeclarationTemp;
            descD3D.StreamOutput.NumEntries       = desc.streamOutput.numEntries;
            descD3D.StreamOutput.pBufferStrides   = reinterpret_cast<const UINT*>(desc.streamOutput.pBufferStrides);
            descD3D.StreamOutput.NumStrides       = desc.streamOutput.numStrides;
            descD3D.StreamOutput.RasterizedStream = desc.streamOutput.rasterizedStream;


            /////////////////////////////////
            // Blending

            D3D12_BLEND blendParametersD3D[] =
            {
                D3D12_BLEND_ZERO,                   // BlendParameter_Zero
                D3D12_BLEND_ONE,                    // BlendParameter_One

                D3D12_BLEND_SRC_COLOR,              // BlendParameter_Src_Color
                D3D12_BLEND_INV_SRC_COLOR,          // BlendParameter_Inv_Src_Color
                D3D12_BLEND_SRC_ALPHA,              // BlendParameter_Src_Alpha
                D3D12_BLEND_INV_SRC_ALPHA,          // BlendParameter_Inv_Src_Alpha
                D3D12_BLEND_SRC_ALPHA_SAT,          // BlendParameter_Src_Alpha_Saturate
        
                D3D12_BLEND_DEST_COLOR,             // BlendParameter_Dst_Color
                D3D12_BLEND_INV_DEST_COLOR,         // BlendParameter_Inv_Dst_Color
                D3D12_BLEND_DEST_ALPHA,             // BlendParameter_Dst_Alpha
                D3D12_BLEND_INV_DEST_ALPHA,         // BlendParameter_Inv_Dst_Alpha
        
                D3D12_BLEND_SRC1_COLOR,             // BlendParameter_Src1_Color
                D3D12_BLEND_INV_SRC1_COLOR,         // BlendParameter_Inv_Src1_Color
                D3D12_BLEND_SRC1_ALPHA,             // BlendParameter_Src1_Alpha
                D3D12_BLEND_INV_SRC1_ALPHA,         // BlendParameter_Inv_Src1_Alpha

                D3D12_BLEND_BLEND_FACTOR,           // BlendParameter_BlendFactor
                D3D12_BLEND_INV_BLEND_FACTOR        // BlendParameter_Inv_BlendFactor
            };

            D3D12_BLEND_OP blendOpsD3D[] =
            {
                D3D12_BLEND_OP_ADD,                 // BlendOp_Add
                D3D12_BLEND_OP_SUBTRACT,            // BlendOp_Subtract
                D3D12_BLEND_OP_REV_SUBTRACT,        // BlendOp_Reverse_Subtract
                D3D12_BLEND_OP_MIN,                 // BlendOp_Min
                D3D12_BLEND_OP_MAX                  // BlendOp_Max
            };


            descD3D.BlendState.AlphaToCoverageEnable  = desc.blendState.enableAlphaToCoverage;
            descD3D.BlendState.IndependentBlendEnable = desc.blendState.enableIndependentBlend;

            for (int iRT = 0; iRT < 8; ++iRT)
            {
                descD3D.BlendState.RenderTarget[iRT].BlendEnable           = desc.blendState.renderTarget[iRT].enableBlending;
                descD3D.BlendState.RenderTarget[iRT].SrcBlend              = blendParametersD3D[desc.blendState.renderTarget[iRT].srcBlendParameter];
                descD3D.BlendState.RenderTarget[iRT].DestBlend             = blendParametersD3D[desc.blendState.renderTarget[iRT].dstBlendParameter];
                descD3D.BlendState.RenderTarget[iRT].BlendOp               = blendOpsD3D[desc.blendState.renderTarget[iRT].blendOp];
                descD3D.BlendState.RenderTarget[iRT].SrcBlendAlpha         = blendParametersD3D[desc.blendState.renderTarget[iRT].srcAlphaBlendParameter];
                descD3D.BlendState.RenderTarget[iRT].DestBlendAlpha        = blendParametersD3D[desc.blendState.renderTarget[iRT].dstAlphaBlendParameter];
                descD3D.BlendState.RenderTarget[iRT].BlendOpAlpha          = blendOpsD3D[desc.blendState.renderTarget[iRT].blendOpAlpha];
                descD3D.BlendState.RenderTarget[iRT].RenderTargetWriteMask = desc.blendState.renderTarget[iRT].writeMask;
            }

            descD3D.SampleMask = desc.sampleMask;


            /////////////////////////////////
            // Rasterizer

            D3D12_FILL_MODE fillModesD3D[] =
            {
                D3D12_FILL_WIREFRAME,
                D3D12_FILL_SOLID
            };

            D3D12_CULL_MODE cullModesD3D[] =
            {
                D3D12_CULL_NONE,
                D3D12_CULL_FRONT,
                D3D12_CULL_BACK
            };

            descD3D.RasterizerState.FillMode              = fillModesD3D[desc.rasterizerState.fillMode];
            descD3D.RasterizerState.CullMode              = cullModesD3D[desc.rasterizerState.cullMode];
            descD3D.RasterizerState.FrontCounterClockwise = (desc.rasterizerState.polygonWinding == PolygonWinding_CW) ? TRUE : FALSE;
            descD3D.RasterizerState.DepthBias             = desc.rasterizerState.depthBias;
            descD3D.RasterizerState.DepthBiasClamp        = desc.rasterizerState.depthBiasClamp;
            descD3D.RasterizerState.SlopeScaledDepthBias  = desc.rasterizerState.slopeScaledDepthBias;
            descD3D.RasterizerState.DepthClipEnable       = desc.rasterizerState.enableDepthClip;
            descD3D.RasterizerState.MultisampleEnable     = desc.rasterizerState.enableMultisample;
            descD3D.RasterizerState.AntialiasedLineEnable = desc.rasterizerState.enableAntialiasedLine;



            /////////////////////////////////
            // Depth/Stencil

            D3D12_DEPTH_WRITE_MASK depthWriteMasks[] =
            {
                D3D12_DEPTH_WRITE_MASK_ZERO,
                D3D12_DEPTH_WRITE_MASK_ALL
            };

            D3D12_COMPARISON_FUNC comparisonFuncs[] =
            {
                D3D12_COMPARISON_NEVER,
                D3D12_COMPARISON_LESS,
                D3D12_COMPARISON_EQUAL,
                D3D12_COMPARISON_LESS_EQUAL,
                D3D12_COMPARISON_GREATER,
                D3D12_COMPARISON_NOT_EQUAL,
                D3D12_COMPARISON_GREATER_EQUAL,
                D3D12_COMPARISON_ALWAYS
            };

            D3D12_STENCIL_OP stencilOps[] =
            {
                D3D12_STENCIL_OP_KEEP,
                D3D12_STENCIL_OP_ZERO,
                D3D12_STENCIL_OP_REPLACE,
                D3D12_STENCIL_OP_INCR_SAT,
                D3D12_STENCIL_OP_DECR_SAT,
                D3D12_STENCIL_OP_INVERT,
                D3D12_STENCIL_OP_INCR,
                D3D12_STENCIL_OP_DECR
            };


            descD3D.DepthStencilState.DepthEnable                  = desc.depthStencilState.enableDepthTest;
            descD3D.DepthStencilState.DepthWriteMask               = depthWriteMasks[desc.depthStencilState.depthWriteMask];
            descD3D.DepthStencilState.DepthFunc                    = comparisonFuncs[desc.depthStencilState.depthFunc];
            descD3D.DepthStencilState.StencilEnable                = desc.depthStencilState.enableStencilTest;
            descD3D.DepthStencilState.StencilReadMask              = desc.depthStencilState.stencilReadMask;
            descD3D.DepthStencilState.StencilWriteMask             = desc.depthStencilState.stencilWriteMask;
            descD3D.DepthStencilState.FrontFace.StencilFailOp      = stencilOps[desc.depthStencilState.stencilFrontFaceOp.stencilFailOp];
            descD3D.DepthStencilState.FrontFace.StencilDepthFailOp = stencilOps[desc.depthStencilState.stencilFrontFaceOp.stencilDepthFailOp];
            descD3D.DepthStencilState.FrontFace.StencilPassOp      = stencilOps[desc.depthStencilState.stencilFrontFaceOp.stencilPassOp];
            descD3D.DepthStencilState.FrontFace.StencilFunc        = comparisonFuncs[desc.depthStencilState.stencilFrontFaceOp.stencilFunc];
            descD3D.DepthStencilState.BackFace.StencilFailOp       = stencilOps[desc.depthStencilState.stencilBackFaceOp.stencilFailOp];
            descD3D.DepthStencilState.BackFace.StencilDepthFailOp  = stencilOps[desc.depthStencilState.stencilBackFaceOp.stencilDepthFailOp];
            descD3D.DepthStencilState.BackFace.StencilPassOp       = stencilOps[desc.depthStencilState.stencilBackFaceOp.stencilPassOp];
            descD3D.DepthStencilState.BackFace.StencilFunc         = comparisonFuncs[desc.depthStencilState.stencilBackFaceOp.stencilFunc];



            /////////////////////////////////
            // Vertex Shader Inputs

            if (desc.inputLayout.attribFormats != nullptr && desc.inputLayout.attribFormatsCount != 0)
            {
                DXGI_FORMAT format[7][4] = 
                {
                    {
                        DXGI_FORMAT_R32_FLOAT,
                        DXGI_FORMAT_R32G32_FLOAT,
                        DXGI_FORMAT_R32G32B32_FLOAT,
                        DXGI_FORMAT_R32G32B32A32_FLOAT
                    },  // VertexAttribFormat_Float
                    {
                        DXGI_FORMAT_R32_SINT,
                        DXGI_FORMAT_R32G32_SINT,
                        DXGI_FORMAT_R32G32B32_SINT,
                        DXGI_FORMAT_R32G32B32A32_SINT
                    },  // VertexAttribFormat_Int32
                    {
                        DXGI_FORMAT_R32_UINT,
                        DXGI_FORMAT_R32G32_UINT,
                        DXGI_FORMAT_R32G32B32_UINT,
                        DXGI_FORMAT_R32G32B32A32_UINT
                    },  // VertexAttribFormat_UInt32
                    {
                        DXGI_FORMAT_R16_SINT,
                        DXGI_FORMAT_R16G16_SINT,
                        DXGI_FORMAT_R16G16B16A16_SINT,
                        DXGI_FORMAT_R16G16B16A16_SINT
                    },  // VertexAttribFormat_Int16
                    {
                        DXGI_FORMAT_R16_UINT,
                        DXGI_FORMAT_R16G16_UINT,
                        DXGI_FORMAT_R16G16B16A16_UINT,
                        DXGI_FORMAT_R16G16B16A16_UINT
                    },  // VertexAttribFormat_UInt16
                    {
                        DXGI_FORMAT_R8_SINT,
                        DXGI_FORMAT_R8G8_SINT,
                        DXGI_FORMAT_R8G8B8A8_SINT,
                        DXGI_FORMAT_R8G8B8A8_SINT
                    },  // VertexAttribFormat_Int8
                    {
                        DXGI_FORMAT_R16_UINT,
                        DXGI_FORMAT_R16G16_UINT,
                        DXGI_FORMAT_R8G8B8A8_UINT,
                        DXGI_FORMAT_R8G8B8A8_UINT
                    }  // VertexAttribFormat_UInt8
                };

                D3D12_INPUT_ELEMENT_DESC* pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[desc.inputLayout.attribFormatsCount];
                for (unsigned int iAttrib = 0; iAttrib < desc.inputLayout.attribFormatsCount; ++iAttrib)
                {
                    pInputElementDescs[iAttrib].SemanticName         = semanticName;
                    pInputElementDescs[iAttrib].SemanticIndex        = desc.inputLayout.attribFormats[iAttrib].attributeIndex;
                    pInputElementDescs[iAttrib].Format               = format[desc.inputLayout.attribFormats[iAttrib].attributeComponentType][desc.inputLayout.attribFormats[iAttrib].attributeComponentCount - 1];
                    pInputElementDescs[iAttrib].InputSlot            = desc.inputLayout.attribFormats[iAttrib].slotIndex;
                    pInputElementDescs[iAttrib].AlignedByteOffset    = desc.inputLayout.attribFormats[iAttrib].attributeOffset;
                    pInputElementDescs[iAttrib].InputSlotClass       = (desc.inputLayout.attribFormats[iAttrib].instanceStepRate == 0) ? D3D12_INPUT_PER_VERTEX_DATA : D3D12_INPUT_PER_INSTANCE_DATA;
                    pInputElementDescs[iAttrib].InstanceDataStepRate = desc.inputLayout.attribFormats[iAttrib].instanceStepRate;
                }

                descD3D.InputLayout.pInputElementDescs = pInputElementDescs;
                descD3D.InputLayout.NumElements        = desc.inputLayout.attribFormatsCount;
            }

            descD3D.IndexBufferProperties = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;


            D3D12_PRIMITIVE_TOPOLOGY_TYPE topologiesD3D[] =
            {
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH
            };
            descD3D.PrimitiveTopologyType = topologiesD3D[desc.primitiveTopologyType];



            /////////////////////////////////
            // Render Targets

            descD3D.NumRenderTargets = desc.renderTargetCount;
            for (unsigned int iRT = 0; iRT < desc.renderTargetCount && iRT < 8; ++iRT)
            {
                descD3D.RTVFormats[iRT] = g_D3DTextureFormatsTable[desc.renderTargetFormats[iRT]];
            }

            descD3D.DSVFormat = g_D3DTextureFormatsTable[desc.depthStencilFormat];



            /////////////////////////////////
            // Samples

            descD3D.SampleDesc.Count   = desc.sampleDesc.count;
            descD3D.SampleDesc.Quality = desc.sampleDesc.quality;



            /////////////////////////////////
            // Other Stuff

            descD3D.NodeMask = desc.nodeMask;
            descD3D.CachedPSO.pCachedBlob           = nullptr;
            descD3D.CachedPSO.CachedBlobSizeInBytes = 0;


        
            // At this point we have converted the pipeline descriptor to the D3D12 format and can now try creating the actual object.
            ID3D12PipelineState* pPipelineState = nullptr;
            HRESULT hr = pDeviceD3D->CreateGraphicsPipelineState(&descD3D, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&pPipelineState));


            delete [] pSODeclarationTemp;
            return reinterpret_cast<HPipelineState>(pPipelineState);
        }
    }

    void D3D12GraphicsInterface::DeleteGraphicsPipelineState(HPipelineState hPipelineState)
    {
        auto pPipelineStateD3D = reinterpret_cast<ID3D12PipelineState*>(hPipelineState);
        if (pPipelineStateD3D != nullptr)
        {
            pPipelineStateD3D->Release();
        }
    }


    //////////////////////////////////////////
    // Allocators

    HCommandAllocator D3D12GraphicsInterface::CreateCommandAllocator(HGraphicsDevice hDevice, CommandListType type)
    {
        auto pDevice = reinterpret_cast<ID3D12Device*>(hDevice);
        assert(pDevice != nullptr);
        {
            ID3D12CommandAllocator* pCommandAllocator = nullptr;
            pDevice->CreateCommandAllocator(g_D3DCommandListTypesTable[type], __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&pCommandAllocator));

            return reinterpret_cast<HCommandAllocator>(pCommandAllocator);
        }
    }

    void D3D12GraphicsInterface::DeleteCommandAllocator(HCommandAllocator hCommandAllocator)
    {
        auto pCommandAllocator = reinterpret_cast<ID3D12CommandAllocator*>(hCommandAllocator);
        if (pCommandAllocator != nullptr)
        {
            pCommandAllocator->Release();
        }
    }


    //////////////////////////////////////////
    // Command Queues and Lists

    HCommandQueue D3D12GraphicsInterface::CreateCommandQueue(HGraphicsDevice hDevice, CommandListType type, unsigned int priority)
    {
        auto pDeviceD3D = reinterpret_cast<ID3D12Device*>(hDevice);
        assert(pDeviceD3D != nullptr);
        {
            D3D12_COMMAND_QUEUE_DESC desc;
            desc.Type     = g_D3DCommandListTypesTable[type];
            desc.Priority = priority;
            desc.Flags    = D3D12_COMMAND_QUEUE_NONE;
            desc.NodeMask = 0;

            ID3D12CommandQueue* pCommandQueueD3D = nullptr;
            pDeviceD3D->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&pCommandQueueD3D));


            return reinterpret_cast<HCommandQueue>(pCommandQueueD3D);
        }
    }

    void D3D12GraphicsInterface::DeleteCommandQueue(HCommandQueue hCommandQueue)
    {
        auto pCommandQueueD3D = reinterpret_cast<ID3D12CommandQueue*>(hCommandQueue);
        if (pCommandQueueD3D != nullptr)
        {
            pCommandQueueD3D->Release();
        }
    }

    void D3D12GraphicsInterface::SignalCommandQueue(HCommandQueue hCommandQueue, HFence hFence, uint64_t value)
    {
        auto pCommandQueue = reinterpret_cast<ID3D12CommandQueue*>(hCommandQueue);
        assert(pCommandQueue != nullptr);
        {
            pCommandQueue->Signal(reinterpret_cast<ID3D12Fence*>(hFence), value);
        }
    }

    void D3D12GraphicsInterface::Wait(HCommandQueue hCommandQueue, HFence hFence, uint64_t value)
    {
        auto pCommandQueue = reinterpret_cast<ID3D12CommandQueue*>(hCommandQueue);
        assert(pCommandQueue != nullptr);
        {
            pCommandQueue->Wait(reinterpret_cast<ID3D12Fence*>(hFence), value);
        }
    }


    HCommandList D3D12GraphicsInterface::CreateCommandList(HGraphicsDevice hDevice, CommandListType type, HCommandAllocator hCommandAllocator, HPipelineState hInitialGraphicsPipelineState)
    {
        auto pDeviceD3D = reinterpret_cast<ID3D12Device*>(hDevice);
        assert(pDeviceD3D != nullptr);
        {
            ID3D12GraphicsCommandList* pCommandList = nullptr;
            pDeviceD3D->CreateCommandList(0, g_D3DCommandListTypesTable[type], reinterpret_cast<ID3D12CommandAllocator*>(hCommandAllocator), reinterpret_cast<ID3D12PipelineState*>(hInitialGraphicsPipelineState), __uuidof(ID3D12GraphicsCommandList), reinterpret_cast<void**>(&pCommandList));


            return reinterpret_cast<HCommandList>(pCommandList);
        }
    }

    void D3D12GraphicsInterface::DeleteCommandList(HCommandList hCommandList)
    {
        auto pCommandListD3D = reinterpret_cast<ID3D12CommandList*>(hCommandList);
        if (pCommandListD3D != nullptr)
        {
            pCommandListD3D->Release();
        }
    }


    HDescriptorHeap D3D12GraphicsInterface::CreateDescriptorHeap(HGraphicsDevice hDevice, HeapType type, unsigned int descriptorCount, bool shaderVisible)
    {
        auto pDevice = reinterpret_cast<ID3D12Device*>(hDevice);
        assert(pDevice != nullptr);
        {
            D3D12_DESCRIPTOR_HEAP_TYPE typesD3D[] =
            {
                D3D12_CBV_SRV_UAV_DESCRIPTOR_HEAP,
                D3D12_SAMPLER_DESCRIPTOR_HEAP,
                D3D12_RTV_DESCRIPTOR_HEAP,
                D3D12_DSV_DESCRIPTOR_HEAP
            };

            D3D12_DESCRIPTOR_HEAP_DESC desc;
            desc.Type           = typesD3D[type];
            desc.NumDescriptors = descriptorCount;
            desc.Flags          = (shaderVisible) ? D3D12_DESCRIPTOR_HEAP_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_NONE;
            desc.NodeMask       = 0;

            ID3D12DescriptorHeap* pHeap = nullptr;
            pDevice->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&pHeap));



            return reinterpret_cast<HDescriptorHeap>(pHeap);
        }
    }

    void D3D12GraphicsInterface::DeleteDescriptorHeap(HDescriptorHeap hHeap)
    {
        auto pHeapD3D = reinterpret_cast<ID3D12DescriptorHeap*>(hHeap);
        if (pHeapD3D != nullptr)
        {
            pHeapD3D->Release();
        }
    }


    //////////////////////////////////////////
    // Synchronization

    HFence D3D12GraphicsInterface::CreateFence(HGraphicsDevice hDevice, uint64_t initialValue, FenceFlag flags)
    {
        auto pDevice = reinterpret_cast<ID3D12Device*>(hDevice);
        assert(pDevice != nullptr);
        {
            D3D12_FENCE_MISC_FLAG flagsD3D = static_cast<D3D12_FENCE_MISC_FLAG>(flags);     // <-- This is a safe cast.

            ID3D12Fence* pFence = nullptr;
            pDevice->CreateFence(initialValue, flagsD3D, __uuidof(ID3D12Fence), reinterpret_cast<void**>(&pFence));

            return reinterpret_cast<HFence>(pFence);
        }
    }

    void D3D12GraphicsInterface::DeleteFence(HFence hFence)
    {
        auto pFence = reinterpret_cast<ID3D12Fence*>(hFence);
        if (pFence != nullptr)
        {
            pFence->Release();
        }
    }

    void D3D12GraphicsInterface::SignalFence(HFence hFence, uint64_t value)
    {
        auto pFence = reinterpret_cast<ID3D12Fence*>(hFence);
        assert(pFence != nullptr);
        {
            pFence->Signal(value);
        }
    }

    void D3D12GraphicsInterface::SetEventOnCompletion(HFence hFence, uint64_t value, HANDLE hEvent)
    {
        auto pFence = reinterpret_cast<ID3D12Fence*>(hFence);
        assert(pFence != nullptr);
        {
            pFence->SetEventOnCompletion(value, hEvent);
        }
    }

    uint64_t D3D12GraphicsInterface::GetCompletedValue(HFence hFence)
    {
        auto pFence = reinterpret_cast<ID3D12Fence*>(hFence);
        assert(pFence != nullptr);
        {
            return pFence->GetCompletedValue();
        }
    }

    


    //////////////////////////////////////////
    // Rendering

    void D3D12GraphicsInterface::ResetCommandAllocator(HCommandAllocator hCommandAllocator)
    {
        auto pCommandAllocator = reinterpret_cast<ID3D12CommandAllocator*>(hCommandAllocator);
        assert(pCommandAllocator != nullptr);
        {
            pCommandAllocator->Reset();
        }
    }

    void D3D12GraphicsInterface::ResetCommandList(HCommandList hCommandList, HCommandAllocator hCommandAllocator, HPipelineState hPipelineState)
    {
        auto pCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(hCommandList);
        assert(pCommandList != nullptr);
        {
            pCommandList->Reset(reinterpret_cast<ID3D12CommandAllocator*>(hCommandAllocator), reinterpret_cast<ID3D12PipelineState*>(hPipelineState));
        }
    }

    void D3D12GraphicsInterface::Close(HCommandList hCommandList)
    {
        auto pCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(hCommandList);
        assert(pCommandList != nullptr);
        {
            pCommandList->Close();
        }
    }

    void D3D12GraphicsInterface::SetViewport(HCommandList hCommandList, const GraphicsViewport &viewport)
    {
        auto pCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(hCommandList);
        assert(pCommandList != nullptr);
        {
            D3D12_VIEWPORT viewportD3D;
            viewportD3D.TopLeftX = viewport.x;
            viewportD3D.TopLeftY = viewport.y;
            viewportD3D.Width    = viewport.width;
            viewportD3D.Height   = viewport.height;
            viewportD3D.MinDepth = viewport.depthRangeNear;
            viewportD3D.MaxDepth = viewport.depthRangeFar;

            pCommandList->RSSetViewports(1, &viewportD3D);
        }
    }


    // TEMP
    void D3D12GraphicsInterface::ClearWindow(HCommandList hCommandList, HWND hWnd, float* color)
    {
        auto pCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(hCommandList);
        assert(pCommandList != nullptr);
        {
            auto iWindowBufferData = m_windowBufferData.Find(hWnd);
            if (iWindowBufferData != nullptr)
            {
                auto &windowBufferData = iWindowBufferData->value;

                pCommandList->ClearRenderTargetView(windowBufferData.hDescriptorHandles[windowBufferData.pSwapChain->GetCurrentBackBufferIndex()], color, nullptr, 0);
            }
        }
    }

    void D3D12GraphicsInterface::ResourceBarrier(HCommandList hCommandList, HWND hWnd, GraphicsResourceUsage stateBefore, GraphicsResourceUsage stateAfter)
    {
        auto pCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(hCommandList);
        assert(pCommandList != nullptr);
        {
            auto iWindowBufferData = m_windowBufferData.Find(hWnd);
            if (iWindowBufferData != nullptr)
            {
                auto &windowBufferData = iWindowBufferData->value;

                UINT backBufferIndex = windowBufferData.pSwapChain->GetCurrentBackBufferIndex();

                D3D12_RESOURCE_BARRIER_DESC barrierDesc;
	            barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	            barrierDesc.Transition.pResource   = windowBufferData.pRenderTargets[backBufferIndex];
	            barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	            barrierDesc.Transition.StateBefore = g_D3DResourceUsagesTable[stateBefore];
	            barrierDesc.Transition.StateAfter  = g_D3DResourceUsagesTable[stateAfter];
                barrierDesc.Transition.Flags       = D3D12_RESOURCE_TRANSITION_BARRIER_NONE;

	            pCommandList->ResourceBarrier(1, &barrierDesc);
            }
        }
    }


    // Command Queue
    void D3D12GraphicsInterface::ExecuteCommandList(HCommandQueue hCommandQueue, HCommandList hCommandList)
    {
        auto pCommandQueue = reinterpret_cast<ID3D12CommandQueue*>(hCommandQueue);
        assert(pCommandQueue != nullptr);
        {
            auto pCommandList = reinterpret_cast<ID3D12CommandList*>(hCommandList);
            assert(pCommandList != nullptr);
            {
                pCommandQueue->ExecuteCommandLists(1, &pCommandList);
            }
        }
    }
}
