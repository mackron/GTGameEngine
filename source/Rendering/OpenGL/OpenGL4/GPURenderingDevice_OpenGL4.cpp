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
#if _DEBUG
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
#endif

    inline void CheckContextIsCurrent(OpenGL4Context &m_gl, HDC m_currentDC)
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
        : GPURenderingDevice(info),
          m_referenceCountLock(),
          m_gl(),
#if defined(GT_PLATFORM_WINDOWS)
          m_currentHWND(NULL),
          m_currentDC(NULL),
#endif
          m_vendor(GPUVendor_Unknown),
          m_supportedShaderLanguages(),

          m_stateFlags(0),
          m_currentTopologyGL(GL_TRIANGLES),
          m_globalVAO(0),
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
            uint32_t flags = 0;
#if _DEBUG
            flags |= OpenGL4Context::EnableDebugging;
#endif

            ResultCode result = m_gl.Startup(flags);
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


                // The supported languages depend on the GLSL version.
                unsigned int majorVersionGLSL;
                unsigned int minorVersionGLSL;
                m_gl.GetGLSLVersion(majorVersionGLSL, minorVersionGLSL);

                assert(majorVersionGLSL >= 4);
                m_supportedShaderLanguages.PushBack(ShaderLanguage_GLSL_400);

                if (minorVersionGLSL > 0)
                {
                    m_supportedShaderLanguages.PushBack(ShaderLanguage_GLSL_410);
                }
                if (minorVersionGLSL > 1)
                {
                    m_supportedShaderLanguages.PushBack(ShaderLanguage_GLSL_420);
                }
                if (minorVersionGLSL > 2)
                {
                    m_supportedShaderLanguages.PushBack(ShaderLanguage_GLSL_430);
                }
                if (minorVersionGLSL > 3)
                {
                    m_supportedShaderLanguages.PushBack(ShaderLanguage_GLSL_440);
                }
                if (minorVersionGLSL > 4)
                {
                    m_supportedShaderLanguages.PushBack(ShaderLanguage_GLSL_450);
                }



                // Create and bind the global VAO object.
                m_gl.CreateVertexArrays(1, &m_globalVAO);
                m_gl.BindVertexArray(m_globalVAO);

                // Create a bind the global shader program pipeline.
                m_gl.CreateProgramPipelines(1, &m_globalShaderPipeline);
                m_gl.BindProgramPipeline(m_globalShaderPipeline);



                // Enable back face culling by default.
                m_gl.Enable(GL_CULL_FACE);

                // Enable depth-testing by default.
                m_gl.Enable(GL_DEPTH_TEST);

                // Clockwise winding by default.
                m_gl.FrontFace(GL_CW);



