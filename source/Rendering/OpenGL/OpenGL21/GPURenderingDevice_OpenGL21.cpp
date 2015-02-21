// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)
#include "GPUShaderProgram_OpenGL21.hpp"
#include "GPUBuffer_OpenGL21.hpp"
#include "GPUInputLayout_OpenGL21.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

#include <GTLib/String.hpp>
#include <GTLib/Math.hpp>

namespace GT
{
    namespace GE
    {
        GPURenderingDevice_OpenGL21::GPURenderingDevice_OpenGL21(const GPURenderingDeviceInfo &info)
            : GPURenderingDevice_Gen1(info),
              m_gl(),
#if defined(GT_PLATFORM_WINDOWS)
              m_currentHWND(NULL),
              m_currentDC(NULL),
#endif
              m_supportedShaderTargets(),
              m_vertexBufferSlots(),
              m_invalidVertexBufferSlots(),
              m_stateFlags(0),
              m_currentTopologyGL(GL_TRIANGLES),
              m_currentIndexBuffer(nullptr),
              m_currentInputLayout(nullptr)
        {
            m_stateFlags |= StageFlag_IsWindowFramebufferCurrent;       // TODO: Remove this from the constructor once we get the framebuffer system working properly.
        }

        GPURenderingDevice_OpenGL21::~GPURenderingDevice_OpenGL21()
        {
        }


        ResultCode GPURenderingDevice_OpenGL21::Startup()
        {
            if (m_info.identifier_OpenGL == 1 && IsRenderingAPISupported(m_info, RenderingAPI_OpenGL21))
            {
                ResultCode result = m_gl.Startup();
                if (Succeeded(result))
                {
                    m_supportedShaderTargets.PushBack(GPUShaderTarget_GLSL_120_VS);
                    m_supportedShaderTargets.PushBack(GPUShaderTarget_GLSL_120_FS);

                    // TODO: Check for ARB program support.
                }

                return result;
            }
            else
            {
                // The device is not the primary adapter or OpenGL 2.1 is not supported.
                return RenderingAPINotSupported;
            }
        }

        void GPURenderingDevice_OpenGL21::Shutdown()
        {
            m_supportedShaderTargets.Clear();

            m_gl.Shutdown();
        }


        RenderingAPI GPURenderingDevice_OpenGL21::GetRenderingAPI() const
        {
            return RenderingAPI_OpenGL21;
        }


        void GPURenderingDevice_OpenGL21::SetSwapInterval(int swapInterval)
        {
            (void)swapInterval;
        }



        ///////////////////////////////////////////
        // Drawing

        void GPURenderingDevice_OpenGL21::ClearColor(float r, float g, float b, float a)
        {
            m_gl.ClearColor(r, g, b, a);
            m_gl.Clear(GL_COLOR_BUFFER_BIT);
        }

        void GPURenderingDevice_OpenGL21::Draw(unsigned int indexCount, unsigned int startIndexLocation)
        {
            if (m_currentIndexBuffer != nullptr)
            {
                // Update the vertex attribute pointers if any are invalid.
                while (m_invalidVertexBufferSlots != 0)
                {
                    this->UpdateSlotVertexAttributePointers(GTLib::NextBitIndex(m_invalidVertexBufferSlots));
                }

                //m_gl.Viewport(0, 0, 100, 100);      // TODO: <-- Testing. Remove this later.
                m_gl.DrawElements(m_currentTopologyGL, indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(startIndexLocation*sizeof(uint32_t)));
            }
        }

        

        ///////////////////////////////////////////
        // State

        void GPURenderingDevice_OpenGL21::SetCurrentShaderProgram(GPUShaderProgram* shaderProgram)
        {
            auto shaderProgramGL = reinterpret_cast<GPUShaderProgram_OpenGL21*>(shaderProgram);
            if (shaderProgramGL != nullptr)
            {
                m_gl.UseProgram(shaderProgramGL->GetOpenGLObject());
            }
            else
            {
                m_gl.UseProgram(0);
            }
        }




        /////////////////////////////////////////////////////////////////////////////
        //
        // Stages
        //
        /////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////
        // Input-Assembler Stage

