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
        if (m_pGraphicsWorldImpl != nullptr) {
            m_pGraphicsWorldImpl->Shutdown();
            delete m_pGraphicsWorldImpl;
        }
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

    HGraphicsResource DefaultGraphicsWorld::CreateTextureResource(const GraphicsTextureResourceDesc &textureDesc)
    {
        return m_pGraphicsWorldImpl->CreateTextureResource(textureDesc);
    }


    HGraphicsResource DefaultGraphicsWorld::CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc)
    {
        return m_pGraphicsWorldImpl->CreateMaterialResource(materialDesc);
    }

    void DefaultGraphicsWorld::SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture)
    {
        return m_pGraphicsWorldImpl->SetMaterialResourceInputVariable(hMaterialResource, variableName, hTexture);
    }


    HGraphicsResource DefaultGraphicsWorld::CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc)
    {
        return m_pGraphicsWorldImpl->CreateMeshResource(meshDesc);
    }

    void DefaultGraphicsWorld::SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource)
    {
        return m_pGraphicsWorldImpl->SetMeshResourceMaterial(hMeshResource, materialSlot, hMaterialResource);
    }

    void DefaultGraphicsWorld::SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture)
    {
        return m_pGraphicsWorldImpl->SetMeshResourceMaterialInputVariable(hMeshResource, materialSlot, variableName, hTexture);
    }


    void DefaultGraphicsWorld::DeleteResource(HGraphicsResource hResource)
    {
        m_pGraphicsWorldImpl->DeleteResource(hResource);
    }



    ////////////////////
    // Objects

    void DefaultGraphicsWorld::SetObjectTransform(HGraphicsObject hObject, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        m_pGraphicsWorldImpl->SetObjectTransform(hObject, position, rotation, scale);
    }

    void DefaultGraphicsWorld::SetObjectPosition(HGraphicsObject hObject, const vec4 &position)
    {
        m_pGraphicsWorldImpl->SetObjectPosition(hObject, position);
    }

    void DefaultGraphicsWorld::SetObjectRotation(HGraphicsObject hObject, const quat &rotation)
    {
        m_pGraphicsWorldImpl->SetObjectRotation(hObject, rotation);
    }

    void DefaultGraphicsWorld::SetObjectScale(HGraphicsObject hObject, const vec4 &scale)
    {
        m_pGraphicsWorldImpl->SetObjectScale(hObject, scale);
    }

    HGraphicsObject DefaultGraphicsWorld::CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        return m_pGraphicsWorldImpl->CreateMeshObject(hMeshResource, position, rotation, scale);
    }

    void DefaultGraphicsWorld::SetMeshObjectMaterial(HGraphicsObject hMeshObject, unsigned int materialSlot, HGraphicsResource hMaterialResource)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterial(hMeshObject, materialSlot, hMaterialResource);
    }

    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x, y);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x, y, z);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x, y, z, w);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x, y);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x, y, z);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, x, y, z, w);
    }
    void DefaultGraphicsWorld::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture)
    {
        m_pGraphicsWorldImpl->SetMeshObjectMaterialInputVariable(hMeshObject, materialSlot, variableName, hTexture);
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

    HGraphicsRenderTarget DefaultGraphicsWorld::GetRenderTargetByWindow(HWND hWnd) const
    {
        return m_pGraphicsWorldImpl->GetRenderTargetByWindow(hWnd);
    }
#endif

    HGraphicsRenderTarget DefaultGraphicsWorld::CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags)
    {
        return m_pGraphicsWorldImpl->CreateRenderTargetFromTexture(hTextureResource, flags);
    }

    void DefaultGraphicsWorld::DeleteRenderTarget(HGraphicsResource hRT)
    {
        m_pGraphicsWorldImpl->DeleteRenderTarget(hRT);
    }


    void DefaultGraphicsWorld::SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height)
    {
        m_pGraphicsWorldImpl->SetRenderTargetViewport(hRT, x, y, width, height);
    }

    void DefaultGraphicsWorld::GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const
    {
        m_pGraphicsWorldImpl->GetRenderTargetViewport(hRT, xOut, yOut, widthOut, heightOut);
    }


    void DefaultGraphicsWorld::SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GTLib::Colour &color)
    {
        m_pGraphicsWorldImpl->SetRenderTargetClearColor(hRT, color);
    }

    void DefaultGraphicsWorld::EnableRenderTargetColorClearing(HGraphicsRenderTarget hRT)
    {
        m_pGraphicsWorldImpl->EnableRenderTargetColorClearing(hRT);
    }

    void DefaultGraphicsWorld::DisableRenderTargetColorClearing(HGraphicsRenderTarget hRT)
    {
        m_pGraphicsWorldImpl->DisableRenderTargetColorClearing(hRT);
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

    void DefaultGraphicsWorld::SetRenderTargetSurfaceSizeToRenderTarget(HGraphicsRenderTarget hRT)
    {
        return m_pGraphicsWorldImpl->SetRenderTargetSurfaceSizeToRenderTarget(hRT);
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


    void DefaultGraphicsWorld::GUI_BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData)
    {
        m_pGraphicsWorldImpl->GUI_BeginPaintSurface(gui, hSurface, pInputData);
    }

    void DefaultGraphicsWorld::GUI_EndPaintSurface(GUIContext &gui)
    {
        m_pGraphicsWorldImpl->GUI_EndPaintSurface(gui);
    }

    void DefaultGraphicsWorld::GUI_Clear(GUIContext &gui)
    {
        m_pGraphicsWorldImpl->GUI_Clear(gui);
    }

    void DefaultGraphicsWorld::GUI_Clear(GUIContext &gui, const GTLib::Rect<int> &rect)
    {
        m_pGraphicsWorldImpl->GUI_Clear(gui, rect);
    }

    void DefaultGraphicsWorld::GUI_DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GTLib::Colour colour)
    {
        m_pGraphicsWorldImpl->GUI_DrawRectangle(gui, rect, colour);
    }

    void DefaultGraphicsWorld::GUI_SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect)
    {
        m_pGraphicsWorldImpl->GUI_SetClippingRect(gui, rect);
    }

    bool DefaultGraphicsWorld::GUI_CanDrawText(GUIContext &gui, HGUIFont hFont)
    {
        return m_pGraphicsWorldImpl->GUI_CanDrawText(gui, hFont);
    }

    void DefaultGraphicsWorld::GUI_DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc)
    {
        m_pGraphicsWorldImpl->GUI_DrawText(gui, textRunDesc);
    }

    void DefaultGraphicsWorld::GUI_DrawRawImage(GUIContext &gui, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent)
    {
        m_pGraphicsWorldImpl->GUI_DrawRawImage(gui, xPos, yPos, width, height, pImageData, isTransparent);
    }
}
