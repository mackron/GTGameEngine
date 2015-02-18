// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)
#include "GPUBuffer_OpenGL21.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

#include <GTLib/String.hpp>

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
              m_stateFlags(0),
              m_currentTopologyGL(GL_TRIANGLES),
              m_currentVertexBuffer(nullptr),
              m_currentIndexBuffer(nullptr)
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
            if (m_currentVertexBuffer != nullptr && m_currentIndexBuffer != nullptr)
            {
                m_gl.DrawElements(m_currentTopologyGL, indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(startIndexLocation*sizeof(uint32_t)));
            }
        }

        

        ///////////////////////////////////////////
        // State

        void GPURenderingDevice_OpenGL21::SetPrimitiveTopology(PrimitiveTopology topology)
        {
            GLenum topologiesGL[] =
            {
                GL_POINTS,                          // PrimitiveTopology_Point
                GL_LINES,                           // PrimitiveTopology_Line
                GL_LINE_STRIP,                      // PrimitiveTopology_LineStrip
                GL_TRIANGLES,                       // PrimitiveTopology_Triangle
                GL_TRIANGLE_STRIP,                  // PrimitiveTopology_TriangleStrip
                GL_LINES,                           // PrimitiveTopology_Line_Adjacency (Not supported in OpenGL 2.1)
                GL_LINE_STRIP,                      // PrimitiveTopology_LineStrip_Adjacency (Not supported in OpenGL 2.1)
                GL_TRIANGLES,                       // PrimitiveTopology_Triangle_Adjacency (Not supported in OpenGL 2.1)
                GL_TRIANGLE_STRIP                   // PrimitiveTopology_TriangleStrip_Adjacency (Not supported in OpenGL 2.1)
            };

            m_currentTopologyGL = topologiesGL[topology];
        }

        void GPURenderingDevice_OpenGL21::SetCurrentVertexBuffer(GPUBuffer* buffer)
        {
            auto bufferGL = reinterpret_cast<GPUBuffer_OpenGL21*>(buffer);
            if (bufferGL != nullptr)
            {
                assert(bufferGL->GetOpenGLTarget() == GL_ARRAY_BUFFER);
                {
                    m_gl.BindBuffer(GL_ARRAY_BUFFER, bufferGL->GetOpenGLObject());
                }
            }
            else
            {
                m_gl.BindBuffer(GL_ARRAY_BUFFER, 0);
            }


            m_currentVertexBuffer = buffer;
        }

        void GPURenderingDevice_OpenGL21::SetCurrentIndexBuffer(GPUBuffer* buffer)
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

        void GPURenderingDevice_OpenGL21::SetCurrentConstantBuffer(GPUBuffer* buffer, unsigned int slot)
        {
            (void)buffer;
            (void)slot;

            // Unsupported with core OpenGL 2.1.
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



        ///////////////////////////////////////////
        // Buffers

        ResultCode GPURenderingDevice_OpenGL21::CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data, GPUBuffer* &bufferOut)
        {
            bufferOut = nullptr;

            if (usage == GPUBufferUsage_Immutable && data == nullptr)
            {
                return NoDataSpecifiedForImmutableBuffer;
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

        ResultCode GPURenderingDevice_OpenGL21::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut)
        {
            GLuint objectGL;
            ResultCode result = this->CompileShader_GLSL(source, sourceLength, defines, target, messagesOut, objectGL);
            if (GT::Succeeded(result))
            {
                // The shader compilation was successful. We now need to build the byte code data. The OpenGL 2.1 API does not support loading shaders from binary data, so we can only output
                // the original shader source and the defines and target that was used to build it.
                result = this->CreateBufferBinaryData(source, sourceLength, defines, target, nullptr, 0, 0, byteCodeOut);
            }

            return result;
        }

        ResultCode GPURenderingDevice_OpenGL21::CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &messagesOut, GLuint &objectGLOut)
        {
            objectGLOut = 0;

            const char* versionStrings[] =
            {
                "",                     // GPUShaderTarget_Unknown
                "#version 120",         // GPUShaderTarget_GLSL_120_VS
                "#version 120"          // GPUShaderTarget_GLSL_120_FS
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
                return FailedToCreateShaderObject;
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
