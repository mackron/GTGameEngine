// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_OpenGL4.hpp"

#if defined(GT_GE_BUILD_OPENGL4)
#include "RenderingTypes_OpenGL4.hpp"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

#include <GTLib/String.hpp>
#include <GTLib/Math.hpp>
#include <GTLib/IO/cstdio.hpp>

namespace GT
{
    void APIENTRY DebugOutputCallback_OpenGL4(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam)
    {
        (void)length;
        (void)userParam;

        const char* sourceStr;
        const char* typeStr;
        const char* severityStr;

        switch (source)
        {
        case GL_DEBUG_SOURCE_API_ARB:               sourceStr = "OpenGL";           break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:     sourceStr = "Window System";    break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:   sourceStr = "Shader Compiler";  break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:       sourceStr = "Application";      break;
        case GL_DEBUG_SOURCE_OTHER_ARB:             sourceStr = "Other";            break;

        default: sourceStr = "Unknown"; break;
        }

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR_ARB:               typeStr = "Error";                  break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: typeStr = "Deprecated Behaviour";   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  typeStr = "Undefined Behavious";    break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:         typeStr = "Portability";            break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:         typeStr = "Performance";            break;
        case GL_DEBUG_TYPE_OTHER_ARB:               typeStr = "Message";                break;

        default: typeStr = "Unknown"; break;
        }
 
	    switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH_ARB:   severityStr = "High";   break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: severityStr = "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW_ARB:    severityStr = "Low";    break;

        default: severityStr = "Unknown"; break;
        }

        printf(
            "%s\n"
            "    ID:       %u\n"
            "    Source:   %s\n"
            "    Type:     %s\n"
            "    Severity: %s\n\n",
            message, id, sourceStr, typeStr, severityStr);
    }

    inline void CheckContextIsCurrent(OpenGLContext &m_gl, HDC m_currentDC)
    {
#if !defined(GT_GE_OPTIMIZE_FOR_SINGLE_RENDERING_DEVICE) || GT_GE_OPTIMIZE_FOR_SINGLE_RENDERING_DEVICE == 0
        if (m_gl.GetCurrentContext() != m_gl.GetRenderingContext())
        {
            m_gl.MakeCurrent(m_currentDC, m_gl.GetRenderingContext());
        }
#else
        (void)m_gl;
        (void)m_currentDC;
#endif
    }


    GPURenderingDevice_OpenGL4::GPURenderingDevice_OpenGL4(const GPURenderingDeviceInfo &info)
        : GPURenderingDevice_Gen1(info),
          m_referenceCountLock(),
          m_gl(),
#if defined(GT_PLATFORM_WINDOWS)
          m_currentHWND(NULL),
          m_currentDC(NULL),