        void GPURenderingDevice_OpenGL21::IASetPrimitiveTopology(GPUPrimitiveTopology topology)
        {
            GLenum topologiesGL[] =
            {
                GL_POINTS,                          // GPUPrimitiveTopology_Point
                GL_LINES,                           // GPUPrimitiveTopology_Line
                GL_LINE_STRIP,                      // GPUPrimitiveTopology_LineStrip
                GL_TRIANGLES,                       // GPUPrimitiveTopology_Triangle
                GL_TRIANGLE_STRIP,                  // GPUPrimitiveTopology_TriangleStrip
                GL_LINES,                           // GPUPrimitiveTopology_Line_Adjacency (Not supported in OpenGL 2.1)
                GL_LINE_STRIP,                      // GPUPrimitiveTopology_LineStrip_Adjacency (Not supported in OpenGL 2.1)
                GL_TRIANGLES,                       // GPUPrimitiveTopology_Triangle_Adjacency (Not supported in OpenGL 2.1)
                GL_TRIANGLE_STRIP                   // GPUPrimitiveTopology_TriangleStrip_Adjacency (Not supported in OpenGL 2.1)
            };

            m_currentTopologyGL = topologiesGL[topology];
        }

        void GPURenderingDevice_OpenGL21::IASetInputLayout(GPUInputLayout* vertexInputLayout)
        {
            if (vertexInputLayout != m_currentInputLayout)
            {
                // Disable the vertex attributes of the previous input layout.
                if (m_currentInputLayout != nullptr)
                {
                    auto prevLayoutGL = reinterpret_cast<GPUInputLayout_OpenGL21*>(m_currentInputLayout);
                    if (prevLayoutGL != nullptr)
                    {
                        size_t attribCount = prevLayoutGL->GetAttributeCount();
                        for (size_t iAttrib = 0; iAttrib < attribCount; ++iAttrib)
                        {
                            m_gl.DisableVertexAttribArray(prevLayoutGL->GetAttribute(iAttrib).attribLocation);
                        }
                    }
                }


                auto newLayoutGL = reinterpret_cast<GPUInputLayout_OpenGL21*>(vertexInputLayout);
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


                m_currentInputLayout = vertexInputLayout;
            }
        }

        void GPURenderingDevice_OpenGL21::IASetVertexBuffer(unsigned int slotIndex, GPUBuffer* buffer, size_t stride, size_t offset)
        {
            assert(slotIndex < GT_GE_MAX_VERTEX_BUFFER_SLOTS);

            m_vertexBufferSlots[slotIndex].buffer = buffer;
            m_vertexBufferSlots[slotIndex].stride = stride;
            m_vertexBufferSlots[slotIndex].offset = offset;


            // Update the vertex attribute pointers.
            this->UpdateSlotVertexAttributePointers(slotIndex);
        }

        void GPURenderingDevice_OpenGL21::IASetIndexBuffer(GPUBuffer* buffer)
        {
            auto bufferGL = reinterpret_cast<GPUBuffer_OpenGL21*>(buffer);
            if (bufferGL != nullptr)
            {
                assert(bufferGL->GetOpenGLTarget() == GL_ELEMENT_ARRAY_BUFFER);
                {
                    m_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferGL->GetOpenGLObject());
                }
            }
            else
            {
                m_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }


            m_currentIndexBuffer = buffer;
        }


        /////////////////////////////////////////////
        // Rasterization Stage

        void GPURenderingDevice_OpenGL21::RSSetViewports(GPUViewport* viewports, size_t viewportCount)
        {
            if (viewports != nullptr && viewportCount > 0)
            {
                m_gl.Viewport(static_cast<GLint>(viewports[0].x), static_cast<GLint>(viewports[0].y), static_cast<GLsizei>(viewports[0].width), static_cast<GLsizei>(viewports[0].height));
                m_gl.DepthRange(viewports[0].depthRangeNear, viewports[0].depthRangeFar);
            }
        }


        ////////////////////////////////////////////
        // Shaders