#if _DEBUG
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
#endif
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
        m_supportedShaderLanguages.Clear();

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
            m_gl.UseProgramStages(m_globalVAO, GL_VERTEX_SHADER_BIT,   reinterpret_cast<Shader_OpenGL4*>(shaderProgramGL->hVertexShader  )->GetOpenGLObject());
            m_gl.UseProgramStages(m_globalVAO, GL_FRAGMENT_SHADER_BIT, reinterpret_cast<Shader_OpenGL4*>(shaderProgramGL->hFragmentShader)->GetOpenGLObject());
            //m_gl.UseProgram(shaderProgramGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgramStages(m_globalVAO, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, 0);
            //m_gl.UseProgram(0);
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
                        m_gl.DisableVertexArrayAttrib(m_globalVAO, prevLayoutGL->GetAttribute(iAttrib).attribLocation);
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

        GLuint bufferGL = 0;
        if (hBuffer != 0)
        {
            bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer)->GetOpenGLObject();
        }

        m_gl.VertexArrayVertexBuffer(m_globalVAO, slotIndex, bufferGL, static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
    }

    void GPURenderingDevice_OpenGL4::IASetIndexBuffer(HBuffer hBuffer, GPUIndexFormat format, size_t offset)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        if (bufferGL != nullptr)
        {
            m_gl.VertexArrayElementBuffer(m_globalVAO, bufferGL->GetOpenGLObject());
        }
        else
        {
            m_gl.VertexArrayElementBuffer(m_globalVAO, 0);
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
                            attribGL.attribLocation       = m_gl.GetAttribLocation(reinterpret_cast<Shader_OpenGL4*>(shaderProgramGL->hVertexShader)->GetOpenGLObject(), attrib.attributeName);
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

    ResultCode GPURenderingDevice_OpenGL4::CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        if (this->IsShaderLanguageSupported(language))
        {
            if (language >= ShaderLanguage_GLSL_400 && language <= ShaderLanguage_GLSL_450)
            {
                return this->CompileShader_GLSL(source, sourceLength, defines, language, type, byteCodeOut, messagesOut);
            }

            if (language >= ShaderLanguage_GL_NV_5 && language <= ShaderLanguage_GL_NV_5)
            {
                // TODO: Add support for NV5 assembly shaders.
            }
        }

        return ShaderTargetNotSupported;
    }

    bool GPURenderingDevice_OpenGL4::IsShaderLanguageSupported(ShaderLanguage language) const
    {
        return m_supportedShaderLanguages.Exists(language);
    }

    HShaderProgram GPURenderingDevice_OpenGL4::CreateShaderProgram(const void* vertexShaderData, size_t vertexShaderDataSize, const void* fragmentShaderData, size_t fragmentShaderDataSize, GT::BasicBuffer &messagesOut)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        (void)messagesOut;


        HVertexShader vertexShader = this->CreateVertexShader(vertexShaderData, vertexShaderDataSize);
        if (vertexShader != 0)
        {
            HFragmentShader fragmentShader = this->CreateFragmentShader(fragmentShaderData, fragmentShaderDataSize);
            if (fragmentShader != 0)
            {
                return reinterpret_cast<HShaderProgram>(new ShaderProgram_OpenGL4(vertexShader, fragmentShader));
            }
            else
            {
                this->ReleaseVertexShader(vertexShader);
            }
        }

        return 0;