#endif
          m_supportedShaderTargets(),

          m_stateFlags(0),
          m_currentTopologyGL(GL_TRIANGLES),
          m_vertexBufferSlots(),
          m_invalidVertexBufferSlots(),
          m_currentIndexBuffer(0),
          m_indexBufferFormat(GL_UNSIGNED_INT),
          m_indexBufferFormatSize(4),
          m_indexBufferOffset(0),
          m_currentInputLayout(0)
    {
        m_stateFlags |= StageFlag_IsWindowFramebufferCurrent;       // TODO: Remove this from the constructor once we get the framebuffer system working properly.
    }

    GPURenderingDevice_OpenGL4::~GPURenderingDevice_OpenGL4()
    {
    }


    ResultCode GPURenderingDevice_OpenGL4::Startup()
    {
        if (m_info.identifier_OpenGL == 1 && IsRenderingAPISupported(m_info, RenderingAPI_OpenGL4))
        {
            unsigned int versionMajor = 2;
            unsigned int versionMinor = 1;
            uint32_t flags = 0;
#if _DEBUG
            flags |= OpenGLContext::EnableDebugging;
#endif

            ResultCode result = m_gl.Startup(versionMajor, versionMinor, flags);
            if (Succeeded(result))
            {
                // Make the dummy DC current by default. If we don't do this, any rendering commands that are issued before making a window current will fail. This is important for things
                // like an initialization step or whatnot.
                m_currentHWND = NULL;
                m_currentDC   = m_gl.GetDummyDC();
                m_gl.MakeCurrent(m_currentDC, m_gl.GetRenderingContext());


                // Retrieve the vendor.
                const char* vendor = reinterpret_cast<const char*>(m_gl.GetString(GL_VENDOR));
                if (strstr(vendor, "NVIDIA") != nullptr)
                {
                    m_vendor = GPUVendor_NVIDIA;
                }
                else if (strstr(vendor, "AMD") != nullptr || strstr(vendor, "ATI") != nullptr)
                {
                    m_vendor = GPUVendor_AMD;
                }
                else if (strstr(vendor, "Intel") != nullptr)
                {
                    m_vendor = GPUVendor_Intel;
                }


                m_supportedShaderTargets.PushBack(GPUShaderTarget_GLSL_120_VS);
                m_supportedShaderTargets.PushBack(GPUShaderTarget_GLSL_120_FS);

                // TODO: Check for ARB program support.


                // Enable back face culling by default.
                m_gl.Enable(GL_CULL_FACE);

                // Enable depth-testing by default.
                m_gl.Enable(GL_DEPTH_TEST);

                // Clockwise winding by default.
                m_gl.FrontFace(GL_CW);




                // Debug Output. For now we are just dumping this to stdout, but later on this should be changed to a proper callback system.
                if (m_gl.IsExtensionSupported("GL_ARB_debug_output"))
                {
                    m_gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
                    m_gl.DebugMessageCallbackARB(DebugOutputCallback_OpenGL4, nullptr);


                    // Disable some specific messages.
                    if (m_vendor == GPUVendor_NVIDIA)
                    {
                        GLuint disabledIDs[] =
                        {
                            131185      // ... Buffer object will use VIDEO memory ...
                        };
                        m_gl.DebugMessageControlARB(GL_DEBUG_SOURCE_API_ARB, GL_DEBUG_TYPE_OTHER_ARB, GL_DONT_CARE, sizeof(disabledIDs) / sizeof(GLuint), disabledIDs, GL_FALSE);
                    }
                }
            }

            return result;
        }
        else
        {
            // The device is not the primary adapter or OpenGL 2.1 is not supported.
            return RenderingAPINotSupported;
        }
    }

    void GPURenderingDevice_OpenGL4::Shutdown()
    {
        m_supportedShaderTargets.Clear();

        m_gl.Shutdown();
    }


    RenderingAPI GPURenderingDevice_OpenGL4::GetRenderingAPI() const
    {
        return RenderingAPI_OpenGL4;
    }

    GPUHandedness GPURenderingDevice_OpenGL4::GetHandedness() const
    {
        return GPUHandedness_Left;
    }


    void GPURenderingDevice_OpenGL4::SetSwapInterval(int swapInterval)
    {
        if (m_gl.SwapIntervalEXT != nullptr)
        {
            if (swapInterval < 0 && !m_gl.IsExtensionSupported("WGL_EXT_swap_control_tear"))
            {
                swapInterval = 0;
            }

            m_gl.SwapIntervalEXT(swapInterval);
        }
    }



    ///////////////////////////////////////////
    // Drawing

    void GPURenderingDevice_OpenGL4::ClearColor(float r, float g, float b, float a)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        m_gl.ClearColor(r, g, b, a);
        m_gl.Clear(GL_COLOR_BUFFER_BIT);
    }

    void GPURenderingDevice_OpenGL4::ClearDepthStencil(GPUClearFlag clearFlags, float depth, uint8_t stencil)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        GLbitfield maskGL = 0;
        if ((clearFlags & GPUClearFlag_Depth) != 0)
        {
            maskGL |= GL_DEPTH_BUFFER_BIT;
            m_gl.ClearDepth(depth);
        }
        if ((clearFlags & GPUClearFlag_Stencil) != 0)
        {
            maskGL |= GL_STENCIL_BUFFER_BIT;
            m_gl.ClearStencil(stencil);
        }

        m_gl.Clear(maskGL);
    }

    void GPURenderingDevice_OpenGL4::Draw(unsigned int indexCount, unsigned int startIndexLocation)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        if (m_currentIndexBuffer != 0)
        {
            // Update the vertex attribute pointers if any are invalid.
            while (m_invalidVertexBufferSlots != 0)
            {
                this->UpdateSlotVertexAttributePointers(GTLib::NextBitIndex(m_invalidVertexBufferSlots));
            }

            m_gl.DrawElements(m_currentTopologyGL, indexCount, m_indexBufferFormat, reinterpret_cast<const void*>(m_indexBufferOffset + (startIndexLocation*m_indexBufferFormatSize)));
        }
    }

        

    ///////////////////////////////////////////
    // State

    void GPURenderingDevice_OpenGL4::SetCurrentShaderProgram(HShaderProgram hShaderProgram)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto shaderProgramGL = reinterpret_cast<ShaderProgram_OpenGL4*>(hShaderProgram);
        if (shaderProgramGL != nullptr)
        {
            m_gl.UseProgram(shaderProgramGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgram(0);
        }
    }

    void GPURenderingDevice_OpenGL4::BindTexture(HTextureView hTextureView, unsigned int slotIndex)
    {
    }

    void GPURenderingDevice_OpenGL4::BindSampler(HSampler hSampler, unsigned int slotIndex)
    {
    }



    /////////////////////////////////////////////////////////////////////////////
    //
    // Stages
    //
    /////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////
    // Input-Assembler Stage

    void GPURenderingDevice_OpenGL4::IASetPrimitiveTopology(GPUPrimitiveTopology topology)
    {
        GLenum topologiesGL[] =
        {
            GL_POINTS,                          // GPUPrimitiveTopology_Point
            GL_LINES,                           // GPUPrimitiveTopology_Line
            GL_LINE_STRIP,                      // GPUPrimitiveTopology_LineStrip
            GL_TRIANGLES,                       // GPUPrimitiveTopology_Triangle
            GL_TRIANGLE_STRIP,                  // GPUPrimitiveTopology_TriangleStrip
            GL_LINES_ADJACENCY,                 // GPUPrimitiveTopology_Line_Adjacency (Not supported in OpenGL 2.1)
            GL_LINE_STRIP_ADJACENCY,            // GPUPrimitiveTopology_LineStrip_Adjacency (Not supported in OpenGL 2.1)
            GL_TRIANGLES_ADJACENCY,             // GPUPrimitiveTopology_Triangle_Adjacency (Not supported in OpenGL 2.1)
            GL_TRIANGLE_STRIP_ADJACENCY         // GPUPrimitiveTopology_TriangleStrip_Adjacency (Not supported in OpenGL 2.1)
        };

        m_currentTopologyGL = topologiesGL[topology];
    }

    void GPURenderingDevice_OpenGL4::IASetInputLayout(HInputLayout hInputLayout)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        if (hInputLayout != m_currentInputLayout)
        {
            // Disable the vertex attributes of the previous input layout.
            if (m_currentInputLayout != 0)
            {
                auto prevLayoutGL = reinterpret_cast<InputLayout_OpenGL4*>(m_currentInputLayout);
                if (prevLayoutGL != nullptr)
                {
                    size_t attribCount = prevLayoutGL->GetAttributeCount();
                    for (size_t iAttrib = 0; iAttrib < attribCount; ++iAttrib)
                    {
                        m_gl.DisableVertexAttribArray(prevLayoutGL->GetAttribute(iAttrib).attribLocation);
                    }
                }
            }


            auto newLayoutGL = reinterpret_cast<InputLayout_OpenGL4*>(hInputLayout);
            if (newLayoutGL != nullptr)
            {
                size_t attribCount = newLayoutGL->GetAttributeCount();
                for (size_t iAttrib = 0; iAttrib < attribCount; ++iAttrib)
                {
                    auto &attribGL = newLayoutGL->GetAttribute(iAttrib);

                    assert(attribGL.slotIndex < GT_GE_MAX_VERTEX_BUFFER_SLOTS);
                    {
                        m_invalidVertexBufferSlots |= (1 << attribGL.slotIndex);
                    }
                }
            }


            m_currentInputLayout = hInputLayout;
        }
    }

    void GPURenderingDevice_OpenGL4::IASetVertexBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t stride, size_t offset)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        assert(slotIndex < GT_GE_MAX_VERTEX_BUFFER_SLOTS);

        m_vertexBufferSlots[slotIndex].buffer = hBuffer;
        m_vertexBufferSlots[slotIndex].stride = stride;
        m_vertexBufferSlots[slotIndex].offset = offset;


        // Update the vertex attribute pointers.
        this->UpdateSlotVertexAttributePointers(slotIndex);
    }

    void GPURenderingDevice_OpenGL4::IASetIndexBuffer(HBuffer hBuffer, GPUIndexFormat format, size_t offset)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        if (bufferGL != nullptr)
        {
            assert(bufferGL->GetOpenGLTarget() == GL_ELEMENT_ARRAY_BUFFER);
            {
                m_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferGL->GetOpenGLObject());
                    
                    
                GLenum indexBufferFormats[] =
                {
                    GL_UNSIGNED_BYTE,
                    GL_UNSIGNED_SHORT,
                    GL_UNSIGNED_INT
                };

                size_t indexBufferFormatSizes[] =
                {
                    1,
                    2,
                    4
                };

                m_indexBufferFormat     = indexBufferFormats[format];
                m_indexBufferFormatSize = indexBufferFormatSizes[format];
                m_indexBufferOffset     = offset;
            }
        }
        else
        {
            m_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }


        m_currentIndexBuffer = hBuffer;
    }


    /////////////////////////////////////////////
    // Rasterization Stage

    void GPURenderingDevice_OpenGL4::RSSetState(HRasterizerState hState)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto stateGL = reinterpret_cast<RasterizerState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            // TODO: Profile this and consider storing a local copy of the relevant state and doing an early comparison before sending the OpenGL commands.

            // Fill mode.
            GLenum fillModesGL[] =
            {
                GL_LINE,        // GPUFillMode_Wireframe
                GL_FILL,        // GPUFillMode_Solid
            };
            m_gl.PolygonMode(GL_FRONT_AND_BACK, fillModesGL[stateGL->fillMode]);
                

            // Cull mode.
            if (stateGL->cullMode == GPUCullMode_None)
            {
                m_gl.Disable(GL_CULL_FACE);
            }
            else
            {
                m_gl.Enable(GL_CULL_FACE);
                m_gl.CullFace((stateGL->cullMode == GPUCullMode_Back) ? GL_BACK : GL_FRONT);
            }


            // Polygon winding.
            GLenum windingModesGL[] =
            {
                GL_CCW,
                GL_CW
            };
            m_gl.FrontFace(windingModesGL[stateGL->polygonWinding]);


            // Depth bias.
            if (stateGL->depthBias != 0)
            {
                m_gl.Enable(GL_POLYGON_OFFSET_FILL);
                m_gl.Enable(GL_POLYGON_OFFSET_LINE);
                m_gl.PolygonOffset(1.0f, static_cast<float>(stateGL->depthBias));
            }
            else
            {
                m_gl.Enable(GL_POLYGON_OFFSET_FILL);
                m_gl.Enable(GL_POLYGON_OFFSET_LINE);
            }


            // Dpeth bias clamp is not supported.
            // Sloped scaled depth bias is not supported.


            // Depth clipping.
            if (stateGL->enableDepthClip)
            {
                m_gl.Disable(GL_DEPTH_CLAMP);
            }
            else
            {
                m_gl.Enable(GL_DEPTH_CLAMP);
            }


            // Enable scissor.
            if (stateGL->enableScissor)
            {
                m_gl.Enable(GL_SCISSOR_TEST);
            }
            else
            {
                m_gl.Disable(GL_SCISSOR_TEST);
            }


            // enableMultisampling and enableAntialiasedLine is not supported.
        }
    }

    void GPURenderingDevice_OpenGL4::RSSetViewports(GPUViewport* viewports, size_t viewportCount)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        if (viewports != nullptr && viewportCount > 0)
        {
            m_gl.Viewport(static_cast<GLint>(viewports[0].x), static_cast<GLint>(viewports[0].y), static_cast<GLsizei>(viewports[0].width), static_cast<GLsizei>(viewports[0].height));
            m_gl.DepthRange(viewports[0].depthRangeNear, viewports[0].depthRangeFar);
        }
    }


    /////////////////////////////////////////////
    // Output Merger Stage

    void GPURenderingDevice_OpenGL4::OMSetDepthStencilState(HDepthStencilState hState, unsigned int stencilRef)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto stateGL = reinterpret_cast<DepthStencilState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            // TODO: Profile this and consider storing a local copy of the relevant state and doing an early comparison before sending the OpenGL commands.

            GLenum comparisonFuncs[] =
            {
                GL_NEVER,
                GL_LESS,
                GL_EQUAL,
                GL_LEQUAL,
                GL_GREATER,
                GL_NOTEQUAL,
                GL_GEQUAL,
                GL_ALWAYS
            };

            GLenum stencilOps[] =
            {
                GL_KEEP,
                GL_ZERO,
                GL_REPLACE,
                GL_INCR,
                GL_DECR,
                GL_INVERT,
                GL_INCR_WRAP,
                GL_DECR_WRAP
            };


            // Enable depth test.
            if (stateGL->enableDepthTest)
            {
                m_gl.Enable(GL_DEPTH_TEST);
            }
            else
            {
                m_gl.Disable(GL_DEPTH_TEST);
            }

            // Depth mask.
            m_gl.DepthMask(stateGL->depthWriteMask != GPUDepthWriteMask_Zero);

            // Depth func.
            m_gl.DepthFunc(comparisonFuncs[stateGL->depthFunc]);



            // Enable stencil test.
            if (stateGL->enableStencilTest)
            {
                m_gl.Enable(GL_STENCIL_TEST);
            }
            else
            {
                m_gl.Disable(GL_STENCIL_TEST);
            }

            // Stencil write mask.
            m_gl.StencilMaskSeparate(GL_FRONT_AND_BACK, stateGL->stencilWriteMask);

            // Front face stencil op.
            m_gl.StencilFuncSeparate(GL_FRONT, comparisonFuncs[stateGL->stencilFrontFaceOp.stencilFunc], static_cast<GLint>(stencilRef), stateGL->stencilReadMask);  // <-- TODO: Check this. Is this the correct use of desc.stencilReadMask?
            m_gl.StencilOpSeparate(GL_FRONT, stencilOps[stateGL->stencilFrontFaceOp.stencilFailOp], stencilOps[stateGL->stencilFrontFaceOp.stencilDepthFailOp], stencilOps[stateGL->stencilFrontFaceOp.stencilPassOp]);

            // Back face stencil op.
            m_gl.StencilFuncSeparate(GL_BACK, comparisonFuncs[stateGL->stencilFrontFaceOp.stencilFunc], static_cast<GLint>(stencilRef), stateGL->stencilReadMask);   // <-- TODO: Check this. Is this the correct use of desc.stencilReadMask?
            m_gl.StencilOpSeparate(GL_BACK, stencilOps[stateGL->stencilFrontFaceOp.stencilFailOp], stencilOps[stateGL->stencilFrontFaceOp.stencilDepthFailOp], stencilOps[stateGL->stencilFrontFaceOp.stencilPassOp]);
        }
    }




    ////////////////////////////////////////////
    // State Objects

    HRasterizerState GPURenderingDevice_OpenGL4::CreateRasterizerState(const GPURasterizerStateDesc &desc)
    {
        return reinterpret_cast<HRasterizerState>(new RasterizerState_OpenGL4(desc));
    }

    void GPURenderingDevice_OpenGL4::ReleaseRasterizerState(HRasterizerState hState)
    {
        auto stateGL = reinterpret_cast<RasterizerState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                assert(stateGL->GetReferenceCount() > 0);

                if (stateGL->DecrementReferenceCount() == 0)
                {
                    delete stateGL;
                }
            }
            m_referenceCountLock.Unlock();
        }
    }

    void GPURenderingDevice_OpenGL4::HoldRasterizerState(HRasterizerState hState)
    {
        auto stateGL = reinterpret_cast<RasterizerState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                stateGL->IncrementReferenceCount();
            }
            m_referenceCountLock.Unlock();
        }
    }


    HDepthStencilState GPURenderingDevice_OpenGL4::CreateDepthStencilState(const GPUDepthStencilStateDesc &desc)
    {
        return reinterpret_cast<size_t>(new DepthStencilState_OpenGL4(desc));
    }

    void GPURenderingDevice_OpenGL4::DeleteDepthStencilState(HDepthStencilState hState)
    {
        auto stateGL = reinterpret_cast<DepthStencilState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                assert(stateGL->GetReferenceCount() > 0);

                if (stateGL->DecrementReferenceCount() == 0)
                {
                    delete stateGL;
                }
            }
            m_referenceCountLock.Unlock();
        }
    }

    void GPURenderingDevice_OpenGL4::HoldDepthStencilState(HDepthStencilState hState)
    {
        auto stateGL = reinterpret_cast<DepthStencilState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                stateGL->IncrementReferenceCount();
            }
            m_referenceCountLock.Unlock();
        }
    }



    ////////////////////////////////////////////
    // Input Layouts

    HInputLayout GPURenderingDevice_OpenGL4::CreateInputLayout(HShaderProgram hShaderProgram, const GPUInputLayoutAttribDesc* attribDesc, size_t attribDescCount)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto shaderProgramGL = reinterpret_cast<ShaderProgram_OpenGL4*>(hShaderProgram);
        if (shaderProgramGL != nullptr)
        {
            auto attribDescGL = reinterpret_cast<InputLayout_OpenGL4::AttributeDesc*>(malloc(sizeof(InputLayout_OpenGL4::AttributeDesc) * attribDescCount));
            if (attribDescGL != nullptr)
            {
                unsigned int slotAttribCounts[GT_GE_MAX_VERTEX_BUFFER_SLOTS];
                memset(slotAttribCounts, 0, sizeof(slotAttribCounts));

                unsigned int iAttribGL = 0;
                for (unsigned int iSlot = 0; iSlot < GT_GE_MAX_VERTEX_BUFFER_SLOTS && iAttribGL < attribDescCount; ++iSlot)
                {
                    for (size_t iAttrib = 0; iAttrib < attribDescCount; ++iAttrib)
                    {
                        assert(iAttribGL < attribDescCount);

                        auto &attrib   = attribDesc[iAttrib];
                        auto &attribGL = attribDescGL[iAttribGL];
                        
                        if (attrib.slotIndex == iSlot)
                        {
                            slotAttribCounts[iSlot] += 1;


                            attribGL.slotIndex            = attrib.slotIndex;
                            attribGL.attribLocation       = m_gl.GetAttribLocation(shaderProgramGL->GetOpenGLObject(), attrib.attributeName);
                            attribGL.attribComponentCount = attrib.attributeComponentCount;
                            attribGL.attribOffset         = attrib.attributeOffset;

                            GLenum attribComponentTypesGL[] =
                            {
                                GL_FLOAT,               // GPUBasicType_Float
                                GL_INT,                 // GPUBasicType_SInt
                                GL_UNSIGNED_INT         // GPUBasicType_UInt
                            };
                            attribGL.attribComponentType = attribComponentTypesGL[attrib.attributeComponentType];

                            attribGL.attributeClass   = attrib.attributeClass;
                            attribGL.instanceStepRate = attrib.instanceStepRate;


                            iAttribGL += 1;
                        }
                    }
                }



                HInputLayout hInputLayout = reinterpret_cast<HInputLayout>(new InputLayout_OpenGL4(attribDescGL, attribDescCount, slotAttribCounts));


                free(attribDescGL);
                return hInputLayout;   // No errors.
            }
            else
            {
                // Failed to allocate memory.
                return 0;
            }
        }
        else
        {
            // Invalid shader program.
            return 0;
        }
    }

    void GPURenderingDevice_OpenGL4::ReleaseInputLayout(HInputLayout hInputLayout)
    {
        auto inputLayoutGL = reinterpret_cast<InputLayout_OpenGL4*>(hInputLayout);
        if (inputLayoutGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                assert(inputLayoutGL->GetReferenceCount() > 0);

                if (inputLayoutGL->DecrementReferenceCount() == 0)
                {
                    if (m_currentInputLayout == hInputLayout)
                    {
                        this->IASetInputLayout(0);
                    }

                    delete inputLayoutGL;
                }
            }
            m_referenceCountLock.Unlock();
        }
    }

    void GPURenderingDevice_OpenGL4::HoldInputLayout(HInputLayout hInputLayout)
    {
        auto inputLayoutGL = reinterpret_cast<InputLayout_OpenGL4*>(hInputLayout);
        if (inputLayoutGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                inputLayoutGL->IncrementReferenceCount();
            }
            m_referenceCountLock.Unlock();
        }
    }


    ////////////////////////////////////////////
    // Shaders

    ResultCode GPURenderingDevice_OpenGL4::CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        if (this->IsShaderTargetSupported(target))
        {
            if (target >= GPUShaderTarget_GLSL_120_VS && target <= GPUShaderTarget_GLSL_120_FS)
            {
                return this->CompileShader_GLSL(source, sourceLength, defines, target, byteCodeOut, messagesOut);
            }

            if (target >= GPUShaderTarget_ARB_VP && target <= GPUShaderTarget_ARB_FP)
            {
                return this->CompileShader_ARB(source, sourceLength, defines, target, byteCodeOut, messagesOut);
            }
        }

        return ShaderTargetNotSupported;
    }

    bool GPURenderingDevice_OpenGL4::IsShaderTargetSupported(GPUShaderTarget target) const
    {
        return m_supportedShaderTargets.Exists(target);
    }

    HShaderProgram GPURenderingDevice_OpenGL4::CreateShaderProgram(const void* vertexShaderData, size_t vertexShaderDataSize, const void* fragmentShaderData, size_t fragmentShaderDataSize, GT::BasicBuffer &messagesOut)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        // TODO: Check that the shader targets are of the correct shader stage.


        // Vertex Shader.
        const char* vertexSource;
        size_t vertexSourceLength;
        GTLib::Vector<GPUShaderDefine> vertexDefines;
        GPUShaderTarget vertexTarget;
        if (GT::Failed(this->ExtractShaderBinaryData(vertexShaderData, vertexShaderDataSize, vertexSource, vertexSourceLength, vertexDefines, vertexTarget)))
        {
            return 0;
        }

        // Fragment Shader.
        const char* fragmentSource;
        size_t fragmentSourceLength;
        GTLib::Vector<GPUShaderDefine> fragmentDefines;
        GPUShaderTarget fragmentTarget;
        if (GT::Failed(this->ExtractShaderBinaryData(fragmentShaderData, fragmentShaderDataSize, fragmentSource, fragmentSourceLength, fragmentDefines, fragmentTarget)))
        {
            return 0;
        }



        // The shader types must be compatible.
        if (vertexTarget == GPUShaderTarget_GLSL_120_VS && fragmentTarget != GPUShaderTarget_GLSL_120_FS)
        {
            // Shader targets are not compatible.
            return 0;
        }



        // Vertex object.
        GLuint vertexObjectGL = 0;
        if (this->IsShaderTargetSupported(vertexTarget))
        {
            if (vertexTarget == GPUShaderTarget_GLSL_120_VS)
            {
                if (GT::Failed(this->CompileShader_GLSL(vertexSource, vertexSourceLength, vertexDefines.buffer, vertexTarget, messagesOut, vertexObjectGL)))
                {
                    // Failed to compile vertex shader.
                    return 0;
                }
            }

            if (vertexTarget == GPUShaderTarget_ARB_VP)
            {
                // TODO: Implement this!
                return 0;
            }
        }
        else
        {
            // Shader target is not supported.
            return 0;
        }


        // Fragment object.
        GLuint fragmentObjectGL = 0;
        if (this->IsShaderTargetSupported(fragmentTarget))
        {
            if (fragmentTarget == GPUShaderTarget_GLSL_120_FS)
            {
                if (GT::Failed(this->CompileShader_GLSL(fragmentSource, fragmentSourceLength, fragmentDefines.buffer, fragmentTarget, messagesOut, fragmentObjectGL)))
                {
                    return 0;
                }
            }

            if (fragmentTarget == GPUShaderTarget_ARB_FP)
            {
                // TODO: Implement this!
                return 0;
            }
        }
        else
        {
            // Shader target is not supported.
            return 0;
        }



        GLuint objectGL = m_gl.CreateProgram();
        if (objectGL > 0)
        {
            // Attach the shader objects.
            m_gl.AttachShader(objectGL, vertexObjectGL);
            m_gl.AttachShader(objectGL, fragmentObjectGL);

            // Perform the link step.
            m_gl.LinkProgram(objectGL);


            // Always check for log messages regardless of whether or not there was an error.
            GLint logLengthInBytes;
            m_gl.GetProgramiv(objectGL, GL_INFO_LOG_LENGTH, &logLengthInBytes);
            if (logLengthInBytes > 0)
            {
                void* messageDst = messagesOut.Allocate(logLengthInBytes, true);
                if (messageDst != nullptr)
                {
                    m_gl.GetProgramInfoLog(objectGL, logLengthInBytes, &logLengthInBytes, reinterpret_cast<GLchar*>(messageDst));
                }
            }


            ShaderProgram_OpenGL4* shaderProgramGL = nullptr;

            // Check for link errors.
            GLint isLinked = 0;
            m_gl.GetProgramiv(objectGL, GL_LINK_STATUS, &isLinked);
            if (isLinked == GL_TRUE)
            {
                shaderProgramGL = new ShaderProgram_OpenGL4(objectGL);
            }


            m_gl.DeleteShader(vertexObjectGL);
            m_gl.DeleteShader(fragmentObjectGL);


            return reinterpret_cast<HShaderProgram>(shaderProgramGL);
        }
        else
        {
            // Failed to create OpenGL program object.
            return 0;
        }



        return 0;
    }

    void GPURenderingDevice_OpenGL4::ReleaseShaderProgram(HShaderProgram hShaderProgram)
    {
        auto shaderProgramGL = reinterpret_cast<ShaderProgram_OpenGL4*>(hShaderProgram);
        if (shaderProgramGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                assert(shaderProgramGL->GetReferenceCount() > 0);

                CheckContextIsCurrent(m_gl, m_currentDC);

                if (shaderProgramGL->DecrementReferenceCount() == 0)
                {
                    m_gl.DeleteProgram(shaderProgramGL->GetOpenGLObject());
                    delete shaderProgramGL;
                }
            }
            m_referenceCountLock.Unlock();
        }
    }

    void GPURenderingDevice_OpenGL4::HoldShaderProgram(HShaderProgram hShaderProgram)
    {
        auto shaderProgramGL = reinterpret_cast<ShaderProgram_OpenGL4*>(hShaderProgram);
        if (shaderProgramGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                shaderProgramGL->IncrementReferenceCount();
            }
            m_referenceCountLock.Unlock();
        }
    }


    ///////////////////////////////////////////
    // Buffers

    HBuffer GPURenderingDevice_OpenGL4::CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data)
    {
        (void)cpuAccessFlags;   // <-- Not considered with OpenGL.


        CheckContextIsCurrent(m_gl, m_currentDC);

        if (usage == GPUBufferUsage_Immutable && data == nullptr)
        {
            // No data specified for immutable buffer. Immutable buffers must have their data set a creation time.
            return 0;
        }

        if (type == GPUBufferType_Constant)
        {
            // Don't currently support constant/uniform buffers.
            return 0;
        }


        GLuint targetsGL[] =
        {
            GL_ARRAY_BUFFER,
            GL_ELEMENT_ARRAY_BUFFER,
            GL_UNIFORM_BUFFER
        };

        GLuint usagesGL[] =
        {
            GL_STREAM_DRAW,     // BufferUsage_Default
            GL_STATIC_DRAW,     // BufferUsage_Immutable,
            GL_DYNAMIC_DRAW,    // BufferUsage_Dynamic,
            GL_DYNAMIC_READ,    // BufferUsage_Staging
        };


        GLuint prevObjectGL;
        m_gl.GetIntegerv((targetsGL[type] == GL_ARRAY_BUFFER) ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&prevObjectGL));


        GLuint objectGL;
        m_gl.GenBuffers(1, &objectGL);
        m_gl.BindBuffer(targetsGL[type], objectGL);
        m_gl.BufferData(targetsGL[type], sizeInBytes, data, usagesGL[usage]);

        HBuffer hBuffer = reinterpret_cast<HBuffer>(new Buffer_OpenGL4(objectGL, targetsGL[type], usagesGL[usage]));

        // Re-bind the previous buffer.
        m_gl.BindBuffer(targetsGL[type], prevObjectGL);


        return hBuffer;
    }

    void GPURenderingDevice_OpenGL4::ReleaseBuffer(HBuffer hBuffer)
    {
        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        if (bufferGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                assert(bufferGL->GetReferenceCount() > 0);

                CheckContextIsCurrent(m_gl, m_currentDC);

                if (bufferGL->DecrementReferenceCount() == 0)
                {
                    GLuint objectGL = bufferGL->GetOpenGLObject();
                    m_gl.DeleteBuffers(1, &objectGL);


                    // If the vertex buffer is bound, unbind it.
                    switch (bufferGL->GetOpenGLTarget())
                    {
                    case GL_ARRAY_BUFFER:
                        {
                            for (unsigned int iVertexBufferSlot = 0; iVertexBufferSlot < GT_GE_MAX_VERTEX_BUFFER_SLOTS; ++iVertexBufferSlot)
                            {
                                if (m_vertexBufferSlots[iVertexBufferSlot].buffer == hBuffer)
                                {
                                    m_vertexBufferSlots[iVertexBufferSlot].buffer = 0;
                                }
                            }

                            break;
                        }

                    case GL_ELEMENT_ARRAY_BUFFER:
                        {
                            if (m_currentIndexBuffer == hBuffer)
                            {
                                m_currentIndexBuffer = 0;
                            }

                            break;
                        }

                    case GL_UNIFORM_BUFFER:
                        {
                            break;
                        }

                    default: break;
                    }

                    delete bufferGL;
                }
            }
            m_referenceCountLock.Unlock();
        }
    }

    void GPURenderingDevice_OpenGL4::HoldBuffer(HBuffer hBuffer)
    {
        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        if (bufferGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                bufferGL->IncrementReferenceCount();
            }
            m_referenceCountLock.Unlock();
        }
    }

    void* GPURenderingDevice_OpenGL4::MapBuffer(HBuffer hBuffer, GPUBufferMapType mapType)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        assert(bufferGL != nullptr);
        {
            if (bufferGL->GetOpenGLUsage() != GL_STATIC_DRAW)   // <-- BufferUsage_Immutable
            {
                const GLenum accessGLTable[] =
                {
                    GL_READ_ONLY,       // GPUBufferMapType_Read
                    GL_WRITE_ONLY,      // GPUBufferMapType_Write
                    GL_READ_WRITE,      // GPUBufferMapType_ReadWrite
                    GL_WRITE_ONLY,      // GPUBufferMapType_Write_Discard
                    GL_WRITE_ONLY       // GPUBufferMapType_Write_NoOverwrite
                };


                GLuint prevObjectGL;
                m_gl.GetIntegerv((bufferGL->GetOpenGLTarget() == GL_ARRAY_BUFFER) ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&prevObjectGL));


                m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), bufferGL->GetOpenGLObject());
                void* dataOut = m_gl.MapBuffer(bufferGL->GetOpenGLTarget(), accessGLTable[mapType]);


                // Re-bind the previous buffer.
                m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), prevObjectGL);


                return dataOut;
            }
            else
            {
                // Buffer is immutable.
                return nullptr;
            }
        }
    }

    void GPURenderingDevice_OpenGL4::UnmapBuffer(HBuffer hBuffer)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        assert(bufferGL != nullptr);
        {
            GLuint prevObjectGL;
            m_gl.GetIntegerv((bufferGL->GetOpenGLTarget() == GL_ARRAY_BUFFER) ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&prevObjectGL));

            m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), bufferGL->GetOpenGLObject());
            m_gl.UnmapBuffer(bufferGL->GetOpenGLTarget());

            // Re-bind the previous buffer.
            m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), prevObjectGL);
        }
    }

    void GPURenderingDevice_OpenGL4::SetBufferData(HBuffer hBuffer, size_t offsetInBytes, size_t sizeInBytes, const void* data)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        assert(bufferGL != nullptr);
        {
            if (bufferGL->GetOpenGLUsage() != GL_STATIC_DRAW)   // <-- BufferUsage_Immutable
            {
                GLuint prevObjectGL;
                m_gl.GetIntegerv((bufferGL->GetOpenGLTarget() == GL_ARRAY_BUFFER) ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&prevObjectGL));

                m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), bufferGL->GetOpenGLObject());
                m_gl.BufferSubData(bufferGL->GetOpenGLTarget(), offsetInBytes, sizeInBytes, data);

                // Re-bind the previous buffer.
                m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), prevObjectGL);
            }
        }
    }



    ///////////////////////////////////////////
    // Textures

    HTexture2D GPURenderingDevice_OpenGL4::CreateTexture2D(const Texture2DDesc &desc)
    {
        return 0;
    }

    void GPURenderingDevice_OpenGL4::ReleaseTexture2D(HTexture2D hTexture)
    {
    }

    void GPURenderingDevice_OpenGL4::HoldTexture2D(HTexture2D hTexture)
    {
    }

    void GPURenderingDevice_OpenGL4::UpdateTexture2D(HTexture2D hTexture, int x, int y, unsigned int width, unsigned int height, unsigned int level, unsigned int layer, const void* srcData, unsigned int srcDataRowPitch)
    {
    }


    HTextureView GPURenderingDevice_OpenGL4::CreateTextureViewFrom1D(HTexture1D hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers)
    {
        return 0;
    }

    HTextureView GPURenderingDevice_OpenGL4::CreateTextureViewFrom2D(HTexture2D hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers)
    {
        return 0;
    }

    HTextureView GPURenderingDevice_OpenGL4::CreateTextureViewFrom2DMultisample(HTexture2DMultisample hTexture, TextureType type, TextureFormat format, unsigned int minLayer, unsigned int numLayers)
    {
        return 0;
    }

    HTextureView GPURenderingDevice_OpenGL4::CreateTextureViewFrom3D(HTexture3D hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels)
    {
        return 0;
    }

    HTextureView GPURenderingDevice_OpenGL4::CreateTextureViewFromCube(HTextureCube hTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers)
    {
        return 0;
    }

    void GPURenderingDevice_OpenGL4::ReleaseTextureView(HTextureView hTextureView)
    {
    }

    void GPURenderingDevice_OpenGL4::HoldTextureView(HTextureView hTextureView)
    {
    }



    ///////////////////////////////////////////
    // Samplers

    HSampler GPURenderingDevice_OpenGL4::CreateSampler(const SamplerDesc &desc)
    {
        return 0;
    }

    void GPURenderingDevice_OpenGL4::ReleaseSampler(HSampler hSampler)
    {
    }

    void GPURenderingDevice_OpenGL4::HoldSampler(HSampler hSampler)
    {
    }




    ///////////////////////////////////////////
    // Framebuffers


    //////////////////////////////////////////
    // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
    void GPURenderingDevice_OpenGL4::SwapBuffers()
    {
        if (m_currentDC != NULL)
        {
            ::SwapBuffers(m_currentDC);
        }
    }

    ResultCode GPURenderingDevice_OpenGL4::InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil)
    {
        (void)includeDepthStencil;      // <-- This is always included with OpenGL contexts because all windows must use the same pixel format.


        if (!m_windowFramebuffers.Exists(hWnd))
        {
            HDC hDC = GetDC(hWnd);

            if (SetPixelFormat(hDC, m_gl.GetPixelFormat(), &m_gl.GetPFD()))
            {
                HWNDFramebuffer framebuffer;
                framebuffer.m_hDC = hDC;

                m_windowFramebuffers.Add(hWnd, framebuffer);

                return 0;   // No error.
            }
            else
            {
                // Failed to set pixel format.
                return OpenGLContext::FailedToSetPixelFormat;
            }
        }
        else
        {
            // Window is already registered. This is not considered an error.
            return 0;
        }
    }

    void GPURenderingDevice_OpenGL4::UninitWindowFramebuffer(HWND hWnd)
    {
        auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
        if (iFramebuffer != nullptr)
        {
            m_windowFramebuffers.RemoveByIndex(iFramebuffer->index);
        }
    }

    ResultCode GPURenderingDevice_OpenGL4::SetCurrentWindow(HWND hWnd)
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
                }

                m_gl.MakeCurrent(framebuffer.m_hDC, m_gl.GetRenderingContext());

                m_currentHWND = hWnd;
                m_currentDC   = framebuffer.m_hDC;
            }
            else
            {
                // The window can not be used as a render target because InitWindowFramebuffer() has not been successfully called on it.
                return InvalidWindowRenderTarget;
            }
        }
        else
        {
            // No window was specified.
            if ((m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
            {
                        
            }

            m_gl.MakeCurrent(m_gl.GetDummyDC(), NULL);

            m_currentHWND = NULL;
            m_currentDC   = m_gl.GetDummyDC();
        }

        return 0;   // No error.
    }

    void GPURenderingDevice_OpenGL4::ResizeWindowFramebuffer(HWND hWnd)
    {
        (void)hWnd;
            
        // This is done automatically in the background.
    }
#endif

#if defined(GT_PLATFORM_LINUX)
#endif



    //////////////////////////////////////////
    // Private

    void GPURenderingDevice_OpenGL4::UpdateSlotVertexAttributePointers(unsigned int slotIndex)
    {
        assert(slotIndex < GT_GE_MAX_VERTEX_BUFFER_SLOTS);


        auto inputLayoutGL = reinterpret_cast<InputLayout_OpenGL4*>(m_currentInputLayout);
        if (inputLayoutGL != nullptr)
        {
            auto &slot = m_vertexBufferSlots[slotIndex];

            // We need to get the indices of the attributes that are tied to the buffer on the given slot.
            size_t attribStart;
            size_t attribEnd;
            inputLayoutGL->GetSlotAttributeRange(slotIndex, attribStart, attribEnd);


            auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(slot.buffer);
            if (bufferGL != nullptr)
            {
                for (size_t iAttrib = attribStart; iAttrib < attribEnd; ++iAttrib)
                {
                    auto &attribGL = inputLayoutGL->GetAttribute(iAttrib);
                    if (attribGL.attribLocation > -1)
                    {
                        m_gl.EnableVertexAttribArray(attribGL.attribLocation);
                        m_gl.BindBuffer(GL_ARRAY_BUFFER, bufferGL->GetOpenGLObject());
                        m_gl.VertexAttribPointer(attribGL.attribLocation, attribGL.attribComponentCount, attribGL.attribComponentType, GL_FALSE, static_cast<GLsizei>(slot.stride), reinterpret_cast<const void*>(slot.offset + attribGL.attribOffset));

                        // NOTE: With OpenGL 4.5 we would check to see if this was per-instance data.
                    }
                }
            }
            else
            {
                // The input buffer is null. We need to disable every vertex attribute location that is bound to the buffer on this slot.
                for (size_t iAttrib = attribStart; iAttrib < attribEnd; ++iAttrib)
                {
                    auto &attribGL = inputLayoutGL->GetAttribute(iAttrib);
                    if (attribGL.attribLocation > -1)
                    {
                        m_gl.DisableVertexAttribArray(attribGL.attribLocation);
                    }
                }
            }


            m_invalidVertexBufferSlots &= ~(1 << slotIndex);
        }
    }

    ResultCode GPURenderingDevice_OpenGL4::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
    {
        GLuint objectGL;
        ResultCode result = this->CompileShader_GLSL(source, sourceLength, defines, target, messagesOut, objectGL);
        if (GT::Succeeded(result))
        {
            // The shader compilation was successful. We now need to build the byte code data. The OpenGL 2.1 API does not support loading shaders from binary data, so we can only output
            // the original shader source and the defines and target that was used to build it.
            result = this->CreateShaderBinaryData(source, sourceLength, defines, target, nullptr, 0, 0, byteCodeOut);


            // The shader object needs to be deleted at this pointer or otherwise it will leak.
            m_gl.DeleteShader(objectGL);
        }

        return result;
    }

    ResultCode GPURenderingDevice_OpenGL4::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &messagesOut, GLuint &objectGLOut)
    {
        assert(target == GPUShaderTarget_GLSL_120_VS || target == GPUShaderTarget_GLSL_120_FS);


        objectGLOut = 0;

        const char* versionStrings[] =
        {
            "",                     // GPUShaderTarget_Unknown
            "#version 120\n",       // GPUShaderTarget_GLSL_120_VS
            "#version 120\n"        // GPUShaderTarget_GLSL_120_FS
        };

        GLenum shaderTypes[] =
        {
            0,
            GL_VERTEX_SHADER,       // GPUShaderTarget_GLSL_120_VS,
            GL_FRAGMENT_SHADER,     // GPUShaderTarget_GLSL_120_FS
        };


        GTLib::String definesString;
        if (defines != nullptr)
        {
            int i = 0;
            while (defines[i].name != nullptr)
            {
                definesString.AppendFormatted("#define %s %s\n", defines[i].name, defines[i].value);
            }
        }
            


        const char* shaderStrings[3];
        shaderStrings[0] = versionStrings[target];
        shaderStrings[1] = definesString.c_str();
        shaderStrings[2] = source;

        GLint shaderStringLengths[3];
        shaderStringLengths[0] = -1;        // Null-terminated.
        shaderStringLengths[1] = -1;        // Null-terminated.
        shaderStringLengths[2] = static_cast<GLint>((sourceLength > 0) ? sourceLength : -1);


        GLuint objectGL = m_gl.CreateShader(shaderTypes[target]);
        if (objectGL != 0)
        {
            // Compile the shader.
            m_gl.ShaderSource(objectGL, 3, shaderStrings, nullptr);
            m_gl.CompileShader(objectGL);


            // Check for errors and/or warnings. We always want to output all messages, even when the compilation is successful.
            GLint logLengthInBytes;
            m_gl.GetShaderiv(objectGL, GL_INFO_LOG_LENGTH, &logLengthInBytes);
            if (logLengthInBytes > 0)
            {
                void* messageDst = messagesOut.Allocate(logLengthInBytes, true);
                if (messageDst != nullptr)
                {
                    m_gl.GetShaderInfoLog(objectGL, logLengthInBytes, &logLengthInBytes, reinterpret_cast<GLchar*>(messageDst));
                }
            }


            // Now check for compilation errors.
            GLint compiled;
            m_gl.GetShaderiv(objectGL, GL_COMPILE_STATUS, &compiled);
            if (compiled == GL_TRUE)
            {
                objectGLOut = objectGL;
                return 0;   // No error.
            }
            else
            {
                return FailedToCompileShader;
            }
        }
        else
        {
            return FailedToCreateOpenGLShaderObject;
        }
    }

    ResultCode GPURenderingDevice_OpenGL4::CompileShader_ARB(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
    {
        (void)source;
        (void)sourceLength;
        (void)defines;
        (void)target;
        (void)byteCodeOut;
        (void)messagesOut;

        return ShaderTargetNotSupported;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
