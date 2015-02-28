// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)
#include "GPUBuffer_D3D11.hpp"
#include "GPUVertexShader_D3D11.hpp"
#include "GPUFragmentShader_D3D11.hpp"
#include "GPUShaderProgram_D3D11.hpp"
#include <GTLib/Strings/Find.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Parse.hpp>

namespace GT
{
    // GUID for use with SetPrivateData().
    const GUID GPURenderingDevice_D3D11::CustomDataGUID = {0xD74A88E1, 0xDA90, 0x05AA, {0xDD, 0x01, 0xEA, 0xD3, 0x31, 0xB3, 0xC2, 0x10}};

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
                    UINT flags = 0;
#ifdef _DEBUG
                    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

                    D3D_FEATURE_LEVEL    featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
                    D3D_FEATURE_LEVEL    featureLevel    = D3D_FEATURE_LEVEL_11_0;
                    HRESULT hr = _D3D11CreateDevice(reinterpret_cast<IDXGIAdapter1*>(m_info.identifier_D3D11), D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, featureLevels, sizeof(featureLevels) / sizeof(featureLevels[0]), D3D11_SDK_VERSION, &m_device, &featureLevel, &m_context);
                    if (SUCCEEDED(hr) && featureLevel == D3D_FEATURE_LEVEL_11_0)
                    {
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

    GPUHandedness GPURenderingDevice_D3D11::GetHandedness() const
    {
        return GPUHandedness_Left;
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

    void GPURenderingDevice_D3D11::ClearDepthStencil(GPUClearFlag clearFlags, float depth, uint8_t stencil)
    {
        auto iFramebuffer = m_windowFramebuffers.Find(m_currentHWND);
        if (iFramebuffer != nullptr)
        {
            UINT clearFlagsD3D = 0;
            if ((clearFlags & GPUClearFlag_Depth) != 0)
            {
                clearFlagsD3D |= D3D11_CLEAR_DEPTH;
            }
            if ((clearFlags & GPUClearFlag_Stencil) != 0)
            {
                clearFlagsD3D |= D3D11_CLEAR_STENCIL;
            }

            m_context->ClearDepthStencilView(iFramebuffer->value.depthStencilView, clearFlagsD3D, depth, stencil);
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
        // All we do is set the vertex and fragment shaders.
        auto shaderProgramD3D = reinterpret_cast<GPUShaderProgram_D3D11*>(shaderProgram);
        if (shaderProgramD3D != nullptr)
        {
            assert(shaderProgramD3D->GetVertexShader()   != nullptr);
            assert(shaderProgramD3D->GetFragmentShader() != nullptr);

            m_context->VSSetShader(shaderProgramD3D->GetVertexShader()->GetD3D11VertexShader(),  nullptr, 0);
            m_context->PSSetShader(shaderProgramD3D->GetFragmentShader()->GetD3D11PixelShader(), nullptr, 0);
        }
        else
        {
            m_context->VSSetShader(nullptr, nullptr, 0);
            m_context->PSSetShader(nullptr, nullptr, 0);
        }
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

    void GPURenderingDevice_D3D11::IASetInputLayout(HInputLayout hInputLayout)
    {
        m_context->IASetInputLayout(reinterpret_cast<ID3D11InputLayout*>(hInputLayout));
    }

    void GPURenderingDevice_D3D11::IASetVertexBuffer(unsigned int slotIndex, GPUBuffer* buffer, size_t stride, size_t offset)
    {
        auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
        if (bufferD3D != nullptr)
        {
            auto bufferD3D11 = bufferD3D->GetD3D11Buffer();
            UINT strideD3D11 = static_cast<UINT>(stride);
            UINT offsetD3D11 = static_cast<UINT>(offset);
            m_context->IASetVertexBuffers(slotIndex, 1, &bufferD3D11, &strideD3D11, &offsetD3D11);
        }
        else
        {
            m_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
        }
    }

    void GPURenderingDevice_D3D11::IASetIndexBuffer(GPUBuffer* buffer, GPUIndexFormat format, size_t offset)
    {
        DXGI_FORMAT formats[] =
        {
            DXGI_FORMAT_R8_UINT,
            DXGI_FORMAT_R16_UINT,
            DXGI_FORMAT_R32_UINT
        };

        auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
        if (bufferD3D != nullptr)
        {
            m_context->IASetIndexBuffer(bufferD3D->GetD3D11Buffer(), formats[format], static_cast<UINT>(offset));
        }
        else
        {
            m_context->IASetIndexBuffer(nullptr, formats[format], static_cast<UINT>(offset));
        }
    }


    /////////////////////////////////////////////
    // Rasterization Stage

    void GPURenderingDevice_D3D11::RSSetState(HRasterizerState hState)
    {
        m_context->RSSetState(reinterpret_cast<ID3D11RasterizerState*>(hState));
    }

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
                free(viewportsD3D11);
            }
        }
    }


    /////////////////////////////////////////////
    // Output Merger Stage

    void GPURenderingDevice_D3D11::OMSetDepthStencilState(HDepthStencilState hState, unsigned int stencilRef)
    {
        m_context->OMSetDepthStencilState(reinterpret_cast<ID3D11DepthStencilState*>(hState), stencilRef);
    }




    /////////////////////////////////////////////////////////////////////////////
    //
    // Object Creation and Deletion
    //
    /////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////
    // State Objects

    HRasterizerState GPURenderingDevice_D3D11::CreateRasterizerState(const GPURasterizerStateDesc &desc)
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
        descD3D.FrontCounterClockwise = (desc.polygonWinding == GPUPolygonWinding_CW) ? TRUE : FALSE;
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

    void GPURenderingDevice_D3D11::ReleaseRasterizerState(HRasterizerState hState)
    {
        if (hState != 0)
        {
            reinterpret_cast<ID3D11RasterizerState*>(hState)->Release();
        }
    }

    void GPURenderingDevice_D3D11::HoldRasterizerState(HRasterizerState hState)
    {
        if (hState != 0)
        {
            reinterpret_cast<ID3D11RasterizerState*>(hState)->AddRef();
        }
    }


    HDepthStencilState GPURenderingDevice_D3D11::CreateDepthStencilState(const GPUDepthStencilStateDesc &desc)
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
            return reinterpret_cast<size_t>(depthStencilStateD3D11);
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

    void GPURenderingDevice_D3D11::HoldDepthStencilState(HDepthStencilState hState)
    {
        if (hState != 0)
        {
            reinterpret_cast<ID3D11DepthStencilState*>(hState)->AddRef();
        }
    }


    ////////////////////////////////////////////
    // Input Layout

    HInputLayout GPURenderingDevice_D3D11::CreateInputLayout(GPUShaderProgram* shaderProgram, const GPUInputLayoutAttribDesc* attribDesc, size_t attribDescCount)
    {
        auto shaderProgramD3D = reinterpret_cast<GPUShaderProgram_D3D11*>(shaderProgram);
        if (shaderProgramD3D != nullptr)
        {
            assert(shaderProgramD3D->GetVertexShader() != nullptr);

            auto vertexShaderData       = shaderProgramD3D->GetVertexShader()->GetD3D11ShaderBinary();
            size_t vertexShaderDataSize = shaderProgramD3D->GetVertexShader()->GetD3D11ShaderBinarySize();

            if (vertexShaderData != nullptr && vertexShaderDataSize > 0)
            {
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


                        DXGI_FORMAT format[3][4] = 
                        {
                            {
                                DXGI_FORMAT_R32_FLOAT,
                                DXGI_FORMAT_R32G32_FLOAT,
                                DXGI_FORMAT_R32G32B32_FLOAT,
                                DXGI_FORMAT_R32G32B32A32_FLOAT
                            },  // GPUBasicType_Float
                            {
                                DXGI_FORMAT_R32_SINT,
                                DXGI_FORMAT_R32G32_SINT,
                                DXGI_FORMAT_R32G32B32_SINT,
                                DXGI_FORMAT_R32G32B32A32_SINT
                            },  // GPUBasicType_SInt
                            {
                                DXGI_FORMAT_R32_UINT,
                                DXGI_FORMAT_R32G32_UINT,
                                DXGI_FORMAT_R32G32B32_UINT,
                                DXGI_FORMAT_R32G32B32A32_UINT
                            }   // GPUBasicType_UInt
                        };


                        attribDescD3D[iAttrib].SemanticName         = semanticName;
                        attribDescD3D[iAttrib].SemanticIndex        = GTLib::Parse<UINT>(semanticIndexStart);
                        attribDescD3D[iAttrib].Format               = format[attribDesc[iAttrib].attributeComponentType][attribDesc[iAttrib].attributeComponentCount - 1];
                        attribDescD3D[iAttrib].InputSlot            = attribDesc[iAttrib].slotIndex;
                        attribDescD3D[iAttrib].AlignedByteOffset    = attribDesc[iAttrib].attributeOffset;
                        attribDescD3D[iAttrib].InputSlotClass       = (attribDesc[iAttrib].attributeClass == GPUInputClassification_PerVertex) ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
                        attribDescD3D[iAttrib].InstanceDataStepRate = attribDesc[iAttrib].instanceStepRate;
                    }


                    ID3D11InputLayout* inputLayoutD3D;
                    if (GT::Succeeded(result))
                    {
                        if (FAILED(m_device->CreateInputLayout(attribDescD3D, static_cast<UINT>(attribDescCount), vertexShaderData, vertexShaderDataSize, &inputLayoutD3D)))
                        {
                            inputLayoutD3D = nullptr;
                        }
                    }
                        


                    for (size_t iAttrib = 0; iAttrib < attribDescCount; ++iAttrib)
                    {
                        free(const_cast<void*>(reinterpret_cast<const void*>(attribDescD3D[iAttrib].SemanticName)));
                    }

                    delete [] attribDescD3D;
                    return reinterpret_cast<HInputLayout>(inputLayoutD3D);
                }
            }
            else
            {
                // Invalid vertex shader data.
                return 0;
            }
        }
        else
        {
            // No shader program specified.
            return 0;
        }
    }

    void GPURenderingDevice_D3D11::ReleaseInputLayout(HInputLayout hInputLayout)
    {
        if (hInputLayout != 0)
        {
            reinterpret_cast<ID3D11InputLayout*>(hInputLayout)->Release();
        }
    }

    void GPURenderingDevice_D3D11::HoldInputLayout(HInputLayout hInputLayout)
    {
        if (hInputLayout != 0)
        {
            reinterpret_cast<ID3D11InputLayout*>(hInputLayout)->AddRef();
        }
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


        D3D11_USAGE usages[] =
        {
            D3D11_USAGE_DEFAULT,
            D3D11_USAGE_IMMUTABLE,
            D3D11_USAGE_DYNAMIC,
            D3D11_USAGE_STAGING,
        };

        UINT bindFlags[] =
        {
            0,
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

        if ((cpuAccessFlags & GPUBufferCPUAccess_Read) != 0)
        {
            bd.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
        }
        if ((cpuAccessFlags & GPUBufferCPUAccess_Write) != 0)
        {
            bd.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
        }


        D3D11_SUBRESOURCE_DATA initialData;
	    ZeroMemory(&initialData, sizeof(D3D11_SUBRESOURCE_DATA));
        initialData.pSysMem = data;


        ID3D11Buffer* bufferD3D11;
        if (SUCCEEDED(m_device->CreateBuffer(&bd, &initialData, &bufferD3D11)))
        {
            bufferOut = new GPUBuffer_D3D11(bufferD3D11, type, usage, cpuAccessFlags);
            return 0;
        }
        else
        {
            // Failed to create the buffer.
            return -1;
        }
    }

    void GPURenderingDevice_D3D11::DeleteBuffer(GPUBuffer* buffer)
    {
        auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
        assert(bufferD3D != nullptr);
        {
            assert(bufferD3D->GetD3D11Buffer() != nullptr);

            bufferD3D->GetD3D11Buffer()->Release();
            delete buffer;
        }
    }

    ResultCode GPURenderingDevice_D3D11::MapBuffer(GPUBuffer* buffer, GPUBufferMapType mapType, void* &dataOut)
    {
        auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
        assert(bufferD3D != nullptr);
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
            if (SUCCEEDED(m_context->Map(bufferD3D->GetD3D11Buffer(), 0, mapTypes[mapType - 1], 0, &mappingInfo)))
            {
                dataOut = mappingInfo.pData;
                return 0;
            }
            else
            {
                // Failed to map the buffer.
                return -1;
            }
        }
    }

    void GPURenderingDevice_D3D11::UnmapBuffer(GPUBuffer* buffer)
    {
        auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
        assert(bufferD3D != nullptr);
        {
            m_context->Unmap(bufferD3D->GetD3D11Buffer(), 0);
        }
    }

    ResultCode GPURenderingDevice_D3D11::SetBufferData(GPUBuffer* buffer, size_t offsetInBytes, size_t sizeInBytes, const void* data)
    {
        auto bufferD3D = reinterpret_cast<GPUBuffer_D3D11*>(buffer);
        assert(bufferD3D != nullptr);
        {
            if (bufferD3D->GetBufferUsage() != GPUBufferUsage_Immutable)
            {
                D3D11_BOX box;
                box.left   = static_cast<UINT>(offsetInBytes);
                box.right  = static_cast<UINT>(offsetInBytes + sizeInBytes);
                box.top    = 0;
                box.bottom = 1;
                box.front  = 0;
                box.back   = 1;

                m_context->UpdateSubresource(bufferD3D->GetD3D11Buffer(), 0, &box, data, static_cast<UINT>(sizeInBytes), 0);

                return 0;
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

    void GPURenderingDevice_D3D11::ResizeWindowFramebuffer(HWND hWnd)
    {
        auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
        if (iFramebuffer != nullptr)
        {
            auto &framebuffer = iFramebuffer->value;

            // If the window is the current render target, unset it (it will be made current again at the end).
            if (m_currentHWND == hWnd && (m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
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



            if (m_currentHWND == hWnd && (m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
            {
                m_context->OMSetRenderTargets(1, &framebuffer.renderTargetView, framebuffer.depthStencilView);
            }
        }
    }
#endif

#if defined(GT_PLATFORM_WINDOWS)
#endif
}

#endif
