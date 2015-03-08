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
#include <GTLib/ImageUtils.hpp>

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
          m_gl(),
#if defined(GT_PLATFORM_WINDOWS)
          m_currentHWND(NULL),
          m_currentDC(NULL),
#endif
          m_vendor(GPUVendor_Unknown),
          m_supportedShaderLanguages(),
          m_supportFlags(0),

          m_stateFlags(0),
          m_currentTopologyGL(GL_TRIANGLES),
          m_globalVAO(0),
          m_vertexBufferSlots(),
          m_invalidVertexBufferSlots(),
          m_currentIndexBuffer(0),
          m_indexBufferFormatGL(GL_UNSIGNED_INT),
          m_indexBufferFormatSize(4),
          m_indexBufferOffset(0),
          m_currentInputLayout(0),
          m_currentFramebuffer(0),
          m_currentViewports(),
          m_currentScissorRects(),
          m_currentWindowHeight(0)
    {
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


                if (minorVersionGLSL >= 5 || m_gl.IsExtensionSupported("GL_ARB_multi_bind"))
                {
                    m_supportFlags |= SupportFlag_MultiBind;
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

                // Emulate Direct3D.
                m_gl.ClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);



                // Retrieve some initial state.
                GLint viewportDims[4];
                m_gl.GetIntegerv(GL_VIEWPORT, viewportDims);

                for (int i = 0; i < GT_MAX_VIEWPORTS; ++i)
                {
                    m_currentViewports[i].x      = static_cast<float>(viewportDims[0]);
                    m_currentViewports[i].y      = static_cast<float>(viewportDims[1]);
                    m_currentViewports[i].width  = static_cast<float>(static_cast<unsigned int>(viewportDims[2]));
                    m_currentViewports[i].height = static_cast<float>(static_cast<unsigned int>(viewportDims[3]));
                }


                GLint scissorDims[4];
                m_gl.GetIntegerv(GL_SCISSOR_BOX, scissorDims);

                for (int i = 0; i < GT_MAX_VIEWPORTS; ++i)
                {
                    m_currentScissorRects[i].x      = scissorDims[0];
                    m_currentScissorRects[i].y      = scissorDims[1];
                    m_currentScissorRects[i].width  = static_cast<unsigned int>(scissorDims[2]);
                    m_currentScissorRects[i].height = static_cast<unsigned int>(scissorDims[3]);
                }


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

    GraphicsHandedness GPURenderingDevice_OpenGL4::GetHandedness() const
    {
        return GraphicsHandedness_Left;
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

    void GPURenderingDevice_OpenGL4::Draw(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        if (m_currentIndexBuffer != 0)
        {
            // Update the vertex attribute pointers if any are invalid.
            while (m_invalidVertexBufferSlots != 0)
            {
                this->UpdateSlotVertexAttributePointers(GTLib::NextBitIndex(m_invalidVertexBufferSlots));
            }

            m_gl.DrawElementsBaseVertex(m_currentTopologyGL, indexCount, m_indexBufferFormatGL, reinterpret_cast<const void*>(m_indexBufferOffset + (startIndexLocation*m_indexBufferFormatSize)), baseVertex);
        }
    }

    void GPURenderingDevice_OpenGL4::DrawInstanced(unsigned int indexCount, unsigned int startIndexLocation, int baseVertex, unsigned int instanceCount, unsigned int baseInstance)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        if (m_currentIndexBuffer != 0)
        {
            // Update the vertex attribute pointers if any are invalid.
            while (m_invalidVertexBufferSlots != 0)
            {
                this->UpdateSlotVertexAttributePointers(GTLib::NextBitIndex(m_invalidVertexBufferSlots));
            }

            m_gl.DrawElementsInstancedBaseVertexBaseInstance(m_currentTopologyGL, indexCount, m_indexBufferFormatGL, reinterpret_cast<const void*>(m_indexBufferOffset + (startIndexLocation*m_indexBufferFormatSize)), instanceCount, baseVertex, baseInstance);
        }
    }

        

    ///////////////////////////////////////////
    // State

    void GPURenderingDevice_OpenGL4::BindTexture(unsigned int slotIndex, HTextureView hTextureView)
    {
        auto textureViewGL = reinterpret_cast<TextureView_OpenGL4*>(hTextureView);
        if (textureViewGL != nullptr)
        {
            m_gl.BindTextureUnit(slotIndex, textureViewGL->GetOpenGLObject());
        }
        else
        {
            m_gl.BindTextureUnit(slotIndex, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::BindSampler(unsigned int slotIndex, HSampler hSampler)
    {
        auto samplerGL = reinterpret_cast<Sampler_OpenGL4*>(hSampler);
        if (samplerGL != nullptr)
        {
            m_gl.BindSampler(slotIndex, samplerGL->GetOpenGLObject());
        }
        else
        {
            m_gl.BindSampler(slotIndex, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::BindConstantBuffer(unsigned int slotIndex, HBuffer hBuffer)
    {
        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        if (bufferGL != nullptr)
        {
            m_gl.BindBufferBase(GL_UNIFORM_BUFFER, slotIndex, bufferGL->GetOpenGLObject());
        }
        else
        {
            m_gl.BindBufferBase(GL_UNIFORM_BUFFER, slotIndex, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::BindConstantBuffer(unsigned int slotIndex, HBuffer hBuffer, size_t offset, size_t size)
    {
        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        if (bufferGL != nullptr)
        {
            m_gl.BindBufferRange(GL_UNIFORM_BUFFER, slotIndex, bufferGL->GetOpenGLObject(), static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
        }
        else
        {
            m_gl.BindBufferRange(GL_UNIFORM_BUFFER, slotIndex, 0, 0, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::BindConstantBuffer(unsigned int slotIndex, size_t count, HBuffer* hBuffers, size_t* offsets, size_t* sizes)
    {
        if (count < GT_MAX_CONSTANT_BUFFER_SLOTS)
        {
            if ((m_supportFlags & SupportFlag_MultiBind) != 0)
            {
                if (hBuffers != nullptr)
                {
                    GLuint buffersGL[GT_MAX_CONSTANT_BUFFER_SLOTS];
                    GLintptr offsetsGL[GT_MAX_CONSTANT_BUFFER_SLOTS];
                    GLsizeiptr sizesGL[GT_MAX_CONSTANT_BUFFER_SLOTS];
                    for (size_t iSlot = 0; iSlot < count; ++iSlot)
                    {
                        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffers[iSlot]);
                        if (bufferGL != nullptr)
                        {
                            buffersGL[iSlot] = bufferGL->GetOpenGLObject();
                        }
                        else
                        {
                            buffersGL[iSlot] = 0;
                        }

                        offsetsGL[iSlot] = static_cast<GLintptr>(offsets[iSlot]);
                        sizesGL[iSlot]   = static_cast<GLsizeiptr>(sizes[iSlot]);
                    }

                    m_gl.BindBuffersRange(GL_UNIFORM_BUFFER, slotIndex, static_cast<GLsizei>(count), buffersGL, offsetsGL, sizesGL);
                }
                else
                {
                    m_gl.BindBuffersRange(GL_UNIFORM_BUFFER, slotIndex, static_cast<GLsizei>(count), nullptr, nullptr, nullptr);
                }
            }
            else
            {
                if (hBuffers != nullptr)
                {
                    for (size_t iSlot = 0; iSlot < count; ++iSlot)
                    {
                        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffers[iSlot]);
                        if (bufferGL != nullptr)
                        {
                            m_gl.BindBufferRange(GL_UNIFORM_BUFFER, slotIndex + static_cast<unsigned int>(iSlot), bufferGL->GetOpenGLObject(), static_cast<GLintptr>(offsets[iSlot]), static_cast<GLsizeiptr>(sizes[iSlot]));
                        }
                        else
                        {
                            m_gl.BindBufferRange(GL_UNIFORM_BUFFER, slotIndex + static_cast<unsigned int>(iSlot), 0, 0, 0);
                        }
                    }
                }
                else
                {
                    for (size_t iSlot = 0; iSlot < count; ++iSlot)
                    {
                        m_gl.BindBufferRange(GL_UNIFORM_BUFFER, slotIndex + static_cast<unsigned int>(iSlot), 0, 0, 0);
                    }
                }
            }
        }
        else
        {
            // Too many buffers were specified.
        }
    }

    void GPURenderingDevice_OpenGL4::BindFramebuffer(HFramebuffer hFramebuffer)
    {
        auto framebufferGL = reinterpret_cast<Framebuffer_OpenGL4*>(hFramebuffer);
        if (framebufferGL != nullptr)
        {
            m_gl.BindFramebuffer(GL_FRAMEBUFFER, framebufferGL->GetOpenGLObject());
        }
        else
        {
            m_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        m_currentFramebuffer = hFramebuffer;
    }


    void GPURenderingDevice_OpenGL4::SetVertexShader(HShader hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_VERTEX_SHADER_BIT, shaderGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_VERTEX_SHADER_BIT, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::SetTessellationControlShader(HShader hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_TESS_CONTROL_SHADER_BIT, shaderGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_TESS_CONTROL_SHADER_BIT, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::SetTessellationEvaluationShader(HShader hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_TESS_EVALUATION_SHADER_BIT, shaderGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_TESS_EVALUATION_SHADER_BIT, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::SetGeometryShader(HShader hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_GEOMETRY_SHADER_BIT, shaderGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_GEOMETRY_SHADER_BIT, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::SetFragmentShader(HShader hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_FRAGMENT_SHADER_BIT, shaderGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_FRAGMENT_SHADER_BIT, 0);
        }
    }

    void GPURenderingDevice_OpenGL4::SetComputeShader(HShader hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_COMPUTE_SHADER_BIT, shaderGL->GetOpenGLObject());
        }
        else
        {
            m_gl.UseProgramStages(m_globalShaderPipeline, GL_COMPUTE_SHADER_BIT, 0);
        }
    }




    /////////////////////////////////////////////////////////////////////////////
    //
    // Stages
    //
    /////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////
    // Input-Assembler Stage

    void GPURenderingDevice_OpenGL4::IASetPrimitiveTopology(PrimitiveTopology topology)
    {
        GLenum topologiesGL[] =
        {
            GL_POINTS,                          // PrimitiveTopology_Point
            GL_LINES,                           // PrimitiveTopology_Line
            GL_LINE_STRIP,                      // PrimitiveTopology_LineStrip
            GL_TRIANGLES,                       // PrimitiveTopology_Triangle
            GL_TRIANGLE_STRIP,                  // PrimitiveTopology_TriangleStrip
            GL_LINES_ADJACENCY,                 // PrimitiveTopology_Line_Adjacency (Not supported in OpenGL 2.1)
            GL_LINE_STRIP_ADJACENCY,            // PrimitiveTopology_LineStrip_Adjacency (Not supported in OpenGL 2.1)
            GL_TRIANGLES_ADJACENCY,             // PrimitiveTopology_Triangle_Adjacency (Not supported in OpenGL 2.1)
            GL_TRIANGLE_STRIP_ADJACENCY         // PrimitiveTopology_TriangleStrip_Adjacency (Not supported in OpenGL 2.1)
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

                    assert(attribGL.slotIndex < GT_MAX_VERTEX_BUFFER_SLOTS);
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

        assert(slotIndex < GT_MAX_VERTEX_BUFFER_SLOTS);

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

    void GPURenderingDevice_OpenGL4::IASetIndexBuffer(HBuffer hBuffer, IndexFormat format, size_t offset)
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


        GLenum formatsGL[]   = { GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
        size_t formatSizes[] = { 1, 2, 4 };

        m_currentIndexBuffer    = hBuffer;
        m_indexBufferFormatGL   = formatsGL[format];
        m_indexBufferFormatSize = formatSizes[format];
        m_indexBufferOffset     = offset;
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
                GL_LINE,        // FillMode_Wireframe
                GL_FILL,        // FillMode_Solid
            };
            m_gl.PolygonMode(GL_FRONT_AND_BACK, fillModesGL[stateGL->fillMode]);
                

            // Cull mode.
            if (stateGL->cullMode == CullMode_None)
            {
                m_gl.Disable(GL_CULL_FACE);
            }
            else
            {
                m_gl.Enable(GL_CULL_FACE);
                m_gl.CullFace((stateGL->cullMode == CullMode_Back) ? GL_BACK : GL_FRONT);
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

    void GPURenderingDevice_OpenGL4::RSSetViewports(GraphicsViewport* viewports, size_t viewportCount)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        for (size_t iViewport = 0; iViewport < viewportCount; ++iViewport)
        {
            m_currentViewports[iViewport] = viewports[iViewport];
        }

        this->UpdateViewports();
    }

    void GPURenderingDevice_OpenGL4::RSSetScissorRects(ScissorRect* scissorRects, size_t scissorCount)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        for (size_t iScissor = 0; iScissor < scissorCount; ++iScissor)
        {
            m_currentScissorRects[iScissor] = scissorRects[iScissor];
        }

        this->UpdateScissorRects();
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
            m_gl.DepthMask(stateGL->depthWriteMask != DepthWriteMask_Zero);

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

    void GPURenderingDevice_OpenGL4::OMSetBlendState(HBlendState hState)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto stateGL = reinterpret_cast<BlendState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            // TODO: Profile this and consider storing a local copy of the relevant state and doing an early comparison before sending the OpenGL commands.

            GLenum blendParametersGL[] =
            {
                GL_ZERO,                            // BlendParameter_Zero
                GL_ONE,                             // BlendParameter_One

                GL_SRC_COLOR,                       // BlendParameter_Src_Color
                GL_ONE_MINUS_SRC_COLOR,             // BlendParameter_Inv_Src_Color
                GL_SRC_ALPHA,                       // BlendParameter_Src_Alpha
                GL_ONE_MINUS_SRC_ALPHA,             // BlendParameter_Inv_Src_Alpha
                GL_SRC_ALPHA_SATURATE,              // BlendParameter_Src_Alpha_Saturate
        
                GL_DST_COLOR,                       // BlendParameter_Dst_Color
                GL_ONE_MINUS_DST_COLOR,             // BlendParameter_Inv_Dst_Color
                GL_DST_ALPHA,                       // BlendParameter_Dst_Alpha
                GL_ONE_MINUS_DST_ALPHA,             // BlendParameter_Inv_Dst_Alpha
        
                GL_SRC1_COLOR,                      // BlendParameter_Src1_Color
                GL_ONE_MINUS_SRC1_COLOR,            // BlendParameter_Inv_Src1_Color
                GL_SRC1_ALPHA,                      // BlendParameter_Src1_Alpha
                GL_ONE_MINUS_SRC1_ALPHA,            // BlendParameter_Inv_Src1_Alpha

                GL_CONSTANT_COLOR,                  // BlendParameter_BlendFactor
                GL_ONE_MINUS_CONSTANT_COLOR         // BlendParameter_Inv_BlendFactor
            };

            GLenum blendOpsGL[] =
            {
                GL_FUNC_ADD,                        // BlendOp_Add
                GL_FUNC_SUBTRACT,                   // BlendOp_Subtract
                GL_FUNC_REVERSE_SUBTRACT,           // BlendOp_Reverse_Subtract
                GL_MIN,                             // BlendOp_Min
                GL_MAX                              // BlendOp_Max
            };


            if (stateGL->enableAlphaToCoverage)
            {
                m_gl.Enable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            }
            else
            {
                m_gl.Disable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            }


            if (stateGL->enableIndependentBlend)
            {
                // Independant.
                for (GLuint iRT = 0; iRT < 8; ++iRT)
                {
                    if (stateGL->renderTarget[iRT].enableBlending)
                    {
                        m_gl.Enablei(GL_BLEND, iRT);
                    }
                    else
                    {
                        m_gl.Disablei(GL_BLEND, iRT);
                    }

                    const GLenum srcColorParam = blendParametersGL[stateGL->renderTarget[iRT].srcBlendParameter];
                    const GLenum dstColorParam = blendParametersGL[stateGL->renderTarget[iRT].dstBlendParameter];
                    const GLenum srcAlphaParam = blendParametersGL[stateGL->renderTarget[iRT].srcAlphaBlendParameter];
                    const GLenum dstAlphaParam = blendParametersGL[stateGL->renderTarget[iRT].dstAlphaBlendParameter];
                    m_gl.BlendFuncSeparatei(iRT, srcColorParam, dstColorParam, srcAlphaParam, dstAlphaParam);

                    const GLenum colorOp = blendOpsGL[stateGL->renderTarget[iRT].blendOp];
                    const GLenum alphaOp = blendOpsGL[stateGL->renderTarget[iRT].blendOpAlpha];
                    m_gl.BlendEquationSeparatei(iRT, colorOp, alphaOp);

                    const GLboolean maskR = (stateGL->renderTarget[iRT].writeMask & (1 << 3)) >> 3;
                    const GLboolean maskG = (stateGL->renderTarget[iRT].writeMask & (1 << 2)) >> 2;
                    const GLboolean maskB = (stateGL->renderTarget[iRT].writeMask & (1 << 1)) >> 1;
                    const GLboolean maskA = (stateGL->renderTarget[iRT].writeMask & (1 << 0)) >> 0;
                    m_gl.ColorMaski(iRT, maskR, maskG, maskB, maskA);
                }
            }
            else
            {
                // Not independant.
                if (stateGL->renderTarget[0].enableBlending)
                {
                    m_gl.Enable(GL_BLEND);
                }
                else
                {
                    m_gl.Disable(GL_BLEND);
                }

                const GLenum srcColorParam = blendParametersGL[stateGL->renderTarget[0].srcBlendParameter];
                const GLenum dstColorParam = blendParametersGL[stateGL->renderTarget[0].dstBlendParameter];
                const GLenum srcAlphaParam = blendParametersGL[stateGL->renderTarget[0].srcAlphaBlendParameter];
                const GLenum dstAlphaParam = blendParametersGL[stateGL->renderTarget[0].dstAlphaBlendParameter];
                m_gl.BlendFuncSeparate(srcColorParam, dstColorParam, srcAlphaParam, dstAlphaParam);

                const GLenum colorOp = blendOpsGL[stateGL->renderTarget[0].blendOp];
                const GLenum alphaOp = blendOpsGL[stateGL->renderTarget[0].blendOpAlpha];
                m_gl.BlendEquationSeparate(colorOp, alphaOp);

                const GLboolean maskR = (stateGL->renderTarget[0].writeMask & (1 << 3)) >> 3;
                const GLboolean maskG = (stateGL->renderTarget[0].writeMask & (1 << 2)) >> 2;
                const GLboolean maskB = (stateGL->renderTarget[0].writeMask & (1 << 1)) >> 1;
                const GLboolean maskA = (stateGL->renderTarget[0].writeMask & (1 << 0)) >> 0;
                m_gl.ColorMask(maskR, maskG, maskB, maskA);
            }
        }
    }

    void GPURenderingDevice_OpenGL4::OMSetBlendFactor(float blendFactor[4])
    {
        m_gl.BlendColor(blendFactor[0], blendFactor[1], blendFactor[2], blendFactor[3]);
    }

    void GPURenderingDevice_OpenGL4::OMSetSampleMask(uint32_t sampleMask)
    {
        m_gl.SampleMaski(0, static_cast<GLbitfield>(sampleMask));
    }


    ////////////////////////////////////////////
    // State Objects

    HRasterizerState GPURenderingDevice_OpenGL4::CreateRasterizerState(const RasterizerStateDesc &desc)
    {
        return reinterpret_cast<HRasterizerState>(new RasterizerState_OpenGL4(desc));
    }

    void GPURenderingDevice_OpenGL4::DeleteRasterizerState(HRasterizerState hState)
    {
        auto stateGL = reinterpret_cast<RasterizerState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            delete stateGL;
        }
    }


    HDepthStencilState GPURenderingDevice_OpenGL4::CreateDepthStencilState(const DepthStencilStateDesc &desc)
    {
        return reinterpret_cast<size_t>(new DepthStencilState_OpenGL4(desc));
    }

    void GPURenderingDevice_OpenGL4::DeleteDepthStencilState(HDepthStencilState hState)
    {
        auto stateGL = reinterpret_cast<DepthStencilState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            delete stateGL;
        }
    }


    HBlendState GPURenderingDevice_OpenGL4::CreateBlendState(const BlendStateDesc &desc)
    {
        return reinterpret_cast<size_t>(new BlendState_OpenGL4(desc));
    }

    void GPURenderingDevice_OpenGL4::DeleteBlendState(HBlendState hState)
    {
        auto stateGL = reinterpret_cast<BlendState_OpenGL4*>(hState);
        if (stateGL != nullptr)
        {
            delete stateGL;
        }
    }



    ////////////////////////////////////////////
    // Input Layouts

    HInputLayout GPURenderingDevice_OpenGL4::CreateInputLayout(HShader hVertexShader, const InputLayoutAttribDesc* attribDesc, size_t attribDescCount)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hVertexShader);
        if (shaderGL != nullptr)
        {
            auto attribDescGL = reinterpret_cast<InputLayout_OpenGL4::AttributeDesc*>(malloc(sizeof(InputLayout_OpenGL4::AttributeDesc) * attribDescCount));
            if (attribDescGL != nullptr)
            {
                unsigned int slotAttribCounts[GT_MAX_VERTEX_BUFFER_SLOTS];
                memset(slotAttribCounts, 0, sizeof(slotAttribCounts));

                unsigned int iAttribGL = 0;
                for (unsigned int iSlot = 0; iSlot < GT_MAX_VERTEX_BUFFER_SLOTS && iAttribGL < attribDescCount; ++iSlot)
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
                            attribGL.attribLocation       = m_gl.GetAttribLocation(shaderGL->GetOpenGLObject(), attrib.attributeName);
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

    void GPURenderingDevice_OpenGL4::DeleteInputLayout(HInputLayout hInputLayout)
    {
        auto inputLayoutGL = reinterpret_cast<InputLayout_OpenGL4*>(hInputLayout);
        if (inputLayoutGL != nullptr)
        {
            if (m_currentInputLayout == hInputLayout)
            {
                this->IASetInputLayout(0);
            }

            delete inputLayoutGL;
        }
    }


    ////////////////////////////////////////////
    // Shaders

    ResultCode GPURenderingDevice_OpenGL4::CompileShader(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
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


    HShader GPURenderingDevice_OpenGL4::CreateShader(const void* shaderBinary, size_t shaderBinarySizeInBytes, ShaderType type)
    {
        const char* shaderSource;
        size_t shaderSourceLength;
        GTLib::Vector<ShaderDefine> defines;
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
                    return reinterpret_cast<HShader>(new Shader_OpenGL4(objectGL, typeGL));
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

    void GPURenderingDevice_OpenGL4::DeleteShader(HShader hShader)
    {
        auto shaderGL = reinterpret_cast<Shader_OpenGL4*>(hShader);
        if (shaderGL != nullptr)
        {
            CheckContextIsCurrent(m_gl, m_currentDC);

            m_gl.DeleteProgram(shaderGL->GetOpenGLObject());
            delete shaderGL;
        }
    }


    ///////////////////////////////////////////
    // Buffers

    HBuffer GPURenderingDevice_OpenGL4::CreateBuffer(BufferType type, BufferUsage usage, BufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data)
    {
        (void)type;     // <-- Not used with DSA.


        // Validation.
        if (usage == BufferUsage_Immutable && data == nullptr)
        {
            if (data == nullptr)
            {
                // No data specified for immutable buffer. Immutable buffers must have their data set a creation time.
                return 0;
            }

            if (cpuAccessFlags != BufferCPUAccess_None)
            {
                // Cannot access an immutable buffer from the CPU.
                return 0;
            }
        }

        if (usage == BufferUsage_Dynamic)
        {
            if ((cpuAccessFlags & BufferCPUAccess_Read) != 0)
            {
                // Specified a dynamic buffer and a read CPU access flag.

                // TODO: Output an error message saying that there is a conflict with the buffer usage and the CPU access flags.
                return 0;
            }

            if ((cpuAccessFlags & BufferCPUAccess_Write) == 0)
            {
                // Specified a dynamic buffer, but have not specified a CPU access flag. Recommend using an immutable or default buffer usage.

                // TODO: Output a warning message saying that an immutable buffer is recommended.
            }
        }


        CheckContextIsCurrent(m_gl, m_currentDC);

        GLbitfield flagsGL = 0;
        switch (usage)
        {
        case BufferUsage_Immutable:
            {
                break;
            }

        case BufferUsage_Dynamic:
            {
                flagsGL |= GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT;
                break;
            }

        case BufferUsage_Staging:
            {
                flagsGL |= GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
                break;
            }

        case BufferUsage_Default:
        default:
            {
                flagsGL |= GL_DYNAMIC_STORAGE_BIT;
                break;
            }
        }


        GLuint objectGL;
        m_gl.CreateBuffers(1, &objectGL);
        m_gl.NamedBufferStorage(objectGL, static_cast<GLsizeiptr>(sizeInBytes), data, flagsGL);

        return reinterpret_cast<HBuffer>(new Buffer_OpenGL4(objectGL, flagsGL, sizeInBytes));
    }

    void GPURenderingDevice_OpenGL4::DeleteBuffer(HBuffer hBuffer)
    {
        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        if (bufferGL != nullptr)
        {
            CheckContextIsCurrent(m_gl, m_currentDC);

            GLuint objectGL = bufferGL->GetOpenGLObject();
            m_gl.DeleteBuffers(1, &objectGL);

            delete bufferGL;
        }
    }

    void* GPURenderingDevice_OpenGL4::MapBuffer(HBuffer hBuffer, BufferMapType mapType)
    {
        CheckContextIsCurrent(m_gl, m_currentDC);

        auto bufferGL = reinterpret_cast<Buffer_OpenGL4*>(hBuffer);
        assert(bufferGL != nullptr);
        {
            GLbitfield flagsGL[] =
            {
                GL_MAP_READ_BIT,                                     // BufferMapType_Read
                GL_MAP_WRITE_BIT ,                                   // BufferMapType_Write
                GL_MAP_READ_BIT  | GL_MAP_WRITE_BIT,                 // BufferMapType_ReadWrite
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT,      // BufferMapType_Write_Discard
                GL_MAP_WRITE_BIT                                     // BufferMapType_Write_NoOverwrite
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

    HTexture GPURenderingDevice_OpenGL4::CreateTexture(const TextureDesc &desc)
    {
        GLsizei levelCountGL = desc.mipmapLevels;
        if (levelCountGL == 0)
        {
            levelCountGL = GTLib::ImageUtils::CalculateMipmapCount(desc.width, desc.height, desc.depth);
        }


        // Always use array formats, but not for cube maps since cube map arrays require slightly newer hardware.
        GLenum targetsGL[] =
        {
            GL_TEXTURE_1D_ARRAY,                // TextureType_1D,
            GL_TEXTURE_1D_ARRAY,                // TextureType_1D_Array,
            GL_TEXTURE_2D_ARRAY,                // TextureType_2D,
            GL_TEXTURE_2D_ARRAY,                // TextureType_2D_Array,
            GL_TEXTURE_2D_MULTISAMPLE_ARRAY,    // TextureType_2D_Multisample,
            GL_TEXTURE_2D_MULTISAMPLE_ARRAY,    // TextureType_2D_Multisample_Array,
            GL_TEXTURE_3D,                      // TextureType_3D,
            GL_TEXTURE_CUBE_MAP,                // TextureType_Cube,
            GL_TEXTURE_CUBE_MAP_ARRAY           // TextureType_Cube_Array
        };

        GLuint objectGL;
        m_gl.CreateTextures(targetsGL[desc.type], 1, &objectGL);

        switch (desc.type)
        {
        case TextureType_1D:
        case TextureType_1D_Array:
            {
                m_gl.TextureStorage2D(objectGL, levelCountGL, g_GLTextureFormatsTable[desc.format], desc.width, desc.height);
                break;
            }

        case TextureType_2D:
        case TextureType_2D_Array:
        case TextureType_3D:
        case TextureType_Cube:
        case TextureType_Cube_Array:
            {
                m_gl.TextureStorage3D(objectGL, levelCountGL, g_GLTextureFormatsTable[desc.format], desc.width, desc.height, desc.depth);
                break;
            }

        case TextureType_2D_Multisample:
        case TextureType_2D_Multisample_Array:
            {
                m_gl.TextureStorage3DMultisample(objectGL, levelCountGL, g_GLTextureFormatsTable[desc.format], desc.width, desc.height, desc.depth, GL_TRUE);
                break;
            }
        };
        

        return reinterpret_cast<HTexture>(new Texture_OpenGL4(objectGL, desc));
    }

    void GPURenderingDevice_OpenGL4::DeleteTexture(HTexture hTexture)   
    {
        auto textureGL = reinterpret_cast<Texture_OpenGL4*>(hTexture);
        if (textureGL != nullptr)
        {
            CheckContextIsCurrent(m_gl, m_currentDC);

            GLuint objectGL = textureGL->GetOpenGLObject();
            m_gl.DeleteTextures(1, &objectGL);

            delete textureGL;
        }
    }

    void GPURenderingDevice_OpenGL4::UpdateTexture(HTexture hTexture, int x, int y, int z, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmapLevel, const void* srcData)
    {
        auto textureGL = reinterpret_cast<Texture_OpenGL4*>(hTexture);
        assert(textureGL != nullptr);
        {
            GLenum formatGL = g_GLTextureSubImageFormatsTable[textureGL->GetDesc().format];
            GLenum typeGL   = g_GLTextureSubImageTypesTable[textureGL->GetDesc().format];

            // TODO: Check if we can use the 3D version with 1D textures with the DSA version. Doubting it will work, but worth having a quick check
            //       so we can get rid of the switches.

            // We need to calculate the row pitch of the destination rectangle, and if it's different to srcDataRowPitch we need to do a copy before submitting it to the driver.
            if (textureGL->GetDesc().format >= TextureFormat_RGBA_DXT1 && textureGL->GetDesc().format <= TextureFormat_RGB_SF16_BPTC)
            {
                // Compressed.
                switch (textureGL->GetDesc().format)
                {
                case TextureType_1D:
                case TextureType_1D_Array:
                    {
                        m_gl.CompressedTextureSubImage2D(textureGL->GetOpenGLObject(), mipmapLevel, x, y, width, height, formatGL, static_cast<GLsizei>(GetImageSizeInBytes(width, height, textureGL->GetDesc().format)), srcData);
                        break;
                    }

                case TextureType_2D:
                case TextureType_2D_Array:
                case TextureType_3D:
                case TextureType_Cube:
                case TextureType_Cube_Array:
                    {
                        m_gl.CompressedTextureSubImage3D(textureGL->GetOpenGLObject(), mipmapLevel, x, y, z, width, height, depth, formatGL, static_cast<GLsizei>(GetImageSizeInBytes(width, height, textureGL->GetDesc().format)), srcData);
                        break;
                    }

                default:
                    {
                        break;
                    }
                }
            }
            else
            {
                // Uncompressed.
                switch (textureGL->GetDesc().format)
                {
                case TextureType_1D:
                case TextureType_1D_Array:
                    {
                        m_gl.TextureSubImage2D(textureGL->GetOpenGLObject(), mipmapLevel, x, y, width, 1, formatGL, typeGL, srcData);
                        break;
                    }

                case TextureType_2D:
                case TextureType_2D_Array:
                case TextureType_3D:
                case TextureType_Cube:
                case TextureType_Cube_Array:
                    {
                        m_gl.TextureSubImage3D(textureGL->GetOpenGLObject(), mipmapLevel, x, y, z, width, height, 1, formatGL, typeGL, srcData);
                        break;
                    }

                default:
                    {
                        break;
                    }
                }
            }
        }
    }


    HTextureView GPURenderingDevice_OpenGL4::CreateTextureView(HTexture hOriginalTexture, TextureType type, TextureFormat format, unsigned int minLevel, unsigned int numLevels, unsigned int minLayer, unsigned int numLayers)
    {
        auto originalTextureGL = reinterpret_cast<Texture_OpenGL4*>(hOriginalTexture);
        assert(originalTextureGL != nullptr);
        {
            GLenum targetsGL[] =
            {
                GL_TEXTURE_1D,                      // TextureType_1D,
                GL_TEXTURE_1D_ARRAY,                // TextureType_1D_Array,
                GL_TEXTURE_2D,                      // TextureType_2D,
                GL_TEXTURE_2D_ARRAY,                // TextureType_2D_Array,
                GL_TEXTURE_2D_MULTISAMPLE,          // TextureType_2D_Multisample,
                GL_TEXTURE_2D_MULTISAMPLE_ARRAY,    // TextureType_2D_Multisample_Array,
                GL_TEXTURE_3D,                      // TextureType_3D,
                GL_TEXTURE_CUBE_MAP,                // TextureType_Cube,
                GL_TEXTURE_CUBE_MAP_ARRAY           // TextureType_Cube_Array
            };

            // Note here howe we use glGenTextures instead of the DSA glCreateTextures(). This is because glCreateTextures() will initialize the object, wherease in
            // fact we want glTextureView() to be the one to initialize it. If we don't do this we'll get a GL_INVALID_VALUE error.
            GLuint textureViewGL;
            m_gl.GenTextures(1, &textureViewGL);
            m_gl.TextureView(textureViewGL, targetsGL[type], originalTextureGL->GetOpenGLObject(), g_GLTextureFormatsTable[format], minLevel, numLevels, minLayer, numLayers);

            return reinterpret_cast<HTextureView>(new TextureView_OpenGL4(textureViewGL));
        }
    }

    void GPURenderingDevice_OpenGL4::DeleteTextureView(HTextureView hTextureView)
    {
        auto textureGL = reinterpret_cast<Texture_OpenGL4*>(hTextureView);
        if (textureGL != nullptr)
        {
            CheckContextIsCurrent(m_gl, m_currentDC);

            GLuint objectGL = textureGL->GetOpenGLObject();
            m_gl.DeleteTextures(1, &objectGL);

            delete textureGL;
        }
    }



    ///////////////////////////////////////////
    // Samplers

    HSampler GPURenderingDevice_OpenGL4::CreateSampler(const SamplerDesc &desc)
    {
        GLuint objectGL;
        m_gl.CreateSamplers(1, &objectGL);


        GLint minFiltersGL[] =
        {
            GL_NEAREST_MIPMAP_NEAREST,  // TextureFilter_Point_Point_Point
            GL_NEAREST_MIPMAP_LINEAR,   // TextureFilter_Point_Point_Linear
            GL_NEAREST_MIPMAP_NEAREST,  // TextureFilter_Point_Linear_Point
            GL_NEAREST_MIPMAP_LINEAR,   // TextureFitler_Point_Linear_Linear
            GL_LINEAR_MIPMAP_NEAREST,   // TextureFilter_Linear_Point_Point
            GL_LINEAR_MIPMAP_LINEAR,    // TextureFilter_Linear_Point_Linear
            GL_LINEAR_MIPMAP_NEAREST,   // TextureFilter_Linear_Linear_Point
            GL_LINEAR_MIPMAP_LINEAR,    // TextureFilter_Linear_Linear_Linear
            GL_LINEAR_MIPMAP_LINEAR     // TextureFilter_Anisotropic
        };
        
        GLint magFiltersGL[] =
        {
            GL_NEAREST,                 // TextureFilter_Point_Point_Point
            GL_NEAREST,                 // TextureFilter_Point_Point_Linear
            GL_LINEAR,                  // TextureFilter_Point_Linear_Point
            GL_LINEAR,                  // TextureFitler_Point_Linear_Linear
            GL_NEAREST,                 // TextureFilter_Linear_Point_Point
            GL_NEAREST,                 // TextureFilter_Linear_Point_Linear
            GL_LINEAR,                  // TextureFilter_Linear_Linear_Point
            GL_LINEAR,                  // TextureFilter_Linear_Linear_Linear
            GL_LINEAR                   // TextureFilter_Anisotropic
        };

        GLint addressModesGL[] =
        {
            GL_REPEAT,                  // TextureAddressMode_Wrap
            GL_MIRRORED_REPEAT,         // TextureAddressMode_Mirror
            GL_CLAMP_TO_EDGE,           // TextureAddressMode_Clamp
            GL_CLAMP_TO_BORDER,         // TextureAddressMode_Border
            GL_MIRROR_CLAMP_TO_EDGE     // TextureAddressMode_Mirror_Once
        };

        // Filter.
        m_gl.SamplerParameteri(objectGL, GL_TEXTURE_MIN_FILTER, minFiltersGL[desc.filter]);
        m_gl.SamplerParameteri(objectGL, GL_TEXTURE_MAG_FILTER, magFiltersGL[desc.filter]);

        // Address Modes.
        m_gl.SamplerParameteri(objectGL, GL_TEXTURE_WRAP_S, addressModesGL[desc.addressModeU]);
        m_gl.SamplerParameteri(objectGL, GL_TEXTURE_WRAP_T, addressModesGL[desc.addressModeV]);
        m_gl.SamplerParameteri(objectGL, GL_TEXTURE_WRAP_R, addressModesGL[desc.addressModeW]);

        // LOD Bias.
        m_gl.SamplerParameterf(objectGL, GL_TEXTURE_LOD_BIAS, desc.mipLODBias);

        // Anisotropy.
        if (desc.maxAnisotropy > 0)
        {
            m_gl.SamplerParameterf(objectGL, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(desc.maxAnisotropy));
        }

        // Border Colour.
        m_gl.SamplerParameterfv(objectGL, GL_TEXTURE_BORDER_COLOR, desc.borderColor);

        // LOD
        m_gl.SamplerParameterf(objectGL, GL_TEXTURE_MIN_LOD, desc.minLOD);
        m_gl.SamplerParameterf(objectGL, GL_TEXTURE_MAX_LOD, desc.maxLOD);


        return reinterpret_cast<HSampler>(new Sampler_OpenGL4(objectGL));
    }

    void GPURenderingDevice_OpenGL4::DeleteSampler(HSampler hSampler)
    {
        auto samplerGL = reinterpret_cast<Sampler_OpenGL4*>(hSampler);
        if (samplerGL != nullptr)
        {
            CheckContextIsCurrent(m_gl, m_currentDC);

            GLuint objectGL = samplerGL->GetOpenGLObject();
            m_gl.DeleteSamplers(1, &objectGL);

            delete samplerGL;
        }
    }


    ///////////////////////////////////////////
    // Framebuffers

    HFramebuffer GPURenderingDevice_OpenGL4::CreateFramebuffer()
    {
        GLuint objectGL;
        m_gl.CreateFramebuffers(1, &objectGL);

        return reinterpret_cast<HFramebuffer>(new Framebuffer_OpenGL4(objectGL));
    }

    void GPURenderingDevice_OpenGL4::DeleteFramebuffer(HFramebuffer hFramebuffer)
    {
        auto framebufferGL = reinterpret_cast<Sampler_OpenGL4*>(hFramebuffer);
        if (framebufferGL != nullptr)
        {
            CheckContextIsCurrent(m_gl, m_currentDC);

            GLuint objectGL = framebufferGL->GetOpenGLObject();
            m_gl.DeleteFramebuffers(1, &objectGL);

            delete framebufferGL;
        }
    }

    void GPURenderingDevice_OpenGL4::AttachFramebufferRenderTarget(HFramebuffer hFramebuffer, unsigned int attachmentIndex, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer)
    {
        auto framebufferGL = reinterpret_cast<Framebuffer_OpenGL4*>(hFramebuffer);
        assert(framebufferGL != nullptr);
        {
            auto textureGL = reinterpret_cast<Texture_OpenGL4*>(hTexture);
            if (textureGL != nullptr)
            {
                m_gl.NamedFramebufferTextureLayer(framebufferGL->GetOpenGLObject(), GL_COLOR_ATTACHMENT0 + attachmentIndex, textureGL->GetOpenGLObject(), static_cast<GLint>(mipmapLevel), static_cast<GLint>(arrayLayer));
                framebufferGL->renderTargetsGL[attachmentIndex] = GL_COLOR_ATTACHMENT0 + attachmentIndex;
            }
            else
            {
                m_gl.NamedFramebufferTextureLayer(framebufferGL->GetOpenGLObject(), GL_COLOR_ATTACHMENT0 + attachmentIndex, 0, static_cast<GLint>(mipmapLevel), static_cast<GLint>(arrayLayer));
                framebufferGL->renderTargetsGL[attachmentIndex] = GL_NONE;
            }

            m_gl.NamedFramebufferDrawBuffers(framebufferGL->GetOpenGLObject(), GT_MAX_FRAMEBUFFER_RENDER_TARGETS, framebufferGL->renderTargetsGL);
        }
    }

    void GPURenderingDevice_OpenGL4::AttachFramebufferDepthStencilTarget(HFramebuffer hFramebuffer, HTexture hTexture, unsigned int mipmapLevel, unsigned int arrayLayer)
    {
        auto framebufferGL = reinterpret_cast<Framebuffer_OpenGL4*>(hFramebuffer);
        assert(framebufferGL != nullptr);
        {
            auto textureGL = reinterpret_cast<Texture_OpenGL4*>(hTexture);
            if (textureGL != nullptr)
            {
                m_gl.NamedFramebufferTextureLayer(framebufferGL->GetOpenGLObject(), GL_DEPTH_STENCIL_ATTACHMENT, textureGL->GetOpenGLObject(), static_cast<GLint>(mipmapLevel), static_cast<GLint>(arrayLayer));
            }
            else
            {
                m_gl.NamedFramebufferTextureLayer(framebufferGL->GetOpenGLObject(), GL_DEPTH_STENCIL_ATTACHMENT, 0, static_cast<GLint>(mipmapLevel), static_cast<GLint>(arrayLayer));
            }
        }
    }

    void GPURenderingDevice_OpenGL4::ClearFramebufferColor(HFramebuffer hFramebuffer, unsigned int attachmentIndex, float color[4])
    {
        auto framebufferGL = reinterpret_cast<Framebuffer_OpenGL4*>(hFramebuffer);
        if (framebufferGL != nullptr)
        {
            m_gl.ClearNamedFramebufferfv(framebufferGL->GetOpenGLObject(), GL_COLOR, static_cast<GLint>(attachmentIndex), color);
        }
        else
        {
            // Clear the default framebuffer.
            m_gl.ClearNamedFramebufferfv(0, GL_COLOR, 0, color);
        }
    }

    void GPURenderingDevice_OpenGL4::ClearFramebufferDepth(HFramebuffer hFramebuffer, float depth)
    {
        auto framebufferGL = reinterpret_cast<Framebuffer_OpenGL4*>(hFramebuffer);
        if (framebufferGL != nullptr)
        {
            m_gl.ClearNamedFramebufferfv(framebufferGL->GetOpenGLObject(), GL_DEPTH, 0, &depth);
        }
        else
        {
            // Clear the default framebuffer.
            m_gl.ClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth);
        }
    }

    void GPURenderingDevice_OpenGL4::ClearFramebufferStencil(HFramebuffer hFramebuffer, uint8_t stencil)
    {
        GLuint stencilGL = stencil;

        auto framebufferGL = reinterpret_cast<Framebuffer_OpenGL4*>(hFramebuffer);
        if (framebufferGL != nullptr)
        {
            m_gl.ClearNamedFramebufferuiv(framebufferGL->GetOpenGLObject(), GL_STENCIL, 0, &stencilGL);
        }
        else
        {
            // Clear the default framebuffer.
            m_gl.ClearNamedFramebufferuiv(0, GL_STENCIL, 0, &stencilGL);
        }
    }

    void GPURenderingDevice_OpenGL4::ClearFramebufferDepthStencil(HFramebuffer hFramebuffer, float depth, uint8_t stencil)
    {
        // NOTE: It looks like glClearNamedFramebufferfi() is broken on NVIDIA as it is throwing an OpenGL error saying invalid draw buffer. To work around
        //       we will instead use to separate clear operations on NVIDIA.
        //
        // TODO: Check whether or not this has been fixed.

        auto framebufferGL = reinterpret_cast<Framebuffer_OpenGL4*>(hFramebuffer);
        if (framebufferGL != nullptr)
        {
            if (m_vendor == GPUVendor_NVIDIA)
            {
                GLuint stencilGL = stencil;
                m_gl.ClearNamedFramebufferfv(framebufferGL->GetOpenGLObject(), GL_DEPTH, 0, &depth);
                m_gl.ClearNamedFramebufferuiv(framebufferGL->GetOpenGLObject(), GL_STENCIL, 0, &stencilGL);
            }
            else
            {
                // TODO: Test this branch on NVIDIA.
                m_gl.ClearNamedFramebufferfi(framebufferGL->GetOpenGLObject(), GL_DEPTH_STENCIL, depth, stencil);
            }
            
        }
        else
        {
            // Clear the default framebuffer.
            if (m_vendor == GPUVendor_NVIDIA)
            {
                GLuint stencilGL = stencil;
                m_gl.ClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth);
                m_gl.ClearNamedFramebufferuiv(0, GL_STENCIL, 0, &stencilGL);
            }
            else
            {
                m_gl.ClearNamedFramebufferfi(0, GL_DEPTH_STENCIL, depth, stencil);
            }
        }
    }



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

                m_gl.MakeCurrent(framebuffer.m_hDC, m_gl.GetRenderingContext());


                // We need to retrieve the height of the window and normalize the viewport and scissor rectangles.
                if (m_currentHWND != hWnd)
                {
                    RECT windowRect;
                    GetClientRect(hWnd, &windowRect);
                    m_currentWindowHeight = windowRect.bottom - windowRect.top;

                    this->UpdateViewports();
                    this->UpdateScissorRects();
                }

                
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
            m_gl.MakeCurrent(m_gl.GetDummyDC(), NULL);

            m_currentHWND = NULL;
            m_currentDC   = m_gl.GetDummyDC();
        }

        return 0;   // No error.
    }

    void GPURenderingDevice_OpenGL4::ResizeWindowFramebuffer(HWND hWnd)
    {
        // We need to get the size of the window and update the viewports and scissor rectangles.
        RECT windowRect;
        GetClientRect(hWnd, &windowRect);
        m_currentWindowHeight = windowRect.bottom - windowRect.top;

        this->UpdateViewports();
        this->UpdateScissorRects();
    }
#endif

#if defined(GT_PLATFORM_LINUX)
#endif



    //////////////////////////////////////////
    // Private


    // TODO: Rename this function.
    void GPURenderingDevice_OpenGL4::UpdateSlotVertexAttributePointers(unsigned int slotIndex)
    {
        assert(slotIndex < GT_MAX_VERTEX_BUFFER_SLOTS);

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
                        if (attribGL.attributeClass == AttribInputClassification_PerInstance)
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


    ResultCode GPURenderingDevice_OpenGL4::CompileShader_GLSL(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
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

    ResultCode GPURenderingDevice_OpenGL4::CompileShader_GLSL(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, GT::BasicBuffer &messagesOut, GLuint &objectGLOut, GLenum &typeGLOut)
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

        GTLib::String extensionsString;
        if (language < ShaderLanguage_GLSL_420)
        {
            extensionsString.Append("#extension GL_ARB_shading_language_420pack : enable\n");
        }

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


        const char* shaderStrings[5];
        shaderStrings[0] = versionStrings[language];
        shaderStrings[1] = extensionsString.c_str();
        shaderStrings[2] = definesString.c_str();
        shaderStrings[3] = vertexOutStruct;
        shaderStrings[4] = source;

        GLint shaderStringLengths[5];
        shaderStringLengths[0] = -1;        // Null-terminated.
        shaderStringLengths[1] = static_cast<GLint>(extensionsString.GetLength());
        shaderStringLengths[2] = static_cast<GLint>(definesString.GetLength());
        shaderStringLengths[3] = -1;
        shaderStringLengths[4] = static_cast<GLint>((sourceLength > 0) ? sourceLength : -1);

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


    void GPURenderingDevice_OpenGL4::UpdateViewports()
    {
        for (int i = 0; i < GT_MAX_VIEWPORTS; ++i)
        {
            GLfloat viewportX      = static_cast<GLfloat>(m_currentViewports[i].x);
            GLfloat viewportY      = static_cast<GLfloat>(m_currentWindowHeight - m_currentViewports[i].y - m_currentViewports[i].height);
            GLfloat viewportWidth  = static_cast<GLfloat>(m_currentViewports[i].width);
            GLfloat viewportHeight = static_cast<GLfloat>(m_currentViewports[i].height);
            m_gl.ViewportIndexedf(i, viewportX, viewportY, viewportWidth, viewportHeight);

            GLdouble viewportDepthRangeNear = static_cast<GLdouble>(m_currentViewports[i].depthRangeNear);
            GLdouble viewportDepthRangeFar  = static_cast<GLdouble>(m_currentViewports[i].depthRangeFar);
            m_gl.DepthRangeIndexed(i, viewportDepthRangeNear, viewportDepthRangeFar);
        }
    }

    void GPURenderingDevice_OpenGL4::UpdateScissorRects()
    {
        for (int i = 0; i < GT_MAX_VIEWPORTS; ++i)
        {
            GLint scissorX        = static_cast<GLint>(m_currentScissorRects[i].x);
            GLint scissorY        = static_cast<GLint>(m_currentWindowHeight - m_currentScissorRects[i].y);
            GLsizei scissorWidth  = static_cast<GLsizei>(m_currentScissorRects[i].width);
            GLsizei scissorHeight = static_cast<GLsizei>(m_currentScissorRects[i].height);
            m_gl.ScissorIndexed(i, scissorX, scissorY, scissorWidth, scissorHeight);
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
