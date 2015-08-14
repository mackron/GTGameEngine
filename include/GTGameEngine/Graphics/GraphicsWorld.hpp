// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsWorld_hpp_
#define __GT_GraphicsWorld_hpp_

#include "GraphicsTypes.hpp"
#include "GraphicsWorldObject.hpp"
#include "../Math.hpp"
#include "../AlignedType.hpp"
#include "../GUI/GUIContext.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

namespace GT
{
    // Handle types.
    typedef uintptr_t HGraphicsResource;
    typedef uintptr_t HGraphicsObject;
    typedef uintptr_t HGraphicsRenderTarget;

    // Flags
    static const uint32_t GraphicsRenderTarget_OnlyUI = (1 << 0);       //< Set on a render target to indicate that it should only render UI elements and not any world objects.


    /// The different resource types.
    enum class GraphicsResourceType
    {
        Texture,
        Material,
        Mesh,
    };

    /// The different object types.
    enum class GraphicsObjectType
    {
        Mesh,
        PointLight,
        SpotLight,
        AmbientLight,
    };


    /// The different types for material variables. Some of these are invalid in certain contexts, which are specific to the underlying rendering API. All
    /// of these match up with easymtl_type.
    enum class GraphicsMaterialVariableType
    {
        Unknown     = 0,

        Float       = 1,
        Float2      = 2,
        Float3      = 3,
        Float4      = 4,

        Integer     = 5,
        Integer2    = 6,
        Integer3    = 7,
        Integer4    = 8,

        Texture1D   = 9,
        Texture2D   = 10,
        Texture3D   = 11,
        TextureCube = 12,

        Boolean     = 13,
    };


    /// Structure describing a texture resource.
    struct GraphicsTextureResourceDesc
    {
        GraphicsTextureResourceDesc()
            : width(1), height(1), depth(1), format(TextureFormat_RGBA8), pData(nullptr)
        {
        }


        /// The width of the texture.
        unsigned int width;

        /// The height of the texture.
        unsigned int height;

        /// The depth of the texture.
        unsigned int depth;

        /// The format of the texture data.
        TextureFormat format;

        /// A pointer to the raw texture data.
        const void* pData;
    };

    /// Structure describing a material resource.
    struct GraphicsMaterialResourceDesc
    {
        GraphicsMaterialResourceDesc()
            : pData(nullptr), dataSizeInBytes(0)
        {
        }

        /// A pointer to the raw material data.
        const void* pData;

        /// The size of the raw material data, in bytes.
        unsigned int dataSizeInBytes;
    };

    /// Structure describing a static mesh resource.
    struct GraphicsMeshResourceDesc
    {
        GraphicsMeshResourceDesc()
            : topology(PrimitiveTopologyType_Triangle),
              pVertexData(nullptr),
              vertexDataSize(0),
              vertexStride(0),
              pVertexLayout(nullptr),
              vertexAttribCount(0),
              pIndexData(nullptr),
              indexDataSize(0),
              indexCount(0),
              indexFormat(IndexFormat_UInt32),
              materials(nullptr),
              materialIndexOffsetCountPairs(nullptr),
              materialCount(0)
        {
        }


        /// The primitive topology (points, lines, triangles or patch)
        PrimitiveTopologyType topology;


        /// A pointer to the vertex data.
        const void* pVertexData;

        /// The size of the vertex data in bytes.
        size_t vertexDataSize;
        
        /// The size in bytes of a single vertex.
        size_t vertexStride;

        /// A pointer to the vertex attribute layout info.
        const VertexAttribLayout* pVertexLayout;

        /// The number of vertex attributes. This defines how many items are in pVertexLayout.
        unsigned int vertexAttribCount;


        /// A pointer to the index data.
        const void* pIndexData;

        /// The size in bytes of the index data.
        size_t indexDataSize;

        /// The number of indices in the index buffer.
        unsigned int indexCount;

        /// The format of the index data.
        IndexFormat indexFormat;


        /// The list of material resources to use for each material slot.
        HGraphicsResource* materials;

        /// A pointer to the list of integers that define the starting index for each material.
        const uint32_t* materialIndexOffsetCountPairs;

        /// The number of materials.
        unsigned int materialCount;
    };


    /// Base class for a graphics world. A graphics world is where rendering occurs.
    ///
    /// The actual rendering and resource creation is performed by derived classes. 
    class alignas(16) GraphicsWorld : public SIMDAlignedType
    {
    public:

        /// Constructor.
        GraphicsWorld(GUIContext &gui);

        /// Destructor.
        virtual ~GraphicsWorld();


        /// Retrieves a reference to the GUI context that is used for drawing GUI elements.
        GUIContext & GetGUI() const;



        ////////////////////////////////////////////
        // Virtual Methods