        ResultCode GPURenderingDevice_OpenGL21::CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
        {
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

        bool GPURenderingDevice_OpenGL21::IsShaderTargetSupported(GPUShaderTarget target) const
        {
            return m_supportedShaderTargets.Exists(target);
        }

        ResultCode GPURenderingDevice_OpenGL21::CreateShaderProgram(const void* vertexShaderData, size_t vertexShaderDataSize, const void* fragmentShaderData, size_t fragmentShaderDataSize, GT::BasicBuffer &messagesOut, GPUShaderProgram* &shaderProgramOut)
        {
            // TODO: Check that the shader targets are of the correct shader stage.

            ResultCode result = 0;

            // Vertex Shader.
            const char* vertexSource;
            size_t vertexSourceLength;
            GTLib::Vector<GPUShaderDefine> vertexDefines;
            GPUShaderTarget vertexTarget;
            result = this->ExtractShaderBinaryData(vertexShaderData, vertexShaderDataSize, vertexSource, vertexSourceLength, vertexDefines, vertexTarget);
            if (Failed(result))
            {
                return result;
            }

            // Fragment Shader.
            const char* fragmentSource;
            size_t fragmentSourceLength;
            GTLib::Vector<GPUShaderDefine> fragmentDefines;
            GPUShaderTarget fragmentTarget;
            result = this->ExtractShaderBinaryData(fragmentShaderData, fragmentShaderDataSize, fragmentSource, fragmentSourceLength, fragmentDefines, fragmentTarget);
            if (Failed(result))
            {
                return result;
            }



            // The shader types must be compatible.
            if (vertexTarget == GPUShaderTarget_GLSL_120_VS && fragmentTarget != GPUShaderTarget_GLSL_120_FS)
            {
                return ShaderTargetNotCompatible;
            }



            // Vertex object.
            GLuint vertexObjectGL = 0;
            if (this->IsShaderTargetSupported(vertexTarget))
            {
                if (vertexTarget == GPUShaderTarget_GLSL_120_VS)
                {
                    result = this->CompileShader_GLSL(vertexSource, vertexSourceLength, vertexDefines.buffer, vertexTarget, messagesOut, vertexObjectGL);
                    if (GT::Failed(result))
                    {
                        return result;
                    }
                }

                if (vertexTarget == GPUShaderTarget_ARB_VP)
                {
                    // TODO: Implement this!
                    return ShaderTargetNotSupported;
                }
            }
            else
            {
                return ShaderTargetNotSupported;
            }


            // Fragment object.
            GLuint fragmentObjectGL = 0;
            if (this->IsShaderTargetSupported(fragmentTarget))
            {
                if (fragmentTarget == GPUShaderTarget_GLSL_120_FS)
                {
                    result = this->CompileShader_GLSL(fragmentSource, fragmentSourceLength, fragmentDefines.buffer, fragmentTarget, messagesOut, fragmentObjectGL);
                    if (GT::Failed(result))
                    {
                        return result;
                    }
                }

                if (fragmentTarget == GPUShaderTarget_ARB_FP)
                {
                    // TODO: Implement this!
                    return ShaderTargetNotSupported;
                }
            }
            else
            {
                return ShaderTargetNotSupported;
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


                ResultCode result = 0;

                // Check for link errors.
                GLint isLinked = 0;
                m_gl.GetProgramiv(objectGL, GL_LINK_STATUS, &isLinked);
                if(isLinked == GL_TRUE)
                {
                    shaderProgramOut = new GPUShaderProgram_OpenGL21(objectGL);
                }
                else
                {
                    result = FailedToLinkProgram;
                }


                m_gl.DeleteShader(vertexObjectGL);
                m_gl.DeleteShader(fragmentObjectGL);


                return result;
            }
            else
            {
                return FailedToCreateOpenGLProgramObject;
            }



            return 0;
        }

        void GPURenderingDevice_OpenGL21::DeleteShaderProgram(GPUShaderProgram* shaderProgram)
        {
            auto shaderProgramGL = reinterpret_cast<GPUShaderProgram_OpenGL21*>(shaderProgram);
            if (shaderProgramGL != nullptr)
            {
                m_gl.DeleteProgram(shaderProgramGL->GetOpenGLObject());
                delete shaderProgram;
            }
        }

        ResultCode GPURenderingDevice_OpenGL21::CreateInputLayout(GPUShaderProgram* shaderProgram, const GPUInputLayoutAttribDesc* attribDesc, size_t attribDescCount, GPUInputLayout* &vertexInputLayoutOut)
        {
            auto shaderProgramGL = reinterpret_cast<GPUShaderProgram_OpenGL21*>(shaderProgram);
            if (shaderProgramGL != nullptr)
            {
                auto attribDescGL = reinterpret_cast<GPUInputLayout_OpenGL21::AttributeDesc*>(malloc(sizeof(GPUInputLayout_OpenGL21::AttributeDesc) * attribDescCount));
                if (attribDescGL != nullptr)
                {
                    unsigned int slotAttribCounts[GT_GE_MAX_VERTEX_BUFFER_SLOTS];
                    memset(slotAttribCounts, 0, sizeof(slotAttribCounts));

                    unsigned int iAttribGL = 0;
                    for (unsigned int iSlot = 0; iSlot < GT_GE_MAX_VERTEX_BUFFER_SLOTS && iAttribGL < attribDescCount; ++iSlot)
                    {
                        GLsizei currentOffset = 0;

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
                                attribGL.attribOffset         = currentOffset;
                                attribGL.attribComponentCount = attrib.attributeComponentCount;

                                GLenum attribComponentTypesGL[] =
                                {
                                    GL_FLOAT,               // GPUBasicType_Float
                                    GL_INT,                 // GPUBasicType_SInt
                                    GL_UNSIGNED_INT         // GPUBasicType_UInt
                                };
                                attribGL.attribComponentType = attribComponentTypesGL[attrib.attributeComponentType];

                                attribGL.attributeClass   = attrib.attributeClass;
                                attribGL.instanceStepRate = attrib.instanceStepRate;



                                // Move the offset.
                                GLsizei attribComponentSizes[] =
                                {
                                    4,      // GPUBasicType_Float
                                    4,      // GPUBasicType_SInt
                                    4       // GPUBasicType_UInt
                                };
                                GLsizei attribComponentSize = attribComponentSizes[attrib.attributeComponentType];

                                currentOffset += attribComponentSize * attrib.attributeComponentCount + attrib.attributePadding;



                                iAttribGL += 1;
                            }
                        }
                    }



                    vertexInputLayoutOut = new GPUInputLayout_OpenGL21(attribDescGL, attribDescCount, slotAttribCounts);


                    free(attribDescGL);
                    return 0;   // No errors.
                }
                else
                {
                    // Failed to allocate memory.
                    return -1;
                }


                /*
                GLuint prevProgramGL;
                m_gl.GetIntegerv(GL_ACTIVE_PROGRAM, reinterpret_cast<GLint*>(&prevProgramGL));
                {
                    m_gl.UseProgram(shaderProgramGL->GetOpenGLObject());
                }
                m_gl.UseProgram(prevProgramGL);
                */
            }
            else
            {
                // Invalid shader program.
                return -1;
            }
        }

        void GPURenderingDevice_OpenGL21::DeleteInputLayout(GPUInputLayout* vertexInputLayout)
        {
            if (m_currentInputLayout == vertexInputLayout)
            {
                this->IASetInputLayout(nullptr);
            }

            delete vertexInputLayout;
        }


        ///////////////////////////////////////////
        // Buffers

        ResultCode GPURenderingDevice_OpenGL21::CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data, GPUBuffer* &bufferOut)
        {
            bufferOut = nullptr;

            if (usage == GPUBufferUsage_Immutable && data == nullptr)
            {
                return NoDataSpecifiedForImmutableBuffer;
            }

            if (type == GPUBufferType_Constant)
            {
                return UnsupportedGPUBufferType;
            }


            GLuint targetGL = 0;
            GLuint usageGL  = 0;

            switch (type)
            {
            case GPUBufferType_Vertex:
                {
                    targetGL = GL_ARRAY_BUFFER;
                    break;
                }

            case GPUBufferType_Index:
                {
                    targetGL = GL_ELEMENT_ARRAY_BUFFER;
                    break;
                }

            case GPUBufferType_Constant:
                {
                    return UnsupportedGPUBufferType;
                }

            default:
                {
                    return UnknownGPUBufferType;
                }
            }


            switch (usage)
            {
            case GPUBufferUsage_Default:
                {
                    usageGL = GL_STREAM_DRAW;
                    break;
                }

            case GPUBufferUsage_Immutable:
                {
                    usageGL = GL_STATIC_DRAW;
                    break;
                }

            case GPUBufferUsage_Dynamic:
                {
                    usageGL = GL_DYNAMIC_DRAW;
                    break;
                }

            case GPUBufferUsage_Staging:
                {
                    usageGL = GL_DYNAMIC_READ;
                    break;
                }

            default:
                {
                    return UnknownGPUBufferUsage;
                }
            }



            GLuint prevObjectGL;
            m_gl.GetIntegerv((targetGL == GL_ARRAY_BUFFER) ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&prevObjectGL));


            GLuint objectGL;
            m_gl.GenBuffers(1, &objectGL);
            m_gl.BindBuffer(targetGL, objectGL);
            m_gl.BufferData(targetGL, sizeInBytes, data, usageGL);

            bufferOut = new GPUBuffer_OpenGL21(type, usage, cpuAccessFlags, objectGL, targetGL, usageGL);


            // Re-bind the previous buffer.
            m_gl.BindBuffer(targetGL, prevObjectGL);


            return 0;   // No error.
        }

        void GPURenderingDevice_OpenGL21::DeleteBuffer(GPUBuffer* buffer)
        {
            auto bufferGL = reinterpret_cast<GPUBuffer_OpenGL21*>(buffer);
            assert(bufferGL != nullptr);
            {
                GLuint objectGL = bufferGL->GetOpenGLObject();
                m_gl.DeleteBuffers(1, &objectGL);

                delete buffer;
            }
        }

        ResultCode GPURenderingDevice_OpenGL21::MapBuffer(GPUBuffer* buffer, GPUBufferMapType mapType, void* &dataOut)
        {
            auto bufferGL = reinterpret_cast<GPUBuffer_OpenGL21*>(buffer);
            assert(bufferGL != nullptr);
            {
                if (bufferGL->GetBufferUsage() != GPUBufferUsage_Immutable)
                {
                    const GLenum accessGLTable[] = {
                        0,
                        GL_READ_ONLY,       // GPUBufferMapType_Read
                        GL_WRITE_ONLY,      // GPUBufferMapType_Write
                        GL_READ_WRITE,      // GPUBufferMapType_ReadWrite
                        GL_WRITE_ONLY,      // GPUBufferMapType_Write_Discard
                        GL_WRITE_ONLY       // GPUBufferMapType_Write_NoOverwrite
                    };


                    GLuint prevObjectGL;
                    m_gl.GetIntegerv((bufferGL->GetOpenGLTarget() == GL_ARRAY_BUFFER) ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&prevObjectGL));


                    m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), bufferGL->GetOpenGLObject());
                    dataOut = m_gl.MapBuffer(bufferGL->GetOpenGLTarget(), accessGLTable[mapType]);


                    // Re-bind the previous buffer.
                    m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), prevObjectGL);


                    if (dataOut != nullptr)
                    {
                        return FailedToMapGPUBuffer;
                    }
                    else
                    {
                        return 0;   // No error.
                    }
                }
                else
                {
                    dataOut = nullptr;
                    return GPUBufferIsImmutable;
                }
            }
        }

        void GPURenderingDevice_OpenGL21::UnmapBuffer(GPUBuffer* buffer)
        {
            auto bufferGL = reinterpret_cast<GPUBuffer_OpenGL21*>(buffer);
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

        ResultCode GPURenderingDevice_OpenGL21::SetBufferData(GPUBuffer* buffer, size_t offsetInBytes, size_t sizeInBytes, const void* data)
        {
            auto bufferGL = reinterpret_cast<GPUBuffer_OpenGL21*>(buffer);
            assert(bufferGL != nullptr);
            {
                if (bufferGL->GetBufferUsage() != GPUBufferUsage_Immutable)
                {
                    GLuint prevObjectGL;
                    m_gl.GetIntegerv((bufferGL->GetOpenGLTarget() == GL_ARRAY_BUFFER) ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&prevObjectGL));

                    m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), bufferGL->GetOpenGLObject());
                    m_gl.BufferSubData(bufferGL->GetOpenGLTarget(), offsetInBytes, sizeInBytes, data);

                    // Re-bind the previous buffer.
                    m_gl.BindBuffer(bufferGL->GetOpenGLTarget(), prevObjectGL);


					return 0;	// No errors.
                }
                else
                {
                    return GPUBufferIsImmutable;
                }
            }
        }



        ///////////////////////////////////////////
        // Framebuffers

        ResultCode GPURenderingDevice_OpenGL21::CreateFramebuffer(Framebuffer* &framebufferOut)
        {
            framebufferOut = nullptr;
            return -1;
        }


        //////////////////////////////////////////
        // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
        void GPURenderingDevice_OpenGL21::SwapBuffers()
        {
            if (m_currentDC != NULL)
            {
                ::SwapBuffers(m_currentDC);
            }
        }

        ResultCode GPURenderingDevice_OpenGL21::InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil)
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

        void GPURenderingDevice_OpenGL21::UninitWindowFramebuffer(HWND hWnd)
        {
            auto iFramebuffer = m_windowFramebuffers.Find(hWnd);
            if (iFramebuffer != nullptr)
            {
                m_windowFramebuffers.RemoveByIndex(iFramebuffer->index);
            }
        }

        ResultCode GPURenderingDevice_OpenGL21::SetCurrentWindow(HWND hWnd)
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
                    // The window can not be used as a render target.
                    return InvalidWindowRenderTarget;
                }
            }
            else
            {
                // No window was specified.
                if ((m_stateFlags & StageFlag_IsWindowFramebufferCurrent) != 0)
                {
                        
                }

                m_gl.MakeCurrent(NULL, NULL);

                m_currentHWND = NULL;
                m_currentDC   = NULL;
            }

            return 0;   // No error.
        }
