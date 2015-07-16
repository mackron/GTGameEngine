// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/DefaultGraphicsWorld.hpp>
#include <GTGameEngine/Graphics/DefaultGraphicsWorld_Null.hpp>
#include <GTGameEngine/Graphics/DefaultGraphicsWorld_OpenGL.hpp>

namespace GT
{
    DefaultGraphicsWorld::DefaultGraphicsWorld(GUIContext &gui, GraphicsAPI &graphicsAPI)
        : GraphicsWorld(gui),
          m_pGraphicsWorldImpl(nullptr)
    {
        switch (graphicsAPI.GetType())
        {
#if defined(GT_BUILD_VULKAN)
        case GraphicsAPIType_Vulkan:
            {
                m_pGraphicsWorldImpl = new DefaultGraphicsWorld_Vulkan(gui, reinterpret_cast<GraphicsAPI_Vulkan &>(graphicsAPI));
                break;
            }
#endif
#if defined(GT_BUILD_D3D12)
        case GraphicsAPIType_D3D12:
            {
                m_pGraphicsWorldImpl = new DefaultGraphicsWorld_D3D12(gui, reinterpret_cast<GraphicsAPI_D3D12 &>(graphicsAPI));
                break;
            }
#endif
#if defined(GT_BUILD_OPENGL)
        case GraphicsAPIType_OpenGL:
            {
                m_pGraphicsWorldImpl = new DefaultGraphicsWorld_OpenGL(gui, reinterpret_cast<GraphicsAPI_OpenGL &>(graphicsAPI));
                break;
            }
#endif

        case GraphicsAPIType_Null:
        default:
            {
                m_pGraphicsWorldImpl = new DefaultGraphicsWorld_Null(gui);
                break;
            }
        }
    }

    DefaultGraphicsWorld::~DefaultGraphicsWorld()
    {
        delete m_pGraphicsWorldImpl;
    }


    bool DefaultGraphicsWorld::Startup()
    {
        return m_pGraphicsWorldImpl->Startup();
    }

    void DefaultGraphicsWorld::Shutdown()
    {
        return m_pGraphicsWorldImpl->Shutdown();
    }



    ////////////////////
    // Resources

    HGraphicsResource DefaultGraphicsWorld::CreateTextureResource(unsigned int width, unsigned int height, unsigned int depth, TextureFormat format, const void* pData)
    {
        return m_pGraphicsWorldImpl->CreateTextureResource(width, height, depth, format, pData);
    }

    HGraphicsResource DefaultGraphicsWorld::CreateMaterialResource()
    {
        return m_pGraphicsWorldImpl->CreateMaterialResource();
    }

    HGraphicsResource DefaultGraphicsWorld::CreateMeshResource()
    {
        return m_pGraphicsWorldImpl->CreateMeshResource();
    }

    void DefaultGraphicsWorld::DeleteResource(HGraphicsResource hResource)
    {
        m_pGraphicsWorldImpl->DeleteResource(hResource);
    }



    ////////////////////
    // Objects

    HGraphicsObject DefaultGraphicsWorld::CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        return m_pGraphicsWorldImpl->CreateMeshObject(hMeshResource, position, rotation, scale);
    }

    void DefaultGraphicsWorld::DeleteObject(HGraphicsObject hObject)
    {
        m_pGraphicsWorldImpl->DeleteObject(hObject);
    }



    ////////////////////
    // Render Targets

#if defined(GT_PLATFORM_WINDOWS)
    HGraphicsRenderTarget DefaultGraphicsWorld::CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags)
    {
        return m_pGraphicsWorldImpl->CreateRenderTargetFromWindow(hWnd, flags);
    }
#endif

    HGraphicsRenderTarget DefaultGraphicsWorld::CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags)
    {
        return m_pGraphicsWorldImpl->CreateRenderTargetFromTexture(hTextureResource, flags);
    }

    void DefaultGraphicsWorld::DeleteRenderTarget(HGraphicsResource hRT)
    {
        return m_pGraphicsWorldImpl->DeleteRenderTarget(hRT);
    }


    void DefaultGraphicsWorld::SetRenderTargetPriority(HGraphicsRenderTarget hRT, int priority)
    {
        m_pGraphicsWorldImpl->SetRenderTargetPriority(hRT, priority);
    }

    int DefaultGraphicsWorld::GetRenderTargetPriority(HGraphicsRenderTarget hRT) const
    {
        return m_pGraphicsWorldImpl->GetRenderTargetPriority(hRT);
    }


    void DefaultGraphicsWorld::EnableRenderTarget(HGraphicsRenderTarget hRT)
    {
        m_pGraphicsWorldImpl->EnableRenderTarget(hRT);
    }

    void DefaultGraphicsWorld::DisableRenderTarget(HGraphicsRenderTarget hRT)
    {
        m_pGraphicsWorldImpl->DisableRenderTarget(hRT);
    }

    bool DefaultGraphicsWorld::IsRenderTargetEnabled(HGraphicsRenderTarget hRT) const
    {
        return m_pGraphicsWorldImpl->IsRenderTargetEnabled(hRT);
    }


    void DefaultGraphicsWorld::SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const mat4 &projection, const mat4 &view)
    {
        m_pGraphicsWorldImpl->SetRenderTargetProjectionAndView(hRT, projection, view);
    }

    void DefaultGraphicsWorld::GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const
    {
        m_pGraphicsWorldImpl->GetRenderTargetProjectionAndView(hRT, projectionOut, viewOut);
    }


    void DefaultGraphicsWorld::SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface)
    {
        m_pGraphicsWorldImpl->SetRenderTargetGUISurface(hRT, hSurface);
    }

    HGUISurface DefaultGraphicsWorld::GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const
    {
        return m_pGraphicsWorldImpl->GetRenderTargetGUISurface(hRT);
    }



    ////////////////////
    // Rendering

    bool DefaultGraphicsWorld::IsCommandBuffersSupported() const
    {
        return m_pGraphicsWorldImpl->IsCommandBuffersSupported();
    }

    void DefaultGraphicsWorld::BuildCommandBuffers()
    {
        m_pGraphicsWorldImpl->BuildCommandBuffers();
    }

    void DefaultGraphicsWorld::ExecuteCommandBuffers()
    {
        m_pGraphicsWorldImpl->ExecuteCommandBuffers();
    }
        
    void DefaultGraphicsWorld::ExecuteRenderingCommands()
    {
        m_pGraphicsWorldImpl->ExecuteRenderingCommands();
    }
}
