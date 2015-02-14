// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPURenderingDevice_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)
#include "GPUBuffer_OpenGL21.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

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
                return m_gl.Startup();
            }
            else
            {
                // The device is not the primary adapter or OpenGL 2.1 is not supported.
                return RenderingAPINotSupported;
            }
        }

        void GPURenderingDevice_OpenGL21::Shutdown()
        {
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
                GL_POINTS,              // PrimitiveTopology_Point
                GL_LINES,               // PrimitiveTopology_Line
                GL_LINE_STRIP,          // PrimitiveTopology_LineStrip
                GL_TRIANGLES,           // PrimitiveTopology_Triangle
                GL_TRIANGLE_STRIP       // PrimitiveTopology_TriangleStrip
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
    }
}

#endif
