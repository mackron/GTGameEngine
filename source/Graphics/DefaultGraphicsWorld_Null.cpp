// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/DefaultGraphicsWorld_Null.hpp>

namespace GT
{
    DefaultGraphicsWorld_Null::DefaultGraphicsWorld_Null(GUIContext &gui)
        : GraphicsWorld(gui)
    {
    }

    DefaultGraphicsWorld_Null::~DefaultGraphicsWorld_Null()
    {
    }


    bool DefaultGraphicsWorld_Null::Startup()
    {
        return false;
    }

    void DefaultGraphicsWorld_Null::Shutdown()
    {
    }



    ////////////////////
    // Resources

    HGraphicsResource DefaultGraphicsWorld_Null::CreateTextureResource(const GraphicsTextureResourceDesc &textureDesc)
    {
        (void)textureDesc;

        return 0;
    }


    HGraphicsResource DefaultGraphicsWorld_Null::CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc)
    {
        (void)materialDesc;

        return 0;
    }

    void DefaultGraphicsWorld_Null::SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture)
    {
        (void)hMaterialResource;
        (void)variableName;
        (void)hTexture;
    }


    HGraphicsResource DefaultGraphicsWorld_Null::CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc)
    {
        (void)meshDesc;

        return 0;
    }

    void DefaultGraphicsWorld_Null::SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource)
    {
        (void)hMeshResource;
        (void)materialSlot;
        (void)hMaterialResource;
    }


    void DefaultGraphicsWorld_Null::DeleteResource(HGraphicsResource hResource)
    {
        (void)hResource;
    }



    ////////////////////
    // Objects

    void DefaultGraphicsWorld_Null::SetObjectTransform(HGraphicsObject hObject, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        (void)hObject;
        (void)position;
        (void)rotation;
        (void)scale;
    }

    void DefaultGraphicsWorld_Null::SetObjectPosition(HGraphicsObject hObject, const vec4 &position)
    {
        (void)hObject;
        (void)position;
    }

    void DefaultGraphicsWorld_Null::SetObjectRotation(HGraphicsObject hObject, const quat &rotation)
    {
        (void)hObject;
        (void)rotation;
    }

    void DefaultGraphicsWorld_Null::SetObjectScale(HGraphicsObject hObject, const vec4 &scale)
    {
        (void)hObject;
        (void)scale;
    }

    HGraphicsObject DefaultGraphicsWorld_Null::CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        (void)hMeshResource;
        (void)position;
        (void)rotation;
        (void)scale;

        return 0;
    }

    void DefaultGraphicsWorld_Null::SetMeshObjectMaterial(HGraphicsObject hMeshObject, unsigned int materialSlot, HGraphicsResource hMaterialResource)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)hMaterialResource;
    }

    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
        (void)y;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
        (void)y;
        (void)z;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
        (void)y;
        (void)z;
        (void)w;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
        (void)y;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
        (void)y;
        (void)z;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)x;
        (void)y;
        (void)z;
        (void)w;
    }
    void DefaultGraphicsWorld_Null::SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture)
    {
        (void)hMeshObject;
        (void)materialSlot;
        (void)variableName;
        (void)hTexture;
    }


    void DefaultGraphicsWorld_Null::DeleteObject(HGraphicsObject hObject)
    {
        (void)hObject;
    }



    ////////////////////
    // Render Targets

#if defined(GT_PLATFORM_WINDOWS)
    HGraphicsRenderTarget DefaultGraphicsWorld_Null::CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags)
    {
        (void)hWnd;
        (void)flags;

        return 0;
    }

    HGraphicsRenderTarget DefaultGraphicsWorld_Null::GetRenderTargetByWindow(HWND hWnd) const
    {
        (void)hWnd;

        return 0;
    }
#endif

    HGraphicsRenderTarget DefaultGraphicsWorld_Null::CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags)
    {
        (void)hTextureResource;
        (void)flags;

        return 0;
    }

    void DefaultGraphicsWorld_Null::DeleteRenderTarget(HGraphicsResource hRT)
    {
        (void)hRT;
    }


    void DefaultGraphicsWorld_Null::SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height)
    {
        (void)hRT;
        (void)x;
        (void)y;
        (void)width;
        (void)height;
    }

    void DefaultGraphicsWorld_Null::GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const
    {
        (void)hRT;
        (void)xOut;
        (void)yOut;
        (void)widthOut;
        (void)heightOut;
    }


    void DefaultGraphicsWorld_Null::SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GTLib::Colour &color)
    {
        (void)hRT;
        (void)color;
    }

    void DefaultGraphicsWorld_Null::EnableRenderTargetColorClearing(HGraphicsRenderTarget hRT)
    {
        (void)hRT;
    }

    void DefaultGraphicsWorld_Null::DisableRenderTargetColorClearing(HGraphicsRenderTarget hRT)
    {
        (void)hRT;
    }


    void DefaultGraphicsWorld_Null::SetRenderTargetPriority(HGraphicsRenderTarget hRT, int priority)
    {
        (void)hRT;
        (void)priority;
    }

    int DefaultGraphicsWorld_Null::GetRenderTargetPriority(HGraphicsRenderTarget hRT) const
    {
        (void)hRT;

        return 0;
    }


    void DefaultGraphicsWorld_Null::EnableRenderTarget(HGraphicsRenderTarget hRT)
    {
        (void)hRT;
    }

    void DefaultGraphicsWorld_Null::DisableRenderTarget(HGraphicsRenderTarget hRT)
    {
        (void)hRT;
    }

    bool DefaultGraphicsWorld_Null::IsRenderTargetEnabled(HGraphicsRenderTarget hRT) const
    {
        (void)hRT;

        return false;
    }


    void DefaultGraphicsWorld_Null::SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const mat4 &projection, const mat4 &view)
    {
        (void)hRT;
        (void)projection;
        (void)view;
    }

    void DefaultGraphicsWorld_Null::GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const
    {
        (void)hRT;
        (void)projectionOut;
        (void)viewOut;
    }


    void DefaultGraphicsWorld_Null::SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface)
    {
        (void)hRT;
        (void)hSurface;
    }

    HGUISurface DefaultGraphicsWorld_Null::GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const
    {
        (void)hRT;

        return 0;
    }



    ////////////////////
    // Rendering

    bool DefaultGraphicsWorld_Null::IsCommandBuffersSupported() const
    {
        return false;
    }

    void DefaultGraphicsWorld_Null::BuildCommandBuffers()
    {
    }

    void DefaultGraphicsWorld_Null::ExecuteCommandBuffers()
    {
    }
        
    void DefaultGraphicsWorld_Null::ExecuteRenderingCommands()
    {
    }
}