        /// Initializes the world.
        ///
        /// @return True if the world is initialized successfully; false otherwise.
        virtual bool Startup() = 0;

        /// Shuts down the world.
        virtual void Shutdown() = 0;



        ////////////////////
        // Resources

        /// Creates a texture resource.
        ///
        /// @remarks
        ///     If the texture is going to be used as a render target, set \c pData to null which will cause the texture data to be
        ///     allocated internally, but it's specific contents will be left undefined (until it has been rendered).
        virtual HGraphicsResource CreateTextureResource(const GraphicsTextureResourceDesc &textureDesc) = 0;


        /// Creates a material resource.
        ///
        /// @remarks
        ///     Materials are immutable which means however they are defined by this method is how the material is defined for it's life. If a
        ///     change is required, the material must be deleted and re-created.
        virtual HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc) = 0;

        /// Sets the value of a material resource's input variable.
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture) = 0;



        /// Creates a mesh resource.
        virtual HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc) = 0;

        /// Sets the default material to use for the given material slot on the given mesh.
        virtual void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource) = 0;


        /// Deletes the given resource.
        ///
        /// @remarks
        ///     Use this to delete a resource of any type.
        virtual void DeleteResource(HGraphicsResource hResource) = 0;



        ////////////////////
        // Objects

        /// Sets the transformation of the given object.
        ///
        /// @param hObject [in] The object whose transformation is being set.
        virtual void SetObjectTransform(HGraphicsObject hObject, const vec4 &position, const quat &rotation, const vec4 &scale) = 0;

        /// Sets the position of the given object.
        virtual void SetObjectPosition(HGraphicsObject hObject, const vec4 &position) = 0;

        /// Sets the rotation of the given object.
        virtual void SetObjectRotation(HGraphicsObject hObject, const quat &rotation) = 0;

        /// Sets the scale of the given object.
        virtual void SetObjectScale(HGraphicsObject hObject, const vec4 &scale) = 0;


        /// Creates a mesh object.
        virtual HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position = vec4(0, 0, 0, 1), const quat &rotation = quat::identity, const vec4 &scale = vec4(1, 1, 1, 1)) = 0;

        /// Sets the material to use with the given mesh resource.
        virtual void SetMeshObjectMaterial(HGraphicsObject hMeshObject, unsigned int materialSlot, HGraphicsResource hMaterialResource) = 0;

        /// Sets the value of a material input variable for the current mesh object.
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w) = 0;
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture) = 0;


        /// Deletes the given object.
        ///
        /// @remarks
        ///     Use this to delete an object of any type.
        virtual void DeleteObject(HGraphicsObject hObject) = 0;



        ////////////////////
        // Render Targets

#if defined(GT_PLATFORM_WINDOWS)
        /// Creates a render target where the result is output to the given window.
        virtual HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags = 0) = 0;

        /// Retrieves the render target for the given window, if any.
        ///
        /// @remarks
        ///     If the given window does not have a render target associated with it, 0 will be returned.
        virtual HGraphicsRenderTarget GetRenderTargetByWindow(HWND hWnd) const = 0;
