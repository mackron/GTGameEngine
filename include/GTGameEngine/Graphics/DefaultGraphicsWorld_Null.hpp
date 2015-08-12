// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_DefaultGraphicsWorld_Null_hpp_
#define __GT_DefaultGraphicsWorld_Null_hpp_

#include "GraphicsWorld.hpp"

namespace GT
{
    // An implementation of the default graphics world that does not do anything.
    //
    // This should only be used as a fallback in cases when no rendering APIs are supported so that a crash can be prevented. In the
    // future this may be replaced with a software implementation.
    class DefaultGraphicsWorld_Null : public GraphicsWorld
    {
    public:

        /// Constructor.
        DefaultGraphicsWorld_Null(GUIContext &gui);

        /// Destructor.
        ~DefaultGraphicsWorld_Null();


        ////////////////////////////////////////////
        // Virtual Methods

        /// @copydoc GraphicsWorld::Startup()
        bool Startup();

        /// @copydoc GraphicsWorld::Shutdown()
        void Shutdown();



        ////////////////////
        // Resources

        /// @copydoc GraphicsWorld::CreateTextureResource()
        HGraphicsResource CreateTextureResource(const GraphicsTextureResourceDesc &textureDesc);


        /// @copydoc GraphicsWorld::CreateMaterialResource()
        HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc);

        /// @copydoc GraphicsWorld::SetMaterialResourceInputVariable()
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::CreateMeshResource()
        HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterial()
        void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource);


        /// @copydoc GraphicsWorld::DeleteResource()
        void DeleteResource(HGraphicsResource hResource);



        ////////////////////
        // Objects

        /// @copydoc GraphicsWorld::SetObjectTransform()
        void SetObjectTransform(HGraphicsObject hObject, const vec4 &position, const quat &rotation, const vec4 &scale);

        /// @copydoc GraphicsWorld::SetObjectPosition()
        void SetObjectPosition(HGraphicsObject hObject, const vec4 &position);

        /// @copydoc GraphicsWorld::SetObjectRotation()
        void SetObjectRotation(HGraphicsObject hObject, const quat &rotation);

        /// @copydoc GraphicsWorld::SetObjectScale()
        void SetObjectScale(HGraphicsObject hObject, const vec4 &scale);


        /// @copydoc GraphicsWorld::CreateMeshObject()
        HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position = vec4(0, 0, 0, 1), const quat &rotation = quat::identity, const vec4 &scale = vec4(1, 1, 1, 1));

        /// @copydoc GraphicsWorld::SetMeshObjectMaterial()
        void SetMeshObjectMaterial(HGraphicsObject hMeshObject, unsigned int materialSlot, HGraphicsResource hMaterialResource);

        /// @copydoc GraphicsWorld::SetMeshObjectMaterialInputVariable()
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w);
        void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::DeleteObject()
        void DeleteObject(HGraphicsObject);



        ////////////////////
        // Render Targets

#if defined(GT_PLATFORM_WINDOWS)
        /// @copydoc GraphicsWorld::CreateRenderTargetFromWindow()
        HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags);

        /// @copydoc GraphicsWorld::GetRenderTargetByWindow()
        HGraphicsRenderTarget GetRenderTargetByWindow(HWND hWnd) const;
#endif

        /// @copydoc GraphicsWorld::CreateRenderTargetFromTexture()
        HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags);

        /// @copydoc GraphicsWorld::DeleteRenderTarget()
        void DeleteRenderTarget(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetViewport()
        void SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height);

        /// @copydoc GraphicsWorld::GetRenderTargetViewport()
        void GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetClearColor()
        void SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GTLib::Colour &color);

        /// @copydoc GraphicsWorld::EnableRenderTargetColorClearing()
        void EnableRenderTargetColorClearing(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::DisableRenderTargetColorClearing()
        void DisableRenderTargetColorClearing(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetPriority()
        void SetRenderTargetPriority(HGraphicsRenderTarget hRT, int priority);

        /// @copydoc GraphicsWorld::GetRenderTargetPriority()
        int GetRenderTargetPriority(HGraphicsRenderTarget hRT) const;


        /// @copydoc GraphicsWorld::EnableRenderTarget()
        void EnableRenderTarget(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::DisableRenderTarget()
        void DisableRenderTarget(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::IsRenderTargetEnabled()
        bool IsRenderTargetEnabled(HGraphicsRenderTarget hRT) const;


        /// @copydoc GraphicsWorld::SetRenderTargetProjectionAndView()
        void SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const mat4 &projection, const mat4 &view);

        /// @copydoc GraphicsWorld::GetRenderTargetProjectionAndView()
        void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetGUISurface()
        void SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface);

        /// @copydoc GraphicsWorld::GetRenderTargetGUISurface()
        HGUISurface GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const;



        ////////////////////
        // Rendering

        /// @copydoc GraphicsWorld::IsCommandBuffersSupported()
        bool IsCommandBuffersSupported() const;

        /// @copydoc GraphicsWorld::BuildCommandBuffers()
        void BuildCommandBuffers();

        /// @copydoc GraphicsWorld::ExecuteCommandBuffers()
        void ExecuteCommandBuffers();
        
        /// @copydoc GraphicsWorld::ExecuteRenderingCommands()
        void ExecuteRenderingCommands();


        // GUI

        /// @copydoc DefaultGraphicsWorld::GUI_BeginPaintSurface()
        void GUI_BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData);

        /// @copydoc DefaultGraphicsWorld::GUI_EndPaintSurface()
        void GUI_EndPaintSurface(GUIContext &gui);

        /// @copydoc DefaultGraphicsWorld::Clear()
        void GUI_Clear(GUIContext &gui);

        /// @copydoc DefaultGraphicsWorld::Clear()
        void GUI_Clear(GUIContext &gui, const GTLib::Rect<int> &rect);

        /// @copydoc DefaultGraphicsWorld::DrawRectangle()
        void GUI_DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc DefaultGraphicsWorld::SetClippingRect()
        void GUI_SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect);

        /// @copydoc DefaultGraphicsWorld::CanDrawText()
        bool GUI_CanDrawText(GUIContext &gui, HGUIFont hFont);

        /// @copydoc DefaultGraphicsWorld::DrawText()
        void GUI_DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc);

        /// @copydoc GUIRenderer::DrawRawImage()
        void GUI_DrawRawImage(GUIContext &gui, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent);
    };
}

#endif