#endif

#if defined(GT_PLATFORM_LINUX)
#endif



        //////////////////////////////////////////
        // Private

        void GPURenderingDevice_OpenGL21::UpdateSlotVertexAttributePointers(unsigned int slotIndex)
        {
            assert(slotIndex < GT_GE_MAX_VERTEX_BUFFER_SLOTS);


            auto inputLayoutGL = reinterpret_cast<GPUInputLayout_OpenGL21*>(m_currentInputLayout);
            if (inputLayoutGL != nullptr)
            {
                auto &slot = m_vertexBufferSlots[slotIndex];

                // We need to get the indices of the attributes that are tied to the buffer on the given slot.
                size_t attribStart;
                size_t attribEnd;
                inputLayoutGL->GetSlotAttributeRange(slotIndex, attribStart, attribEnd);


                auto bufferGL = reinterpret_cast<GPUBuffer_OpenGL21*>(slot.buffer);
                if (bufferGL != nullptr)
                {
                    for (size_t iAttrib = attribStart; iAttrib < attribEnd; ++iAttrib)
                    {
                        auto &attribGL = inputLayoutGL->GetAttribute(iAttrib);
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
                        {
                            m_gl.DisableVertexAttribArray(attribGL.attribLocation);
                        }
                    }
                }


                m_invalidVertexBufferSlots &= ~(1 << slotIndex);
            }
        }

        ResultCode GPURenderingDevice_OpenGL21::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
        {
            GLuint objectGL;
            ResultCode result = this->CompileShader_GLSL(source, sourceLength, defines, target, messagesOut, objectGL);
            if (GT::Succeeded(result))
            {
                // The shader compilation was successful. We now need to build the byte code data. The OpenGL 2.1 API does not support loading shaders from binary data, so we can only output
                // the original shader source and the defines and target that was used to build it.
                result = this->CreateShaderBinaryData(source, sourceLength, defines, target, nullptr, 0, 3, byteCodeOut);


                // The shader object needs to be deleted at this pointer or otherwise it will leak.
                m_gl.DeleteShader(objectGL);
            }

            return result;
        }

        ResultCode GPURenderingDevice_OpenGL21::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &messagesOut, GLuint &objectGLOut)
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

        ResultCode GPURenderingDevice_OpenGL21::CompileShader_ARB(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
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
}

#endif