#endif

        /// Creates a render target where the result is output to the given texture.
        virtual HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags = 0) = 0;

        /// Deletes the given render target.
        virtual void DeleteRenderTarget(HGraphicsRenderTarget hRT) = 0;


        /// Sets the viewport for the given render target.
        virtual void SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height) = 0;

        /// Retrieves the viewport for the given render target.
        virtual void GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const = 0;


        /// Sets the clear color for the given render target.
        virtual void SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GTLib::Colour &color) = 0;

        /// Enables clearing of the background color of the given render target.
        ///
        /// @remarks
        ///     This is enabled by default, but for scenes where the background is never seen (most FPS games, for example), it is
        ///     more efficient to disable this.
        virtual void EnableRenderTargetColorClearing(HGraphicsRenderTarget hRT) = 0;

        /// Disables clearing of the background color of the given render target.
        virtual void DisableRenderTargetColorClearing(HGraphicsRenderTarget hRT) = 0;


        /// Sets the priority of the given render target.
        ///
        /// @remarks
        ///     Texture render targets are always rendered before window render targets.
        ///     @par
        ///     Render targets have a default priority of 0.
        virtual void SetRenderTargetPriority(HGraphicsRenderTarget hRT, int priority) = 0;

        /// Retrieves the priority of the given render target.
        virtual int GetRenderTargetPriority(HGraphicsRenderTarget hRT) const = 0;


        /// Enables the given render target.
        ///
        /// @remarks
        ///     Render targets are enabled by default.
        virtual void EnableRenderTarget(HGraphicsRenderTarget hRT) = 0;

        /// Disables the given render target.
        ///
        /// @remarks
        ///     Render targets are enabled by default.
        virtual void DisableRenderTarget(HGraphicsRenderTarget hRT) = 0;

        /// Determines whether or not a render target is enabled.
        ///
        /// @remarks
        ///     Render targets are enabled by default.
        virtual bool IsRenderTargetEnabled(HGraphicsRenderTarget hRT) const = 0;


        /// Sets the projection and view matrices to use when rendering the given render target.
        virtual void SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const mat4 &projection, const mat4 &view) = 0;

        /// Retrieves the projection and view matrices to use when rendering the given render target.
        ///
        /// @remarks
        ///     If the render target is not valid, \c projectionOut and \c viewOut will be left unmodified.
        virtual void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const = 0;


        /// Sets the GUI surface that should be rendered to the given render target.
        ///
        /// @remarks
        ///     The surface must belong to the GUI that was passed to the constructor of the graphics world.
        ///     @par
        ///     Setting the surface to NULL or 0 will remove the surface from the render target. This is the default, and should
        ///     be done if the surface is deleted from the GUI.
        virtual void SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface) = 0;

        /// Retrieves the surface that is being rendered to the given render target.
        virtual HGUISurface GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const = 0;

        /// Sets the size of the given surface's GUI surface to that of the viewport of the given render target.
        ///
        /// @param hRT [in] The render target.
        ///
        /// @remarks
        ///     This is useful for updating the size of the surface when the size of the render target changes.
        virtual void SetRenderTargetSurfaceSizeToRenderTarget(HGraphicsRenderTarget hRT) = 0;



        ////////////////////
        // Rendering

        /// Determines whether or not command buffers are supported.
        virtual bool IsCommandBuffersSupported() const = 0;

        /// Builds the command buffers required for rendering the world.
        ///
        /// @remarks
        ///     This will fail if command buffers are not supported.
        virtual void BuildCommandBuffers() = 0;

        /// Executes the command buffers that were previous compiled with BuildCommandBuffers().
        ///
        /// @remarks
        ///     This will fail if command buffers are not supported.
        virtual void ExecuteCommandBuffers() = 0;
        
        /// Executes the rendering commands required for rendering the world.
        ///
        /// @remarks
        ///     This should be used in single-threaded environments and when command buffers are not supported.
        ///     @par
        ///     This is equivalent to BuildCommandBuffers(), immediately followed by ExecuteCommandBuffers().
        virtual void ExecuteRenderingCommands() = 0;


        // GUI
        //
        // When rendering a render target, the last thing to be drawn will be the GUI surface. All of these rendering commands
        // will be routed to these functions.
        //
        // These are optional and do not need to be used if the host has an alternative way of doing rendering.

        /// @copydoc GUIRenderer::BeginPaintSurface()
        virtual void GUI_BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData);

        /// @copydoc GUIRenderer::EndPaintSurface()
        virtual void GUI_EndPaintSurface(GUIContext &gui);

        /// @copydoc GUIRenderer::Clear()
        virtual void GUI_Clear(GUIContext &gui);

        /// @copydoc GUIRenderer::Clear()
        virtual void GUI_Clear(GUIContext &gui, const GTLib::Rect<int> &rect);

        /// @copydoc GUIRenderer::DrawRectangle()
        virtual void GUI_DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc GUIRenderer::SetClippingRect()
        virtual void GUI_SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect);

        /// @copydoc GUIRenderer::CanDrawText()
        virtual bool GUI_CanDrawText(GUIContext &gui, HGUIFont hFont);

        /// @copydoc GUIRenderer::DrawText()
        virtual void GUI_DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc);

        /// @copydoc GUIRenderer::DrawRawImage()
        virtual void GUI_DrawRawImage(GUIContext &gui, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent);



    private:

        /// A reference to the GUI context.
        GUIContext &m_gui;

        /// Padding.
#if UINTPTR_MAX == UINT32_MAX
        uint32_t m_padding0;
        uint32_t m_padding1;
#endif


    private:    // No copying.
        GraphicsWorld(const GraphicsWorld &);
        GraphicsWorld & operator=(const GraphicsWorld &);
    };


