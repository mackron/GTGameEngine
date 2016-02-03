// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultGraphicsWorld_Null
#define GT_DefaultGraphicsWorld_Null

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
        DefaultGraphicsWorld_Null(drgui_context* pGUI);

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

        /// @copydoc GraphicsWorld::GetTextureSize()
        void GetTextureSize(HGraphicsResource hTextureResource, unsigned int &widthOut, unsigned int &heightOut, unsigned int &depthOut);

        /// @copydoc GraphicsWorld::GetTextureFormat()
        TextureFormat GetTextureFormat(HGraphicsResource hTextureResource);

        /// @copydoc GraphicsWorld::GetTextureData()
        bool GetTextureData(HGraphicsResource hTextureResource, void* pDataOut);


        /// @copydoc GraphicsWorld::CreateMaterialResource()
        HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc);

        /// @copydoc GraphicsWorld::SetMaterialResourceInputVariable()
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y, float z);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y, float z, float w);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y, int z);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y, int z, int w);
        void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::CreateMeshResource()
        HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterial()
        void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterialInputVariable()
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y, float z);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y, int z);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w);
        void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::DeleteResource()
        void DeleteResource(HGraphicsResource hResource);



        ////////////////////
        // Objects

        /// @copydoc GraphicsWorld::SetObjectTransform()
        void SetObjectTransform(HGraphicsObject hObject, const glm::vec4 &position, const glm::quat &rotation, const glm::vec4 &scale);

        /// @copydoc GraphicsWorld::SetObjectPosition()
        void SetObjectPosition(HGraphicsObject hObject, const glm::vec4 &position);

        /// @copydoc GraphicsWorld::SetObjectRotation()
        void SetObjectRotation(HGraphicsObject hObject, const glm::quat &rotation);

        /// @copydoc GraphicsWorld::SetObjectScale()
        void SetObjectScale(HGraphicsObject hObject, const glm::vec4 &scale);


        /// @copydoc GraphicsWorld::CreateMeshObject()
        HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const glm::vec4 &position = glm::vec4(0, 0, 0, 1), const glm::quat &rotation = glm::quat(), const glm::vec4 &scale = glm::vec4(1, 1, 1, 1));

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

#if defined(_WIN32)
        /// @copydoc GraphicsWorld::CreateRenderTargetFromWindow()
        HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags);

        /// @copydoc GraphicsWorld::GetRenderTargetByWindow()
        HGraphicsRenderTarget GetRenderTargetByWindow(HWND hWnd) const;
#endif

        /// @copydoc GraphicsWorld::CreateRenderTargetFromTexture()
        HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, AAType aaType, unsigned int aaQuality, uint32_t flags);

        /// @copydoc GraphicsWorld::DeleteRenderTarget()
        void DeleteRenderTarget(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetViewport()
        void SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height);

        /// @copydoc GraphicsWorld::GetRenderTargetViewport()
        void GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetClearColor()
        void SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GT::Color &color);

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
        void SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const glm::mat4 &projection, const glm::mat4 &view);

        /// @copydoc GraphicsWorld::SetRenderTargetProjection()
        void SetRenderTargetProjection(HGraphicsRenderTarget hRT, const glm::mat4 &projection);

        /// @copydoc GraphicsWorld::SetRenderTargetView()
        void SetRenderTargetView(HGraphicsRenderTarget hRT, const glm::mat4 &view);

        /// @copydoc GraphicsWorld::GetRenderTargetProjectionAndView()
        void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, glm::mat4 &projectionOut, glm::mat4 &viewOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetGUISurface()
        void SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, drgui_element* pGUIElement);

        /// @copydoc GraphicsWorld::GetRenderTargetGUISurface()
        drgui_element* GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const;

        /// @copydoc GraphicsWorld::SetRenderTargetSurfaceSizeToRenderTarget()
        void SetRenderTargetSurfaceSizeToRenderTarget(HGraphicsRenderTarget hRT);



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
    };
}

#endif