#if 0
        // Vertex Shader.
        const char* vertexSource;
        size_t vertexSourceLength;
        GTLib::Vector<GPUShaderDefine> vertexDefines;
        ShaderLanguage vertexLanguage;
        ShaderType vertexType;
        if (GT::Failed(this->ExtractShaderBinaryData(vertexShaderData, vertexShaderDataSize, vertexSource, vertexSourceLength, vertexDefines, vertexLanguage, vertexType)))
        {
            return 0;
        }

        if (vertexType != ShaderType_Vertex)
        {
            return 0;
        }


        // Fragment Shader.
        const char* fragmentSource;
        size_t fragmentSourceLength;
        GTLib::Vector<GPUShaderDefine> fragmentDefines;
        ShaderLanguage fragmentLanguage;
        ShaderType fragmentType;
        if (GT::Failed(this->ExtractShaderBinaryData(fragmentShaderData, fragmentShaderDataSize, fragmentSource, fragmentSourceLength, fragmentDefines, fragmentLanguage, fragmentType)))
        {
            return 0;
        }

        if (fragmentType != ShaderType_Fragment)
        {
            return 0;
        }



        // Vertex object.
        GLuint vertexObjectGL = 0;
        if (this->IsShaderLanguageSupported(vertexLanguage))
        {
            if (vertexLanguage >= ShaderLanguage_GLSL_400 && vertexLanguage <= ShaderLanguage_GLSL_450)
            {
                if (GT::Failed(this->CompileShader_GLSL(vertexSource, vertexSourceLength, vertexDefines.buffer, vertexLanguage, vertexType, messagesOut, vertexObjectGL)))
                {
                    // Failed to compile vertex shader.
                    return 0;
                }
            }
        }
        else
        {
            // Shader target is not supported.
            return 0;
        }


        // Fragment object.
        GLuint fragmentObjectGL = 0;
        if (this->IsShaderLanguageSupported(fragmentLanguage))
        {
            if (fragmentLanguage >= ShaderLanguage_GLSL_400 && fragmentLanguage <= ShaderLanguage_GLSL_450)
            {
                if (GT::Failed(this->CompileShader_GLSL(fragmentSource, fragmentSourceLength, fragmentDefines.buffer, fragmentLanguage, fragmentType, messagesOut, fragmentObjectGL)))
                {
                    return 0;
                }
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
#endif
    }

    void GPURenderingDevice_OpenGL4::ReleaseShaderProgram(HShaderProgram hShaderProgram)
    {
        auto shaderProgramGL = reinterpret_cast<ShaderProgram_OpenGL4*>(hShaderProgram);
        if (shaderProgramGL != nullptr)
        {
            assert(shaderProgramGL->hVertexShader   != 0);
            assert(shaderProgramGL->hFragmentShader != 0);

            this->ReleaseShader(shaderProgramGL->hVertexShader);
            this->ReleaseShader(shaderProgramGL->hFragmentShader);

            assert(reinterpret_cast<Shader_OpenGL4*>(shaderProgramGL->hVertexShader)->GetReferenceCount() == reinterpret_cast<Shader_OpenGL4*>(shaderProgramGL->hFragmentShader)->GetReferenceCount());

            if (reinterpret_cast<Shader_OpenGL4*>(shaderProgramGL->hVertexShader)->GetReferenceCount() == 0)
            {
                delete shaderProgramGL;
            }
        }

#if 0
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
#endif
    }

    void GPURenderingDevice_OpenGL4::HoldShaderProgram(HShaderProgram hShaderProgram)
    {
        
    }


    HVertexShader GPURenderingDevice_OpenGL4::CreateVertexShader(const void* shaderData, size_t shaderDataSize)
    {
        return this->CreateShader(shaderData, shaderDataSize, ShaderType_Vertex);
    }

    void GPURenderingDevice_OpenGL4::ReleaseVertexShader(HVertexShader hShader)
    {
        this->ReleaseShader(hShader);
    }

    void GPURenderingDevice_OpenGL4::HoldVertexShader(HVertexShader hShader)
    {
        this->HoldShader(hShader);
    }


    HFragmentShader GPURenderingDevice_OpenGL4::CreateFragmentShader(const void* shaderData, size_t shaderDataSize)
    {
        return this->CreateShader(shaderData, shaderDataSize, ShaderType_Fragment);
    }

    void GPURenderingDevice_OpenGL4::ReleaseFragmentShader(HFragmentShader hShader)
    {
        this->ReleaseShader(hShader);
    }

    void GPURenderingDevice_OpenGL4::HoldFragmentShader(HFragmentShader hShader)
    {
        this->HoldShader(hShader);
    }


    ///////////////////////////////////////////
    // Buffers

    HBuffer GPURenderingDevice_OpenGL4::CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data)
    {
        // Validation.
        if (usage == GPUBufferUsage_Immutable && data == nullptr)
        {
            if (data == nullptr)
            {
                // No data specified for immutable buffer. Immutable buffers must have their data set a creation time.
                return 0;
            }

            if (cpuAccessFlags != GPUBufferCPUAccess_None)
            {
                // Cannot access an immutable buffer from the CPU.
                return 0;
            }
        }

        if (usage == GPUBufferUsage_Dynamic)
        {
            if ((cpuAccessFlags & GPUBufferCPUAccess_Read) != 0)
            {
                // Specified a dynamic buffer and a read CPU access flag.

                // TODO: Output an error message saying that there is a conflict with the buffer usage and the CPU access flags.
                return 0;
            }

            if ((cpuAccessFlags & GPUBufferCPUAccess_Write) == 0)
            {
                // Specified a dynamic buffer, but have not specified a CPU access flag. Recommend using an immutable or default buffer usage.

                // TODO: Output a warning message saying that an immutable buffer is recommended.
            }
        }


        CheckContextIsCurrent(m_gl, m_currentDC);

        GLbitfield flagsGL = 0;
        switch (usage)
        {
        case GPUBufferUsage_Immutable:
            {
                break;
            }

        case GPUBufferUsage_Dynamic:
            {
                flagsGL |= GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT;
                break;
            }

        case GPUBufferUsage_Staging:
            {
                flagsGL |= GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
                break;
            }

        case GPUBufferUsage_Default:
        default:
            {
                flagsGL |= GL_DYNAMIC_STORAGE_BIT;
                break;
            }
        }


        GLuint targetsGL[] =
        {
            GL_ARRAY_BUFFER,
            GL_ELEMENT_ARRAY_BUFFER,
            GL_UNIFORM_BUFFER
        };

        
        GLuint objectGL;
        m_gl.CreateBuffers(1, &objectGL);
        m_gl.NamedBufferStorage(objectGL, static_cast<GLsizeiptr>(sizeInBytes), data, flagsGL);

        return reinterpret_cast<HBuffer>(new Buffer_OpenGL4(objectGL, flagsGL, sizeInBytes));
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
            GLbitfield flagsGL[] =
            {
                GL_MAP_READ_BIT,                                     // GPUBufferMapType_Read
                GL_MAP_WRITE_BIT ,                                   // GPUBufferMapType_Write
                GL_MAP_READ_BIT  | GL_MAP_WRITE_BIT,                 // GPUBufferMapType_ReadWrite
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT,      // GPUBufferMapType_Write_Discard
                GL_MAP_WRITE_BIT                                     // GPUBufferMapType_Write_NoOverwrite
            };

            return m_gl.MapNamedBufferRange(bufferGL->GetOpenGLObject(), 0, bufferGL->GetSizeInBytes(), flagsGL[mapType]);
        }
    }

    void GPURenderingDevice_OpenGL4::UnmapBuffer(HBuffer hBuffer)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        assert(bufferGL != nullptr);
        {
            m_gl.UnmapNamedBuffer(bufferGL->GetOpenGLObject());
        }
    }

    void GPURenderingDevice_OpenGL4::SetBufferData(HBuffer hBuffer, size_t offsetInBytes, size_t sizeInBytes, const void* data)
    {
        // TODO: Prevent this call for Dynamic usage buffers.

        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        assert(bufferGL != nullptr);
        {
            m_gl.NamedBufferSubData(bufferGL->GetOpenGLObject(), static_cast<GLintptr>(offsetInBytes), static_cast<GLintptr>(sizeInBytes), data);
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
                return OpenGL4Context::FailedToSetPixelFormat;
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


    // TODO: Rename this function.
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
                        m_gl.EnableVertexArrayAttrib(m_globalVAO, attribGL.attribLocation);
                        m_gl.VertexArrayAttribBinding(m_globalVAO, attribGL.attribLocation, attribGL.slotIndex);

                        // TODO: Use the I and L versions of glVertexArrayAttribFormat where applicable based on the component type.
                        m_gl.VertexArrayAttribFormat(m_globalVAO, attribGL.attribLocation, attribGL.attribComponentCount, attribGL.attribComponentType, GL_FALSE, attribGL.attribOffset);

                        // Check if we are looking at per-instance format.
                        //
                        // TODO: This is untested. Check this.
                        if (attribGL.attributeClass == GPUInputClassification_PerInstance)
                        {
                            m_gl.VertexArrayBindingDivisor(m_globalVAO, attribGL.slotIndex, attribGL.instanceStepRate);
                        }
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
                        m_gl.DisableVertexArrayAttrib(m_globalVAO, attribGL.attribLocation);
                    }
                }
            }


            m_invalidVertexBufferSlots &= ~(1 << slotIndex);
        }
    }


    HGeneric GPURenderingDevice_OpenGL4::CreateShader(const void* shaderBinary, size_t shaderBinarySizeInBytes, ShaderType type)
    {
        const char* shaderSource;
        size_t shaderSourceLength;
        GTLib::Vector<GPUShaderDefine> defines;
        ShaderLanguage language;
        ShaderType actualType;
        if (GT::Succeeded(this->ExtractShaderBinaryData(shaderBinary, shaderBinarySizeInBytes, shaderSource, shaderSourceLength, defines, language, actualType)))
        {
            if (actualType == type)
            {
                GT::BasicBuffer devnull;

                GLuint objectGL;
                GLenum typeGL;
                if (GT::Succeeded(this->CompileShader_GLSL(shaderSource, shaderSourceLength, defines.buffer, language, type, devnull, objectGL, typeGL)))
                {
                    return reinterpret_cast<HVertexShader>(new Shader_OpenGL4(objectGL, typeGL));
                }
                else
                {
                    // Failed to compile shader.
                    return 0;
                }
            }
            else
            {
                // Not a vertex shader.
                return 0;
            }
        }
        else
        {
            // Failed to extract binary data.
            return 0;
        }
    }

    void GPURenderingDevice_OpenGL4::ReleaseShader(HGeneric hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                assert(shaderGL->GetReferenceCount() > 0);

                CheckContextIsCurrent(m_gl, m_currentDC);

                if (shaderGL->DecrementReferenceCount() == 0)
                {
                    m_gl.DeleteProgram(shaderGL->GetOpenGLObject());
                    delete shaderGL;
                }
            }
            m_referenceCountLock.Unlock();
        }
    }

    void GPURenderingDevice_OpenGL4::HoldShader(HGeneric hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_referenceCountLock.Lock();
            {
                shaderGL->IncrementReferenceCount();
            }
            m_referenceCountLock.Unlock();
        }
    }


    ResultCode GPURenderingDevice_OpenGL4::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
    {
        GLuint objectGL;
        GLenum typeGL;
        ResultCode result = this->CompileShader_GLSL(source, sourceLength, defines, language, type, messagesOut, objectGL, typeGL);
        if (GT::Succeeded(result))
        {
            // The shader compilation was successful. We now need to build the byte code data. The OpenGL 2.1 API does not support loading shaders from binary data, so we can only output
            // the original shader source and the defines and target that was used to build it.
            result = this->CreateShaderBinaryData(source, sourceLength, defines, language, type, nullptr, 0, 0, byteCodeOut);


            // The shader object needs to be deleted at this pointer or otherwise it will leak.
            m_gl.DeleteProgram(objectGL);
        }

        return result;
    }

    ResultCode GPURenderingDevice_OpenGL4::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &messagesOut, GLuint &objectGLOut, GLenum &typeGLOut)
    {
        assert(language >= ShaderLanguage_GLSL_400 && language <= ShaderLanguage_GLSL_450);


        objectGLOut = 0;

        const char* versionStrings[] =
        {
            "",                     // ShaderLanguage_Unknown
            "#version 400\n",       // ShaderLanguage_GLSL_400
            "#version 410\n",       // ShaderLanguage_GLSL_410
            "#version 420\n",       // ShaderLanguage_GLSL_420
            "#version 430\n",       // ShaderLanguage_GLSL_430
            "#version 440\n",       // ShaderLanguage_GLSL_440
            "#version 450\n"        // ShaderLanguage_GLSL_450
        };

        GLenum shaderTypes[] =
        {
            GL_VERTEX_SHADER,           // ShaderType_Vertex,
            GL_TESS_EVALUATION_SHADER,  // ShaderType_TessellationEvaluation
            GL_TESS_CONTROL_SHADER,     // ShaderType_TessellationControl
            GL_GEOMETRY_SHADER,         // ShaderType_Geometry
            GL_FRAGMENT_SHADER,         // ShaderType_Fragment
            GL_COMPUTE_SHADER,          // ShaderType_Compute
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
        

        const char* vertexOutStruct = "";
        if (type == ShaderType_Vertex)
        {
            vertexOutStruct =
                "out gl_PerVertex\n"
                "{\n"
                "    vec4  gl_Position;\n"
                "    float gl_PointSize;\n"
                "    float gl_ClipDistance[];\n"
                "};\n";
        }


        const char* shaderStrings[4];
        shaderStrings[0] = versionStrings[language];
        shaderStrings[1] = definesString.c_str();
        shaderStrings[2] = vertexOutStruct;
        shaderStrings[3] = source;

        GLint shaderStringLengths[4];
        shaderStringLengths[0] = -1;        // Null-terminated.
        shaderStringLengths[1] = -1;        // Null-terminated.
        shaderStringLengths[2] = -1;
        shaderStringLengths[3] = static_cast<GLint>((sourceLength > 0) ? sourceLength : -1);

        GLuint objectGL = m_gl.CreateShaderProgramv(shaderTypes[type], sizeof(shaderStrings) / sizeof(shaderStrings[0]), shaderStrings);


        // Always check the log, even if compilation was successful - it might contain warnings.
        GLint logLengthInBytes;
        m_gl.GetProgramiv(objectGL, GL_INFO_LOG_LENGTH, &logLengthInBytes);
        if (logLengthInBytes > 1)
        {
            void* messageDst = messagesOut.Allocate(logLengthInBytes, true);
            if (messageDst != nullptr)
            {
                m_gl.GetProgramInfoLog(objectGL, logLengthInBytes, &logLengthInBytes, reinterpret_cast<GLchar*>(messageDst));
            }
        }


        if (objectGL != 0)
        {
            objectGLOut = objectGL;
            typeGLOut   = shaderTypes[type];

            return 0;
        }
        else
        {
            return FailedToCompileShader;
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