#if 0
    class GraphicsInterface;

    /// Structure used for creating a mesh descriptor.
    struct GraphicsWorldMeshDesc
    {
        GraphicsWorldMeshDesc()
            : topology(PrimitiveTopologyType_Triangle),
              pVertexData(nullptr),
              vertexDataSize(0),
              vertexStride(0),
              pVertexLayout(nullptr),
              vertexAttribCount(0),
              pIndexData(nullptr),
              indexDataSize(0),
              indexCount(0),
              indexFormat(IndexFormat_UInt32)
        {
        }


        /// The primitive topology (points, lines, triangles or patch)
        PrimitiveTopologyType topology;


        /// A pointer to the vertex data.
        const void* pVertexData;

        /// The size of the vertex data in bytes.
        size_t vertexDataSize;
        
        /// The size in bytes of a single vertex.
        size_t vertexStride;

        /// A pointer to the vertex attribute layout info.
        const VertexAttribLayout* pVertexLayout;

        /// The number of vertex attributes. This defines how many items are in pVertexLayout.
        unsigned int vertexAttribCount;


        /// A pointer to the index data.
        const void* pIndexData;

        /// The size in bytes of the index data.
        size_t indexDataSize;

        /// The number of indices in the index buffer.
        unsigned int indexCount;

        /// The format of the index data.
        IndexFormat indexFormat;
    };


    /// Class representing the graphics world.
    class GraphicsWorld
    {
    public:

        /// Constructor.
        GraphicsWorld(/*GraphicsInterface &graphicsInterface*/);

        /// Destructor.
        ~GraphicsWorld();


        /// Starts up the graphics world.
        bool Startup();

        /// Shuts down the graphics world.
        void Shutdown();


        /// Renders the graphics world from the given perspective.
        ///
        /// @param projection   [in] The projection matrix.
        /// @param view         [in] The view matrix.
        /// @param hFramebuffer [in] The framebuffer to output the result to.
        /// @param viewport     [in] The viewport to draw to.
        ///
        /// @remarks
        ///     The result will output to the first render target of the given framebuffer.
        void Render(const mat4 &projection, const mat4 &view, HFramebuffer hFramebuffer, const GraphicsViewport &viewport);


        /// Creates a mesh descriptor.
        ///
        /// @param desc [in] A reference to the structure containing information about the mesh.
        ///
        /// @remarks
        ///     A mesh object is created from a mesh descriptor.
        HGraphicsWorldMeshDescriptor CreateMeshDescriptor(const GraphicsWorldMeshDesc &desc);

        /// Deletes a mesh descriptor.
        ///
        /// @param hDesc [in] A handle to the mesh descriptor to delete.
        void DeleteMeshDescriptor(HGraphicsWorldMeshDescriptor hDesc);


        /// Creates a mesh object.
        ///
        /// @param hDescriptor [in] A handle to the mesh descriptor which defines the mesh.
        /// @param position    [in] The initial position of the object.
        /// @param rotation    [in] The initial rotation of the object.
        /// @param scale       [in] The initial scale of the object.
        ///
        /// @return A pointer to the new mesh object.
        ///
        /// @remarks
        ///     The object will be made invisible by default.
        GraphicsWorldObject* CreateMesh(HGraphicsWorldMeshDescriptor hDescriptor, const vec4 &position = vec4(0.0f, 0.0f, 0.0f, 1.0f), const quat &rotation = quat::identity, const vec4 &scale = vec4(1.0f, 1.0f, 1.0f, 1.0f));


        /// Deletes an object.
        ///
        /// @param pObject [in] A pointer to the object to delete.
        void DeleteObject(GraphicsWorldObject* pObject);



    private:

        /// Structure representing a mesh descriptor.
        struct MeshDesc
        {
            /// The primitive topology.
            PrimitiveTopologyType topology;


            /// The vertex buffer.
            HBuffer hVertexBuffer;

            /// The index buffer.
            HBuffer hIndexBuffer;


            /// The vertex stride.
            size_t vertexStride;

            /// The index count.
            unsigned int indexCount;

            /// The index format.
            IndexFormat indexFormat;

            
            /// The input layout to use with this mesh. This is temp for now.
            HVSInputLayout inputLayout;
        };


        /// Structure representing the global block constant buffer.
        struct CBGlobalBlock
        {
            mat4 projection;
            mat4 view;
        };

        /// Structure representing the per-object block constant buffer.
        struct CBObjectBlock
        {
            mat4 model;
        };


    private:

        /// A reference to the graphics world.
        GraphicsInterface &m_graphicsInterface;


        /// The list of meshes in the world.
        GTLib::Vector<GraphicsWorldMesh*> m_meshes;


        /// The default vertex shader.
        HShader m_hVertexShader;

        /// The default fragment shader.
        HShader m_hFragmentShader;

        /// The temporary vertex input layout object. This will be deleted later.
        HVSInputLayout m_hVertexLayout;


        /// The constant buffer for the global block.
        HBuffer m_hCBGlobalBlock;

        /// The constant buffer for the per-object block.
        HBuffer m_hCBObjectBlock;


    private:    // No copying.
        GraphicsWorld(const GraphicsWorld &);
        GraphicsWorld & operator=(const GraphicsWorld &);
    };
#endif

}

#endif // !__GT_GraphicsWorld_hpp_
