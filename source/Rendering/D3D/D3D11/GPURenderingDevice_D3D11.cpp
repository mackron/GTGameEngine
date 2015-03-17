// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)
#include "RenderingTypes_D3D11.hpp"
#include <GTLib/Strings/Find.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Parse.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    // GUID for use with SetPrivateData().
    const GUID GPURenderingDevice_D3D11::CustomDataGUID_Generic      = {0xD74A88E1, 0xDA90, 0x05AA, {0xDD, 0x01, 0xEA, 0xD3, 0x31, 0xB3, 0xC2, 0x00}};
    const GUID GPURenderingDevice_D3D11::CustomDataGUID_ShaderBinary = {0xD74A88E1, 0xDA90, 0x05AA, {0xDD, 0x01, 0xEA, 0xD3, 0x31, 0xB3, 0xC2, 0x01}};

    GPURenderingDevice_D3D11::GPURenderingDevice_D3D11(const GPURenderingDeviceInfo &info)
        : GPURenderingDevice(info),
          m_hD3D11(NULL),
          m_hD3DCompiler(NULL),
          m_device(nullptr),
          m_context(nullptr),
          m_windowFramebuffers(),
          m_currentHWND(0),
          m_currentSwapChain(nullptr),
          m_stateFlags(0),
          m_swapInterval(0),
          m_currentPrimitiveTopology(PrimitiveTopology_Triangle),
          m_currentBlendState(0),
          m_currentBlendFactor(),
          m_currentSampleMask(0xFFFFFFFF),
          m_currentFramebuffer(0),
          m_D3DCompile(nullptr)
    {
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
                    UINT flags = 0;
                    flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
                    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

                    ID3D11Device* tempDevice;
                    ID3D11DeviceContext* tempContext;

                    D3D_FEATURE_LEVEL    featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
                    D3D_FEATURE_LEVEL    featureLevel    = D3D_FEATURE_LEVEL_11_0;
                    HRESULT hr = _D3D11CreateDevice(reinterpret_cast<IDXGIAdapter1*>(m_info.identifier_D3D11), D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, featureLevels, sizeof(featureLevels) / sizeof(featureLevels[0]), D3D11_SDK_VERSION, &tempDevice, &featureLevel, &tempContext);
                    if (SUCCEEDED(hr) && featureLevel == D3D_FEATURE_LEVEL_11_0)
                    {
                        tempDevice->QueryInterface<ID3D11Device1>(&m_device);
                        tempContext->QueryInterface<ID3D11DeviceContext1>(&m_context);

                        // Release the original device and context objects.
                        tempDevice->Release();
                        tempContext->Release();


                        // Initialize the API.
                        m_D3DCompile = reinterpret_cast<pD3DCompile>(GetProcAddress(m_hD3DCompiler, "D3DCompile"));
                        if (m_D3DCompile == nullptr)
                        {
                            return FailedToLoadD3D11API;
                        }

                        m_D3DReflect = reinterpret_cast<pD3DReflect>(GetProcAddress(m_hD3DCompiler, "D3DReflect"));
                        if (m_D3DReflect == nullptr)
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

    GraphicsHandedness GPURenderingDevice_D3D11::GetHandedness() const
    {
        return GraphicsHandedness_Left;
    }



    void GPURenderingDevice_D3D11::SetSwapInterval(int swapInterval)
    {
        m_swapInterval = swapInterval;
    }


    ///////////////////////////////////////////
    // Drawing

    void GPURenderingDevice_D3D11::Draw(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex)
    {
        m_context->DrawIndexed(indexCount, startIndexLocation, baseVertex);
    }

    void GPURenderingDevice_D3D11::DrawInstanced(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex, unsigned int instanceCount, unsigned int baseInstance)
    {
        m_context->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertex, baseInstance);
    }



    ///////////////////////////////////////////
    // State

    void GPURenderingDevice_D3D11::BindTexture(unsigned int slotIndex, HTextureView hTextureView)
    {
        // Bind the texture on all stages.
        ID3D11ShaderResourceView* viewD3D11 = reinterpret_cast<ID3D11ShaderResourceView*>(hTextureView);
        
        m_context->VSSetShaderResources(slotIndex, 1, &viewD3D11);
        m_context->DSSetShaderResources(slotIndex, 1, &viewD3D11);
        m_context->HSSetShaderResources(slotIndex, 1, &viewD3D11);
        m_context->GSSetShaderResources(slotIndex, 1, &viewD3D11);
        m_context->PSSetShaderResources(slotIndex, 1, &viewD3D11);
        m_context->CSSetShaderResources(slotIndex, 1, &viewD3D11);
    }

    void GPURenderingDevice_D3D11::BindSampler(unsigned int slotIndex, HSampler hSampler)
    {
        // Bind to all stages.
        ID3D11SamplerState* samplerD3D11 = reinterpret_cast<ID3D11SamplerState*>(hSampler);

        m_context->VSSetSamplers(slotIndex, 1, &samplerD3D11);
        m_context->DSSetSamplers(slotIndex, 1, &samplerD3D11);
        m_context->HSSetSamplers(slotIndex, 1, &samplerD3D11);
        m_context->GSSetSamplers(slotIndex, 1, &samplerD3D11);
        m_context->PSSetSamplers(slotIndex, 1, &samplerD3D11);
        m_context->CSSetSamplers(slotIndex, 1, &samplerD3D11);
    }

    void GPURenderingDevice_D3D11::BindConstantBuffer(unsigned int slotIndex, HBuffer hBuffer)
    {
        // Bind to all stages.
        ID3D11Buffer* bufferD3D11 = reinterpret_cast<ID3D11Buffer*>(hBuffer);

        m_context->VSSetConstantBuffers(slotIndex, 1, &bufferD3D11);
        m_context->DSSetConstantBuffers(slotIndex, 1, &bufferD3D11);
        m_context->HSSetConstantBuffers(slotIndex, 1, &bufferD3D11);
        m_context->GSSetConstantBuffers(slotIndex, 1, &bufferD3D11);
        m_context->PSSetConstantBuffers(slotIndex, 1, &bufferD3D11);
        m_context->CSSetConstantBuffers(slotIndex, 1, &bufferD3D11);
    }

    void GPURenderingDevice_D3D11::BindConstantBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t offset, size_t size)
    {
        // Bind to all stages.
        ID3D11Buffer* bufferD3D11 = reinterpret_cast<ID3D11Buffer*>(hBuffer);

        UINT offsetD3D = static_cast<UINT>(offset);
        UINT sizeD3D   = static_cast<UINT>(size);

        m_context->VSSetConstantBuffers1(slotIndex, 1, &bufferD3D11, &offsetD3D, &sizeD3D);
        m_context->DSSetConstantBuffers1(slotIndex, 1, &bufferD3D11, &offsetD3D, &sizeD3D);
        m_context->HSSetConstantBuffers1(slotIndex, 1, &bufferD3D11, &offsetD3D, &sizeD3D);
        m_context->GSSetConstantBuffers1(slotIndex, 1, &bufferD3D11, &offsetD3D, &sizeD3D);
        m_context->PSSetConstantBuffers1(slotIndex, 1, &bufferD3D11, &offsetD3D, &sizeD3D);
        m_context->CSSetConstantBuffers1(slotIndex, 1, &bufferD3D11, &offsetD3D, &sizeD3D);
    }

    void GPURenderingDevice_D3D11::BindConstantBuffer(unsigned int slotIndex, size_t count, HBuffer* hBuffers, size_t* offsets, size_t* sizes)
    {
        if (count < GT_MAX_CONSTANT_BUFFER_SLOTS)
        {
            ID3D11Buffer* buffersD3D[GT_MAX_CONSTANT_BUFFER_SLOTS];
            UINT offsetsD3D[GT_MAX_CONSTANT_BUFFER_SLOTS];
            UINT sizesD3D[GT_MAX_CONSTANT_BUFFER_SLOTS];

            for (size_t iSlot = 0; iSlot < count; ++iSlot)
            {
                buffersD3D[iSlot] = reinterpret_cast<ID3D11Buffer*>(hBuffers[iSlot]);
                offsetsD3D[iSlot] = static_cast<UINT>(offsets[iSlot]);
                sizesD3D[iSlot]   = static_cast<UINT>(sizes[iSlot]);
            }


            m_context->VSSetConstantBuffers1(slotIndex, 1, buffersD3D, offsetsD3D, sizesD3D);
            m_context->DSSetConstantBuffers1(slotIndex, 1, buffersD3D, offsetsD3D, sizesD3D);
            m_context->HSSetConstantBuffers1(slotIndex, 1, buffersD3D, offsetsD3D, sizesD3D);
            m_context->GSSetConstantBuffers1(slotIndex, 1, buffersD3D, offsetsD3D, sizesD3D);
            m_context->PSSetConstantBuffers1(slotIndex, 1, buffersD3D, offsetsD3D, sizesD3D);
            m_context->CSSetConstantBuffers1(slotIndex, 1, buffersD3D, offsetsD3D, sizesD3D);
        }
        else
        {
            // Too many buffers were specified.
        }
    }

    void GPURenderingDevice_D3D11::BindFramebuffer(HFramebuffer hFramebuffer)
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            m_context->OMSetRenderTargets(GT_MAX_FRAMEBUFFER_RENDER_TARGETS, framebufferD3D->renderTargets, framebufferD3D->depthStencilView);
        }
        else
        {
            // Make the window framebuffer current.
            if (m_currentHWND != nullptr)
            {
                auto iFramebuffer = m_windowFramebuffers.Find(m_currentHWND);
                assert(iFramebuffer != nullptr);
                {
                    auto &framebuffer = iFramebuffer->value;
                    m_context->OMSetRenderTargets(1, &framebuffer.renderTargetView, framebuffer.depthStencilView);
                }
            }
            else
            {
                m_context->OMSetRenderTargets(1, nullptr, nullptr);
            }
        }

        m_currentFramebuffer = hFramebuffer;
    }


    void GPURenderingDevice_D3D11::SetVertexShader(HShader hShader)
    {
        m_context->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(hShader), nullptr, 0);
    }

    void GPURenderingDevice_D3D11::SetTessellationControlShader(HShader hShader)
    {
        m_context->HSSetShader(reinterpret_cast<ID3D11HullShader*>(hShader), nullptr, 0);
    }

    void GPURenderingDevice_D3D11::SetTessellationEvaluationShader(HShader hShader)
    {
        m_context->DSSetShader(reinterpret_cast<ID3D11DomainShader*>(hShader), nullptr, 0);
    }

    void GPURenderingDevice_D3D11::SetGeometryShader(HShader hShader)
    {
        m_context->GSSetShader(reinterpret_cast<ID3D11GeometryShader*>(hShader), nullptr, 0);
    }

    void GPURenderingDevice_D3D11::SetFragmentShader(HShader hShader)
    {
        m_context->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(hShader), nullptr, 0);
    }

    void GPURenderingDevice_D3D11::SetComputeShader(HShader hShader)
    {
        m_context->CSSetShader(reinterpret_cast<ID3D11ComputeShader*>(hShader), nullptr, 0);
    }



    /////////////////////////////////////////////////////////////////////////////
    //
    // Stages
    //
    /////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////
    // Input-Assembler Stage

    void GPURenderingDevice_D3D11::IASetPrimitiveTopology(PrimitiveTopology topology)
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

    void GPURenderingDevice_D3D11::IASetInputLayout(HVSInputLayout hInputLayout)
    {
        m_context->IASetInputLayout(reinterpret_cast<ID3D11InputLayout*>(hInputLayout));
    }

    void GPURenderingDevice_D3D11::IASetVertexBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t stride, size_t offset)
    {
        auto bufferD3D11 = reinterpret_cast<ID3D11Buffer*>(hBuffer);
        UINT strideD3D11 = static_cast<UINT>(stride);
        UINT offsetD3D11 = static_cast<UINT>(offset);
        m_context->IASetVertexBuffers(slotIndex, 1, &bufferD3D11, &strideD3D11, &offsetD3D11);
    }

    void GPURenderingDevice_D3D11::IASetIndexBuffer(HBuffer hBuffer, IndexFormat format, size_t offset)
    {
        DXGI_FORMAT formats[] =
        {
            DXGI_FORMAT_R8_UINT,
            DXGI_FORMAT_R16_UINT,
            DXGI_FORMAT_R32_UINT
        };

        m_context->IASetIndexBuffer(reinterpret_cast<ID3D11Buffer*>(hBuffer), formats[format], static_cast<UINT>(offset));
    }


    /////////////////////////////////////////////
    // Rasterization Stage

    void GPURenderingDevice_D3D11::RSSetState(HRasterizerState hState)
    {
        m_context->RSSetState(reinterpret_cast<ID3D11RasterizerState*>(hState));
    }

    void GPURenderingDevice_D3D11::RSSetViewports(GraphicsViewport* viewports, size_t viewportCount)
    {
        D3D11_VIEWPORT viewportsD3D[GT_MAX_VIEWPORTS];
        for (size_t iViewport = 0; iViewport < viewportCount; ++iViewport)
        {
            viewportsD3D[iViewport].TopLeftX = viewports[iViewport].x;
            viewportsD3D[iViewport].TopLeftY = viewports[iViewport].y;
            viewportsD3D[iViewport].Width    = viewports[iViewport].width;
            viewportsD3D[iViewport].Height   = viewports[iViewport].height;
            viewportsD3D[iViewport].MinDepth = viewports[iViewport].depthRangeNear;
            viewportsD3D[iViewport].MaxDepth = viewports[iViewport].depthRangeFar;
        }

        m_context->RSSetViewports(static_cast<UINT>(viewportCount), viewportsD3D);
    }

    void GPURenderingDevice_D3D11::RSSetScissorRects(ScissorRect* scissorRects, size_t scissorCount)
    {
        RECT scissorRectsD3D[GT_MAX_VIEWPORTS];
        for (size_t iScissor = 0; iScissor < scissorCount; ++iScissor)
        {
            scissorRectsD3D[iScissor].left   = static_cast<LONG>(scissorRects[iScissor].x);
            scissorRectsD3D[iScissor].top    = static_cast<LONG>(scissorRects[iScissor].y);
            scissorRectsD3D[iScissor].right  = static_cast<LONG>(scissorRects[iScissor].x + scissorRects[iScissor].width);
            scissorRectsD3D[iScissor].bottom = static_cast<LONG>(scissorRects[iScissor].y + scissorRects[iScissor].height);
        }

        m_context->RSSetScissorRects(static_cast<UINT>(scissorCount), scissorRectsD3D);
    }


    /////////////////////////////////////////////
    // Output Merger Stage

    void GPURenderingDevice_D3D11::OMSetDepthStencilState(HDepthStencilState hState, unsigned int stencilRef)
    {
        m_context->OMSetDepthStencilState(reinterpret_cast<ID3D11DepthStencilState*>(hState), stencilRef);
    }

    void GPURenderingDevice_D3D11::OMSetBlendState(HBlendState hState)
    {
        m_context->OMSetBlendState(reinterpret_cast<ID3D11BlendState*>(hState), m_currentBlendFactor, m_currentSampleMask);
        m_currentBlendState = hState;
    }

    void GPURenderingDevice_D3D11::OMSetBlendFactor(float blendFactor[4])
    {
        m_currentBlendFactor[0] = blendFactor[0];
        m_currentBlendFactor[1] = blendFactor[1];
        m_currentBlendFactor[2] = blendFactor[2];
        m_currentBlendFactor[3] = blendFactor[3];
        this->OMSetBlendState(m_currentBlendState);
    }

    void GPURenderingDevice_D3D11::OMSetSampleMask(uint32_t sampleMask)
    {
        m_currentSampleMask = static_cast<UINT>(sampleMask);
        this->OMSetBlendState(m_currentBlendState);
    }


    /////////////////////////////////////////////////////////////////////////////
    //
    // Object Creation and Deletion
    //
    /////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////
    // State Objects

    HRasterizerState GPURenderingDevice_D3D11::CreateRasterizerState(const RasterizerStateDesc &desc)
    {
        D3D11_FILL_MODE fillModesD3D[] =
        {
            D3D11_FILL_WIREFRAME,
            D3D11_FILL_SOLID
        };

        D3D11_CULL_MODE cullModesD3D[] =
        {
            D3D11_CULL_NONE,
            D3D11_CULL_FRONT,
            D3D11_CULL_BACK
        };


        D3D11_RASTERIZER_DESC descD3D;
        descD3D.FillMode              = fillModesD3D[desc.fillMode];
        descD3D.CullMode              = cullModesD3D[desc.cullMode];
        descD3D.FrontCounterClockwise = (desc.polygonWinding == PolygonWinding_CW) ? TRUE : FALSE;
        descD3D.DepthBias             = desc.depthBias;
        descD3D.DepthBiasClamp        = desc.depthBiasClamp;
        descD3D.SlopeScaledDepthBias  = desc.slopeScaledDepthBias;
        descD3D.DepthClipEnable       = desc.enableDepthClip;
        descD3D.MultisampleEnable     = desc.enableMultisample;
        descD3D.AntialiasedLineEnable = desc.enableAntialiasedLine;
            
        ID3D11RasterizerState* rasterizerStateD3D11;
        if (SUCCEEDED(m_device->CreateRasterizerState(&descD3D, &rasterizerStateD3D11)))
        {
            return reinterpret_cast<HRasterizerState>(rasterizerStateD3D11);
        }
        else
        {
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteRasterizerState(HRasterizerState hState)
    {
        if (hState != 0)
        {
            reinterpret_cast<ID3D11RasterizerState*>(hState)->Release();
        }
    }


    HDepthStencilState GPURenderingDevice_D3D11::CreateDepthStencilState(const DepthStencilStateDesc &desc)
    {
        D3D11_DEPTH_WRITE_MASK depthWriteMasks[] =
        {
            D3D11_DEPTH_WRITE_MASK_ZERO,
            D3D11_DEPTH_WRITE_MASK_ALL
        };

        D3D11_COMPARISON_FUNC comparisonFuncs[] =
        {
            D3D11_COMPARISON_NEVER,
            D3D11_COMPARISON_LESS,
            D3D11_COMPARISON_EQUAL,
            D3D11_COMPARISON_LESS_EQUAL,
            D3D11_COMPARISON_GREATER,
            D3D11_COMPARISON_NOT_EQUAL,
            D3D11_COMPARISON_GREATER_EQUAL,
            D3D11_COMPARISON_ALWAYS
        };

        D3D11_STENCIL_OP stencilOps[] =
        {
            D3D11_STENCIL_OP_KEEP,
            D3D11_STENCIL_OP_ZERO,
            D3D11_STENCIL_OP_REPLACE,
            D3D11_STENCIL_OP_INCR_SAT,
            D3D11_STENCIL_OP_DECR_SAT,
            D3D11_STENCIL_OP_INVERT,
            D3D11_STENCIL_OP_INCR,
            D3D11_STENCIL_OP_DECR
        };


        D3D11_DEPTH_STENCIL_DESC descD3D;
        descD3D.DepthEnable                  = desc.enableDepthTest;
        descD3D.DepthWriteMask               = depthWriteMasks[desc.depthWriteMask];
        descD3D.DepthFunc                    = comparisonFuncs[desc.depthFunc];
        descD3D.StencilEnable                = desc.enableStencilTest;
        descD3D.StencilReadMask              = desc.stencilReadMask;
        descD3D.StencilWriteMask             = desc.stencilWriteMask;
        descD3D.FrontFace.StencilFailOp      = stencilOps[desc.stencilFrontFaceOp.stencilFailOp];
        descD3D.FrontFace.StencilDepthFailOp = stencilOps[desc.stencilFrontFaceOp.stencilDepthFailOp];
        descD3D.FrontFace.StencilPassOp      = stencilOps[desc.stencilFrontFaceOp.stencilPassOp];
        descD3D.FrontFace.StencilFunc        = comparisonFuncs[desc.stencilFrontFaceOp.stencilFunc];
        descD3D.BackFace.StencilFailOp       = stencilOps[desc.stencilBackFaceOp.stencilFailOp];
        descD3D.BackFace.StencilDepthFailOp  = stencilOps[desc.stencilBackFaceOp.stencilDepthFailOp];
        descD3D.BackFace.StencilPassOp       = stencilOps[desc.stencilBackFaceOp.stencilPassOp];
        descD3D.BackFace.StencilFunc         = comparisonFuncs[desc.stencilBackFaceOp.stencilFunc];

        ID3D11DepthStencilState* depthStencilStateD3D11;
        if (SUCCEEDED(m_device->CreateDepthStencilState(&descD3D, &depthStencilStateD3D11)))
        {
            return reinterpret_cast<HDepthStencilState>(depthStencilStateD3D11);
        }
        else
        {
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteDepthStencilState(HDepthStencilState hState)
    {
        if (hState != 0)
        {
            reinterpret_cast<ID3D11DepthStencilState*>(hState)->Release();
        }
    }


    HBlendState GPURenderingDevice_D3D11::CreateBlendState(const BlendStateDesc &desc)
    {
        D3D11_BLEND blendParametersD3D[] =
        {
            D3D11_BLEND_ZERO,                   // BlendParameter_Zero
            D3D11_BLEND_ONE,                    // BlendParameter_One

            D3D11_BLEND_SRC_COLOR,              // BlendParameter_Src_Color
            D3D11_BLEND_INV_SRC_COLOR,          // BlendParameter_Inv_Src_Color
            D3D11_BLEND_SRC_ALPHA,              // BlendParameter_Src_Alpha
            D3D11_BLEND_INV_SRC_ALPHA,          // BlendParameter_Inv_Src_Alpha
            D3D11_BLEND_SRC_ALPHA_SAT,          // BlendParameter_Src_Alpha_Saturate
        
            D3D11_BLEND_DEST_COLOR,             // BlendParameter_Dst_Color
            D3D11_BLEND_INV_DEST_COLOR,         // BlendParameter_Inv_Dst_Color
            D3D11_BLEND_DEST_ALPHA,             // BlendParameter_Dst_Alpha
            D3D11_BLEND_INV_DEST_ALPHA,         // BlendParameter_Inv_Dst_Alpha
        
            D3D11_BLEND_SRC1_COLOR,             // BlendParameter_Src1_Color
            D3D11_BLEND_INV_SRC1_COLOR,         // BlendParameter_Inv_Src1_Color
            D3D11_BLEND_SRC1_ALPHA,             // BlendParameter_Src1_Alpha
            D3D11_BLEND_INV_SRC1_ALPHA,         // BlendParameter_Inv_Src1_Alpha

            D3D11_BLEND_BLEND_FACTOR,           // BlendParameter_BlendFactor
            D3D11_BLEND_INV_BLEND_FACTOR        // BlendParameter_Inv_BlendFactor
        };

        D3D11_BLEND_OP blendOpsD3D[] =
        {
            D3D11_BLEND_OP_ADD,                 // BlendOp_Add
            D3D11_BLEND_OP_SUBTRACT,            // BlendOp_Subtract
            D3D11_BLEND_OP_REV_SUBTRACT,        // BlendOp_Reverse_Subtract
            D3D11_BLEND_OP_MIN,                 // BlendOp_Min
            D3D11_BLEND_OP_MAX                  // BlendOp_Max
        };


        D3D11_BLEND_DESC descD3D;
        descD3D.AlphaToCoverageEnable  = desc.enableAlphaToCoverage;
        descD3D.IndependentBlendEnable = desc.enableIndependentBlend;

        for (int iRT = 0; iRT < 8; ++iRT)
        {
            descD3D.RenderTarget[iRT].BlendEnable           = desc.renderTarget[iRT].enableBlending;
            descD3D.RenderTarget[iRT].SrcBlend              = blendParametersD3D[desc.renderTarget[iRT].srcBlendParameter];
            descD3D.RenderTarget[iRT].DestBlend             = blendParametersD3D[desc.renderTarget[iRT].dstBlendParameter];
            descD3D.RenderTarget[iRT].BlendOp               = blendOpsD3D[desc.renderTarget[iRT].blendOp];
            descD3D.RenderTarget[iRT].SrcBlendAlpha         = blendParametersD3D[desc.renderTarget[iRT].srcAlphaBlendParameter];
            descD3D.RenderTarget[iRT].DestBlendAlpha        = blendParametersD3D[desc.renderTarget[iRT].dstAlphaBlendParameter];
            descD3D.RenderTarget[iRT].BlendOpAlpha          = blendOpsD3D[desc.renderTarget[iRT].blendOpAlpha];
            descD3D.RenderTarget[iRT].RenderTargetWriteMask = desc.renderTarget[iRT].writeMask;
        }


        ID3D11BlendState* blendStateD3D11;
        if (SUCCEEDED(m_device->CreateBlendState(&descD3D, &blendStateD3D11)))
        {
            return reinterpret_cast<HBlendState>(blendStateD3D11);
        }
        else
        {
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteBlendState(HBlendState hState)
    {
        if (hState != 0)
        {
            reinterpret_cast<ID3D11BlendState*>(hState)->Release();
        }
    }


    ////////////////////////////////////////////
    // Input Layout

    HVSInputLayout GPURenderingDevice_D3D11::CreateVSInputLayout(HShader hVertexShader, const VSInputAttribFormat* attribDesc, size_t attribDescCount)
    {
        // Validate the input layout.
        if (!IsInputLayoutValid(attribDesc, attribDescCount))
        {
            return 0;
        }

        auto shaderD3D11 = reinterpret_cast<ID3D11VertexShader*>(hVertexShader);
        if (shaderD3D11 != nullptr)
        {
            UINT vertexShaderDataSize = 0;
            shaderD3D11->GetPrivateData(CustomDataGUID_ShaderBinary, &vertexShaderDataSize, nullptr);

            if (vertexShaderDataSize > 0)
            {
                void* vertexShaderData = malloc(vertexShaderDataSize);
                if (vertexShaderData != nullptr)
                {
                    shaderD3D11->GetPrivateData(CustomDataGUID_ShaderBinary, &vertexShaderDataSize, vertexShaderData);

                    auto attribDescD3D = new D3D11_INPUT_ELEMENT_DESC[attribDescCount];
                    assert(attribDescD3D != nullptr);
                    {
                        memset(attribDescD3D, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * attribDescCount);


                        ResultCode result = 0;

                        for (size_t iAttrib = 0; iAttrib < attribDescCount; ++iAttrib)
                        {
                            assert(attribDesc[iAttrib].attributeComponentCount >= 1 && attribDesc[iAttrib].attributeComponentCount <=4);

                            auto semanticIndexStart = GTLib::Strings::FindFirstOf(attribDesc[iAttrib].attributeName, "0123456789");

                            ptrdiff_t semanticNameLength = semanticIndexStart - attribDesc[iAttrib].attributeName;
                            char* semanticName = reinterpret_cast<char*>(malloc(semanticNameLength + 1));
                            if (semanticName != nullptr)
                            {
                                memcpy(semanticName, attribDesc[iAttrib].attributeName, semanticNameLength);
                                semanticName[semanticNameLength] = '\0';
                            }
                            else
                            {
                                // Failed to allocate memory.
                                result = -4;
                                break;
                            }


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


                            attribDescD3D[iAttrib].SemanticName         = semanticName;
                            attribDescD3D[iAttrib].SemanticIndex        = GTLib::Parse<UINT>(semanticIndexStart);
                            attribDescD3D[iAttrib].Format               = format[attribDesc[iAttrib].attributeComponentType][attribDesc[iAttrib].attributeComponentCount - 1];
                            attribDescD3D[iAttrib].InputSlot            = attribDesc[iAttrib].slotIndex;
                            attribDescD3D[iAttrib].AlignedByteOffset    = attribDesc[iAttrib].attributeOffset;
                            attribDescD3D[iAttrib].InputSlotClass       = (attribDesc[iAttrib].attributeClass == AttribInputClassification_PerVertex) ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
                            attribDescD3D[iAttrib].InstanceDataStepRate = attribDesc[iAttrib].instanceStepRate;
                        }


                        ID3D11InputLayout* inputLayoutD3D = nullptr;
                        if (GT::Succeeded(result))
                        {
                            m_device->CreateInputLayout(attribDescD3D, static_cast<UINT>(attribDescCount), vertexShaderData, vertexShaderDataSize, &inputLayoutD3D);
                        }
                        


                        for (size_t iAttrib = 0; iAttrib < attribDescCount; ++iAttrib)
                        {
                            free(const_cast<void*>(reinterpret_cast<const void*>(attribDescD3D[iAttrib].SemanticName)));
                        }

                        delete [] attribDescD3D;
                        free(vertexShaderData);


                        return reinterpret_cast<HVSInputLayout>(inputLayoutD3D);
                    }
                }
                else
                {
                    // Failed to allocate memory.
                    return 0;
                }
            }
            else
            {
                // Vertex shader binary data is 0 bytes.
                return 0;
            }
        }
        else
        {
            // No shader program specified.
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteVSInputLayout(HVSInputLayout hInputLayout)
    {
        if (hInputLayout != 0)
        {
            reinterpret_cast<ID3D11InputLayout*>(hInputLayout)->Release();
        }
    }


    ////////////////////////////////////////////
    // Shaders

    ResultCode GPURenderingDevice_D3D11::CompileShader(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
    {
        if (language >= ShaderLanguage_HLSL_50 && language <= ShaderLanguage_HLSL_50)
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
            const char* targetD3D11 = targetStrs[type];


            ID3DBlob* shaderCompiled = NULL;
            ID3DBlob* shaderErrors   = NULL;
            HRESULT hr = m_D3DCompile(source, (sourceLength > 0) ? sourceLength : strlen(source), nullptr, nullptr, nullptr, "main", targetD3D11, D3DCOMPILE_ENABLE_STRICTNESS, 0, &shaderCompiled, &shaderErrors);
            if (SUCCEEDED(hr))
            {
                if (shaderCompiled != nullptr)
                {
                    ResultCode result = this->CreateShaderBinaryData(source, sourceLength, defines, language, type, shaderCompiled->GetBufferPointer(), shaderCompiled->GetBufferSize(), 0, byteCodeOut);

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

    bool GPURenderingDevice_D3D11::IsShaderLanguageSupported(ShaderLanguage language) const
    {
        return (language >= ShaderLanguage_HLSL_50 && language <= ShaderLanguage_HLSL_50);
    }


    HShader GPURenderingDevice_D3D11::CreateShader(const void* shaderData, size_t shaderDataSize, ShaderType shaderType)
    {
        const void* binaryData;
        size_t binaryDataSize;
        ResultCode result = this->ExtractShaderBinaryData(shaderData, shaderDataSize, binaryData, binaryDataSize);
        if (GT::Succeeded(result))
        {
            ID3D11DeviceChild* shaderD3D11 = nullptr;
            
            switch (shaderType)
            {
            case ShaderType_Vertex:
                {
                    if (FAILED(m_device->CreateVertexShader(binaryData, binaryDataSize, nullptr, reinterpret_cast<ID3D11VertexShader**>(&shaderD3D11))))
                    {
                        return 0;
                    }

                    break;
                }

            case ShaderType_TessellationControl:
                {
                    if (FAILED(m_device->CreateHullShader(binaryData, binaryDataSize, nullptr, reinterpret_cast<ID3D11HullShader**>(&shaderD3D11))))
                    {
                        return 0;
                    }

                    break;
                }

            case ShaderType_TessellationEvaluation:
                {
                    if (FAILED(m_device->CreateDomainShader(binaryData, binaryDataSize, nullptr, reinterpret_cast<ID3D11DomainShader**>(&shaderD3D11))))
                    {
                        return 0;
                    }

                    break;
                }

            case ShaderType_Geometry:
                {
                    if (FAILED(m_device->CreateGeometryShader(binaryData, binaryDataSize, nullptr, reinterpret_cast<ID3D11GeometryShader**>(&shaderD3D11))))
                    {
                        return 0;
                    }

                    break;
                }

            case ShaderType_Fragment:
                {
                    if (FAILED(m_device->CreatePixelShader(binaryData, binaryDataSize, nullptr, reinterpret_cast<ID3D11PixelShader**>(&shaderD3D11))))
                    {
                        return 0;
                    }

                    break;
                }

            case ShaderType_Compute:
                {
                    if (FAILED(m_device->CreateComputeShader(binaryData, binaryDataSize, nullptr, reinterpret_cast<ID3D11ComputeShader**>(&shaderD3D11))))
                    {
                        return 0;
                    }

                    break;
                }

            default:
                {
                    // Unknown or unsupported shader type.
                    return 0;
                }
            }

            assert(shaderD3D11 != nullptr);
            {
                if (SUCCEEDED(shaderD3D11->SetPrivateData(CustomDataGUID_ShaderBinary, static_cast<UINT>(binaryDataSize), binaryData)))
                {
                    return reinterpret_cast<HShader>(shaderD3D11);
                }
                else
                {
                    shaderD3D11->Release();
                    return 0;
                }
            }
        }
        else
        {
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteShader(HShader hShader)
    {
        if (hShader != 0)
        {
            reinterpret_cast<ID3D11DeviceChild*>(hShader)->Release();
        }
    }


    ///////////////////////////////////////////
    // Buffers

    HBuffer GPURenderingDevice_D3D11::CreateBuffer(BufferType type, BufferUsage usage, BufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data)
    {
        if (usage == BufferUsage_Immutable && data == nullptr)
        {
            // No data specified for immutable buffer. Immutable buffers must have their data set a creation time.
            return 0;
        }


        D3D11_USAGE usages[] =
        {
            D3D11_USAGE_DEFAULT,
            D3D11_USAGE_IMMUTABLE,
            D3D11_USAGE_DYNAMIC,
            D3D11_USAGE_STAGING,
        };

        UINT bindFlags[] =
        {
            D3D11_BIND_VERTEX_BUFFER,
            D3D11_BIND_INDEX_BUFFER,
            D3D11_BIND_CONSTANT_BUFFER
        };



        D3D11_BUFFER_DESC bd;
	    ZeroMemory(&bd, sizeof(bd));
        bd.Usage          = usages[usage];
        bd.ByteWidth      = static_cast<UINT>(sizeInBytes);
        bd.BindFlags      = bindFlags[type];
	    bd.CPUAccessFlags = 0;

        if ((cpuAccessFlags & BufferCPUAccess_Read) != 0)
        {
            bd.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
        }
        if ((cpuAccessFlags & BufferCPUAccess_Write) != 0)
        {
            bd.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
        }


        D3D11_SUBRESOURCE_DATA initialData;
	    ZeroMemory(&initialData, sizeof(D3D11_SUBRESOURCE_DATA));
        initialData.pSysMem = data;


        ID3D11Buffer* bufferD3D11;
        if (SUCCEEDED(m_device->CreateBuffer(&bd, (data != nullptr) ? &initialData : nullptr, &bufferD3D11)))
        {
            return reinterpret_cast<HBuffer>(bufferD3D11);
        }
        else
        {
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteBuffer(HBuffer hBuffer)
    {
        if (hBuffer != 0)
        {
            reinterpret_cast<ID3D11Buffer*>(hBuffer)->Release();
        }
    }

    void* GPURenderingDevice_D3D11::MapBuffer(HBuffer hBuffer, BufferMapType mapType)
    {
        D3D11_MAP mapTypes[] =
        {
            D3D11_MAP_READ,
            D3D11_MAP_WRITE,
            D3D11_MAP_READ_WRITE,
            D3D11_MAP_WRITE_DISCARD,
            D3D11_MAP_WRITE_NO_OVERWRITE,
        };

        D3D11_MAPPED_SUBRESOURCE mappingInfo;
        if (SUCCEEDED(m_context->Map(reinterpret_cast<ID3D11Buffer*>(hBuffer), 0, mapTypes[mapType], 0, &mappingInfo)))
        {
            return mappingInfo.pData;
        }
        else
        {
            return nullptr;
        }
    }

    void GPURenderingDevice_D3D11::UnmapBuffer(HBuffer hBuffer)
    {
        m_context->Unmap(reinterpret_cast<ID3D11Buffer*>(hBuffer), 0);
    }

    void GPURenderingDevice_D3D11::SetBufferData(HBuffer hBuffer, size_t offsetInBytes, size_t sizeInBytes, const void* data)
    {
        auto bufferD3D11 = reinterpret_cast<ID3D11Buffer*>(hBuffer);
        assert(bufferD3D11 != nullptr);
        {
            // Make sure the buffer is not immutable.
            D3D11_BUFFER_DESC desc;
            bufferD3D11->GetDesc(&desc);

            if (desc.Usage != D3D11_USAGE_IMMUTABLE)
            {
                D3D11_BOX box;
                box.left   = static_cast<UINT>(offsetInBytes);
                box.right  = static_cast<UINT>(offsetInBytes + sizeInBytes);
                box.top    = 0;
                box.bottom = 1;
                box.front  = 0;
                box.back   = 1;

                m_context->UpdateSubresource(bufferD3D11, 0, &box, data, static_cast<UINT>(sizeInBytes), 0);
            }
        }
    }



    ///////////////////////////////////////////
    // Textures

    HTexture GPURenderingDevice_D3D11::CreateTexture(const TextureDesc &desc)
    {
        UINT bindFlagsD3D = 0;
        if ((desc.usage & TextureUsage_ShaderResource) != 0)
        {
            bindFlagsD3D |= D3D11_BIND_SHADER_RESOURCE;
        }
        if ((desc.usage & TextureUsage_RenderTarget) != 0)
        {
            bindFlagsD3D |= D3D11_BIND_RENDER_TARGET;
        }
        if ((desc.usage & TextureUsage_DepthStencilTarget) != 0)
        {
            bindFlagsD3D |= D3D11_BIND_DEPTH_STENCIL;
        }

        D3D11_USAGE usageD3D = D3D11_USAGE_DEFAULT;         // <-- TODO: Might want to make this customizable.


        switch (desc.type)
        {
        case TextureType_1D:
        case TextureType_1D_Array:
            {
                D3D11_TEXTURE1D_DESC descD3D;
                descD3D.Width          = desc.width;
                descD3D.MipLevels      = desc.mipmapLevels;
                descD3D.ArraySize      = desc.height;
                descD3D.Format         = g_D3DTextureFormatsTable[desc.format];
                descD3D.Usage          = usageD3D;
                descD3D.BindFlags      = bindFlagsD3D;
                descD3D.CPUAccessFlags = 0;
                descD3D.MiscFlags      = 0;

                ID3D11Texture1D* textureD3D11 = nullptr;
                if (SUCCEEDED(m_device->CreateTexture1D(&descD3D, nullptr, &textureD3D11)))
                {
                    textureD3D11->SetPrivateData(CustomDataGUID_Generic, sizeof(desc), &desc);
                }

                return reinterpret_cast<HTexture>(textureD3D11);
            }

        case TextureType_2D:
        case TextureType_2D_Array:
        case TextureType_2D_Multisample:
        case TextureType_2D_Multisample_Array:
        case TextureType_Cube:
        case TextureType_Cube_Array:
            {
                D3D11_TEXTURE2D_DESC descD3D;
                descD3D.Width              = desc.width;
                descD3D.Height             = desc.height;
                descD3D.MipLevels          = desc.mipmapLevels;
                descD3D.ArraySize          = desc.depth;
                descD3D.Format             = g_D3DTextureFormatsTable[desc.format];
                descD3D.SampleDesc.Count   = desc.sampleCount;
                descD3D.SampleDesc.Quality = 0;
                descD3D.Usage              = usageD3D;
                descD3D.BindFlags          = bindFlagsD3D;
                descD3D.CPUAccessFlags     = 0;
                descD3D.MiscFlags          = 0;

                if (desc.type != TextureType_2D_Multisample && desc.type != TextureType_2D_Multisample_Array)
                {
                    descD3D.SampleDesc.Count = 1;
                }


                ID3D11Texture2D* textureD3D11 = nullptr;
                if (SUCCEEDED(m_device->CreateTexture2D(&descD3D, nullptr, &textureD3D11)))
                {
                    textureD3D11->SetPrivateData(CustomDataGUID_Generic, sizeof(desc), &desc);
                }

                return reinterpret_cast<HTexture>(textureD3D11);
            }

        case TextureType_3D:
            {
                D3D11_TEXTURE3D_DESC descD3D;
                descD3D.Width          = desc.width;
                descD3D.Height         = desc.height;
                descD3D.Depth          = desc.depth;
                descD3D.MipLevels      = desc.mipmapLevels;
                descD3D.Format         = g_D3DTextureFormatsTable[desc.format];
                descD3D.Usage          = usageD3D;
                descD3D.BindFlags      = bindFlagsD3D;
                descD3D.CPUAccessFlags = 0;
                descD3D.MiscFlags      = 0;

                ID3D11Texture3D* textureD3D11 = nullptr;
                if (SUCCEEDED(m_device->CreateTexture3D(&descD3D, nullptr, &textureD3D11)))
                {
                    textureD3D11->SetPrivateData(CustomDataGUID_Generic, sizeof(desc), &desc);
                }

                return reinterpret_cast<HTexture>(textureD3D11);
            }

        default:
            {
                break;
            }
        }

        return 0;
    }

    void GPURenderingDevice_D3D11::DeleteTexture(HTexture hTexture)   
    {
        if (hTexture != 0)
        {
            reinterpret_cast<ID3D11Resource*>(hTexture)->Release();
        }
    }

    void GPURenderingDevice_D3D11::UpdateTexture(HTexture hTexture, int x, int y, int z, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmapLevel, const void* srcData)
    {
        ID3D11Resource* textureD3D11 = reinterpret_cast<ID3D11Resource*>(hTexture);
        if (textureD3D11 != nullptr)
        {
            UINT descSize = sizeof(TextureDesc);
            TextureDesc desc;
            textureD3D11->GetPrivateData(CustomDataGUID_Generic, &descSize, &desc);

            D3D11_BOX box;
            box.left   = x;
            box.right  = x + width;
            box.top    = y;
            box.bottom = y + height;
            box.front  = z;
            box.back   = z + depth;

            m_context->UpdateSubresource(textureD3D11, mipmapLevel + (z * desc.mipmapLevels), &box, srcData, static_cast<UINT>(GetRowPitch(width, desc.format)), static_cast<UINT>(GetImageSizeInBytes(desc.width, desc.height, desc.format)));
        }
    }


    HTextureView GPURenderingDevice_D3D11::CreateTextureView(HTexture hOriginalTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC descD3D;
        descD3D.Format = g_D3DTextureFormatsTable[format];

        switch (type)
        {
        case TextureType_1D:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
                descD3D.Texture1D.MipLevels       = numLevels;
                descD3D.Texture1D.MostDetailedMip = minLevel;

                break;
            }

        case TextureType_1D_Array:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
                descD3D.Texture1DArray.MipLevels       = numLevels;
                descD3D.Texture1DArray.MostDetailedMip = minLevel;
                descD3D.Texture1DArray.ArraySize       = numLayers;
                descD3D.Texture1DArray.FirstArraySlice = minLayer;

                break;
            }

        case TextureType_2D:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                descD3D.Texture2D.MipLevels       = numLevels;
                descD3D.Texture2D.MostDetailedMip = minLevel;

                break;
            }

        case TextureType_2D_Array:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                descD3D.Texture2D.MipLevels            = numLevels;
                descD3D.Texture2D.MostDetailedMip      = minLevel;
                descD3D.Texture2DArray.ArraySize       = numLayers;
                descD3D.Texture2DArray.FirstArraySlice = minLayer;

                break;
            }

        case TextureType_2D_Multisample:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

                break;
            }

        case TextureType_2D_Multisample_Array:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                descD3D.Texture2DMSArray.ArraySize       = numLayers;
                descD3D.Texture2DMSArray.FirstArraySlice = minLayer;

                break;
            }

        case TextureType_3D:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
                descD3D.Texture3D.MipLevels       = numLevels;
                descD3D.Texture3D.MostDetailedMip = minLevel;

                break;
            }

        case TextureType_Cube:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                descD3D.TextureCube.MipLevels       = numLevels;
                descD3D.TextureCube.MostDetailedMip = minLevel;

                break;
            }

        case TextureType_Cube_Array:
            {
                descD3D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
                descD3D.TextureCubeArray.MipLevels        = numLevels;
                descD3D.TextureCubeArray.MostDetailedMip  = minLevel;
                descD3D.TextureCubeArray.NumCubes         = numLayers / 6;
                descD3D.TextureCubeArray.First2DArrayFace = minLayer;

                break;
            }

        default:
            {
                // Invalid format.
                return 0;
            }
        }
        

        ID3D11ShaderResourceView* viewD3D11;
        if (SUCCEEDED(m_device->CreateShaderResourceView(reinterpret_cast<ID3D11Resource*>(hOriginalTexture), &descD3D, &viewD3D11)))
        {
            return reinterpret_cast<HTextureView>(viewD3D11);
        }
        else
        {
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteTextureView(HTextureView hTextureView)
    {
        if (hTextureView != 0)
        {
            reinterpret_cast<ID3D11View*>(hTextureView)->Release();
        }
    }


    ///////////////////////////////////////////
    // Samplers

    HSampler GPURenderingDevice_D3D11::CreateSampler(const SamplerDesc &desc)
    {
        D3D11_FILTER filtersD3D[] =
        {
            D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
            D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
            D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
            D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
            D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
            D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_FILTER_ANISOTROPIC
        };

        D3D11_TEXTURE_ADDRESS_MODE addressModesD3D[] =
        {
            D3D11_TEXTURE_ADDRESS_WRAP,
            D3D11_TEXTURE_ADDRESS_MIRROR,
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_TEXTURE_ADDRESS_BORDER,
            D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
        };

        D3D11_SAMPLER_DESC descD3D;
        ZeroMemory(&descD3D, sizeof(descD3D));
        descD3D.Filter         = filtersD3D[desc.filter];
        descD3D.AddressU       = addressModesD3D[desc.addressModeU];
        descD3D.AddressV       = addressModesD3D[desc.addressModeV];
        descD3D.AddressW       = addressModesD3D[desc.addressModeW];
        descD3D.MipLODBias     = desc.mipLODBias;
        descD3D.MaxAnisotropy  = desc.maxAnisotropy;
        descD3D.BorderColor[0] = desc.borderColor[0];
        descD3D.BorderColor[1] = desc.borderColor[1];
        descD3D.BorderColor[2] = desc.borderColor[2];
        descD3D.BorderColor[3] = desc.borderColor[3];
        descD3D.MinLOD         = desc.minLOD;
        descD3D.MaxLOD         = desc.maxLOD;

        ID3D11SamplerState* samplerD3D11;
        if (SUCCEEDED(m_device->CreateSamplerState(&descD3D, &samplerD3D11)))
        {
            return reinterpret_cast<HSampler>(samplerD3D11);
        }
        else
        {
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::DeleteSampler(HSampler hSampler)
    {
        if (hSampler != 0)
        {
            reinterpret_cast<ID3D11SamplerState*>(hSampler)->Release();
        }
    }



    ///////////////////////////////////////////
    // Framebuffers

    HFramebuffer GPURenderingDevice_D3D11::CreateFramebuffer()
    {
        return reinterpret_cast<HFramebuffer>(new Framebuffer_D3D11());
    }

    void GPURenderingDevice_D3D11::DeleteFramebuffer(HFramebuffer hFramebuffer)
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            for (int i = 0; i < GT_MAX_FRAMEBUFFER_RENDER_TARGETS; ++i)
            {
                if (framebufferD3D->renderTargets[i] != nullptr)
                {
                    framebufferD3D->renderTargets[i]->Release();
                }
            }

            if (framebufferD3D->depthStencilView != nullptr)
            {
                framebufferD3D->depthStencilView->Release();
            }


            delete framebufferD3D;
        }
    }

    void GPURenderingDevice_D3D11::AttachFramebufferRenderTarget(HFramebuffer hFramebuffer, unsigned int attachmentIndex, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer)
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            // Release the previous render target view if one exists.
            if (framebufferD3D->renderTargets[attachmentIndex] != nullptr)
            {
                framebufferD3D->renderTargets[attachmentIndex]->Release();
                framebufferD3D->renderTargets[attachmentIndex] = nullptr;
            }


            ID3D11Resource* textureD3D = reinterpret_cast<ID3D11Resource*>(hTexture);
            if (textureD3D != nullptr)
            {
                UINT textureDescSize = sizeof(TextureDesc);
                TextureDesc textureDesc;
                textureD3D->GetPrivateData(CustomDataGUID_Generic, &textureDescSize, &textureDesc);

                D3D11_RENDER_TARGET_VIEW_DESC descRTV;
                ZeroMemory(&descRTV, sizeof(descRTV));
                descRTV.Format = g_D3DTextureFormatsTable[textureDesc.format];

                switch (textureDesc.type)
                {
                case TextureType_1D:
                    {
                        descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
                        descRTV.Texture1D.MipSlice = mipmapLevel;

                        break;
                    }
                case TextureType_1D_Array:
                    {
                        descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
                        descRTV.Texture1DArray.MipSlice        = mipmapLevel;
                        descRTV.Texture1DArray.FirstArraySlice = arrayLayer;
                        descRTV.Texture1DArray.ArraySize       = 1;

                        break;
                    }

                case TextureType_2D:
                    {
                        descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                        descRTV.Texture2D.MipSlice = mipmapLevel;

                        break;
                    }
                case TextureType_2D_Array:
                    {
                        descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                        descRTV.Texture2DArray.MipSlice        = mipmapLevel;
                        descRTV.Texture2DArray.FirstArraySlice = arrayLayer;
                        descRTV.Texture2DArray.ArraySize       = 1;

                        break;
                    }

                case TextureType_2D_Multisample:
                    {
                        descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

                        break;
                    }
                case TextureType_2D_Multisample_Array:
                    {
                        descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        descRTV.Texture2DMSArray.FirstArraySlice = arrayLayer;
                        descRTV.Texture2DMSArray.ArraySize       = 1;

                        break;
                    }

                case TextureType_3D:
                    {
                        descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
                        descRTV.Texture3D.MipSlice    = mipmapLevel;
                        descRTV.Texture3D.FirstWSlice = arrayLayer;
                        descRTV.Texture3D.WSize       = 1;

                        break;
                    }

                default:
                    {
                        // Texture type not supported.
                        return;
                    }
                }

                m_device->CreateRenderTargetView(textureD3D, &descRTV, &framebufferD3D->renderTargets[attachmentIndex]);
            }

            
            // If the framebuffer is currently bound, the render targets need to be updated immediately.
            if (m_currentFramebuffer == hFramebuffer)
            {
                m_context->OMSetRenderTargets(GT_MAX_FRAMEBUFFER_RENDER_TARGETS, framebufferD3D->renderTargets, framebufferD3D->depthStencilView);
            }
        }
    }

    void GPURenderingDevice_D3D11::AttachFramebufferDepthStencilTarget(HFramebuffer hFramebuffer, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer)
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            // Release the previous render target view if one exists.
            if (framebufferD3D->depthStencilView != nullptr)
            {
                framebufferD3D->depthStencilView->Release();
                framebufferD3D->depthStencilView = nullptr;
            }


            ID3D11Resource* textureD3D = reinterpret_cast<ID3D11Resource*>(hTexture);
            if (textureD3D != nullptr)
            {
                UINT textureDescSize = sizeof(TextureDesc);
                TextureDesc textureDesc;
                textureD3D->GetPrivateData(CustomDataGUID_Generic, &textureDescSize, &textureDesc);

                D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
                ZeroMemory(&descDSV, sizeof(descDSV));
                descDSV.Format = g_D3DTextureFormatsTable[textureDesc.format];

                switch (textureDesc.type)
                {
                case TextureType_1D:
                    {
                        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
                        descDSV.Texture1D.MipSlice = mipmapLevel;

                        break;
                    }
                case TextureType_1D_Array:
                    {
                        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
                        descDSV.Texture1DArray.MipSlice        = mipmapLevel;
                        descDSV.Texture1DArray.FirstArraySlice = arrayLayer;
                        descDSV.Texture1DArray.ArraySize       = 1;

                        break;
                    }

                case TextureType_2D:
                    {
                        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                        descDSV.Texture2D.MipSlice = mipmapLevel;

                        break;
                    }
                case TextureType_2D_Array:
                    {
                        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                        descDSV.Texture2DArray.MipSlice        = mipmapLevel;
                        descDSV.Texture2DArray.FirstArraySlice = arrayLayer;
                        descDSV.Texture2DArray.ArraySize       = 1;

                        break;
                    }

                case TextureType_2D_Multisample:
                    {
                        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

                        break;
                    }
                case TextureType_2D_Multisample_Array:
                    {
                        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                        descDSV.Texture2DMSArray.FirstArraySlice = arrayLayer;
                        descDSV.Texture2DMSArray.ArraySize       = 1;

                        break;
                    }

                default:
                    {
                        // Texture type not supported.
                        return;
                    }
                }


                m_device->CreateDepthStencilView(textureD3D, &descDSV, &framebufferD3D->depthStencilView);
            }


            // If the framebuffer is currently bound, the render targets need to be updated immediately.
            if (m_currentFramebuffer == hFramebuffer)
            {
                m_context->OMSetRenderTargets(GT_MAX_FRAMEBUFFER_RENDER_TARGETS, framebufferD3D->renderTargets, framebufferD3D->depthStencilView);
            }
        }
    }

    void GPURenderingDevice_D3D11::ClearFramebufferColor(HFramebuffer hFramebuffer, unsigned int attachmentIndex, float color[4])
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            m_context->ClearRenderTargetView(framebufferD3D->renderTargets[attachmentIndex], color);
        }
        else
        {
            // Clear the colour buffer of the framebuffer of the current window.
            auto iFramebuffer = m_windowFramebuffers.Find(m_currentHWND);
            if (iFramebuffer != nullptr)
            {
                m_context->ClearRenderTargetView(iFramebuffer->value.renderTargetView, color);
            }
        }
    }

    void GPURenderingDevice_D3D11::ClearFramebufferDepth(HFramebuffer hFramebuffer, float depth)
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            if (framebufferD3D->depthStencilView != nullptr)
            {
                m_context->ClearDepthStencilView(framebufferD3D->depthStencilView, D3D11_CLEAR_DEPTH, depth, 0);
            }
        }
        else
        {
            // Clear the colour buffer of the framebuffer of the current window.
            auto iFramebuffer = m_windowFramebuffers.Find(m_currentHWND);
            if (iFramebuffer != nullptr)
            {
                m_context->ClearDepthStencilView(iFramebuffer->value.depthStencilView, D3D11_CLEAR_DEPTH, depth, 0);
            }
        }
    }

    void GPURenderingDevice_D3D11::ClearFramebufferStencil(HFramebuffer hFramebuffer, uint8_t stencil)
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            if (framebufferD3D->depthStencilView != nullptr)
            {
                m_context->ClearDepthStencilView(framebufferD3D->depthStencilView, D3D11_CLEAR_STENCIL, 0.0f, stencil);
            }
        }
        else
        {
            // Clear the colour buffer of the framebuffer of the current window.
            auto iFramebuffer = m_windowFramebuffers.Find(m_currentHWND);
            if (iFramebuffer != nullptr)
            {
                m_context->ClearDepthStencilView(iFramebuffer->value.depthStencilView, D3D11_CLEAR_STENCIL, 0.0f, stencil);
            }
        }
    }

    void GPURenderingDevice_D3D11::ClearFramebufferDepthStencil(HFramebuffer hFramebuffer, float depth, uint8_t stencil)
    {
        auto framebufferD3D = reinterpret_cast<Framebuffer_D3D11*>(hFramebuffer);
        if (framebufferD3D != nullptr)
        {
            if (framebufferD3D->depthStencilView != nullptr)
            {
                m_context->ClearDepthStencilView(framebufferD3D->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
            }
        }
        else
        {
            // Clear the colour buffer of the framebuffer of the current window.
            auto iFramebuffer = m_windowFramebuffers.Find(m_currentHWND);
            if (iFramebuffer != nullptr)
            {
                m_context->ClearDepthStencilView(iFramebuffer->value.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
            }
        }
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
            IDXGIDevice1* pDXGIDevice;
            hr = m_device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&pDXGIDevice));
            if (FAILED(hr))
            {
                return FailedToRetrieveIDXGIDevice;
            }

            IDXGIAdapter1* pDXGIAdapter;
            hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter1), reinterpret_cast<void**>(&pDXGIAdapter));
            if (FAILED(hr))
            {
                pDXGIDevice->Release();

                return FailedToRetrieveIDXGIAdapter;
            }

            IDXGIFactory1* pDXGIFactory;
            hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory1),reinterpret_cast<void**>(&pDXGIFactory));
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
                    if (m_currentFramebuffer == 0)
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
                if (m_currentFramebuffer == 0)
                {
                    m_context->OMSetRenderTargets(0, nullptr, nullptr);
                }

                m_currentHWND      = NULL;
                m_currentSwapChain = nullptr;
            }
        }

        return 0;
    }

    void GPURenderingDevice_D3D11::ResizeWindowFramebuffer(HWND hWnd)
    {
        auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
        if (iFramebuffer != nullptr)
        {
            auto &framebuffer = iFramebuffer->value;

            // If the window is the current render target, unset it (it will be made current again at the end).
            if (m_currentHWND == hWnd && m_currentFramebuffer == 0)
            {
                m_context->OMSetRenderTargets(0, nullptr, nullptr);
            }
                
            framebuffer.renderTargetView->Release();

            // Resize the swap chain buffers.
            framebuffer.swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

            // Recreate the render target.
            ID3D11Texture2D* pBackBuffer1 = NULL;
            framebuffer.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer1);
            m_device->CreateRenderTargetView(pBackBuffer1, NULL, &framebuffer.renderTargetView);
            pBackBuffer1->Release();


            // Re-create the depth/stencil buffer if applicable.
            if (framebuffer.depthStencil != nullptr)
            {
                framebuffer.depthStencilView->Release();
                framebuffer.depthStencil->Release();

                framebuffer.depthStencilView = nullptr;
                framebuffer.depthStencil     = nullptr;
                    

                RECT windowClientRect;
                GetClientRect(hWnd, &windowClientRect);

                UINT windowWidth  = windowClientRect.right - windowClientRect.left;
                UINT windowHeight = windowClientRect.bottom - windowClientRect.top;



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
                HRESULT hr = m_device->CreateTexture2D(&descDepth, NULL, &framebuffer.depthStencil);
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
                        framebuffer.depthStencil = nullptr;
                    }
                }
            }



            if (m_currentHWND == hWnd && m_currentFramebuffer == 0)
            {
                m_context->OMSetRenderTargets(1, &framebuffer.renderTargetView, framebuffer.depthStencilView);
            }
        }
    }
#endif

#if defined(GT_PLATFORM_WINDOWS)
#endif
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
