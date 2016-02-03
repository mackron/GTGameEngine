// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GraphicsWorld
#define GT_GraphicsWorld

#include "GraphicsTypes.hpp"
#include "GraphicsWorldObject.hpp"
#include "../Math.hpp"
#include "../AlignedType.hpp"
#include "../Core/Colour.hpp"
#include <dr_libs/dr_gui.h>

#if defined(_WIN32)
#include <GTGE/Core/windows.hpp>
#endif

namespace GT
{
    // Handle types.
    typedef uintptr_t HGraphicsResource;
    typedef uintptr_t HGraphicsObject;
    typedef uintptr_t HGraphicsRenderTarget;

    // Flags
    static const uint32_t GraphicsRenderTarget_OnlyUI = (1 << 0);       ///< Set on a render target to indicate that it should only render UI elements and not any world objects.


    /// The different resource types.
    enum class GraphicsResourceType
    {
        Texture,
        Material,
        Mesh
    };

    /// The different object types.
    enum class GraphicsObjectType
    {
        Mesh//,
        //PointLight,
        //SpotLight,
        //AmbientLight
    };


    /// Structure describing a texture resource.
    struct GraphicsTextureResourceDesc
    {
        GraphicsTextureResourceDesc()
            : width(1), height(1), depth(1), format(TextureFormat_RGBA8), samplesMSAA(0), pData(nullptr)
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

        /// The number of samples to use with this texture for multisample anti-aliasing. Set to 0 or 1 to not use multisampling. If multisampling is requested,
        /// but the underlying rendering API does not support it, texture creation will fail.
        unsigned int samplesMSAA;

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
        GraphicsWorld(drgui_context* pGUI);

        /// Destructor.
        virtual ~GraphicsWorld();


        /// Retrieves a reference to the GUI context that is used for drawing GUI elements.
        drgui_context* GetGUI() const;



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

        /// Retrieves the dimensions of the given texture.
        ///
        /// @param hTextureResource [in] A handle to the texture resource whose dimensions are being retrieved.
        virtual void GetTextureSize(HGraphicsResource hTextureResource, unsigned int &widthOut, unsigned int &heightOut, unsigned int &depthOut) = 0;

        /// Retrieves the format of the given texture's data.
        ///
        /// @param hTextureResource [in] A handle to the texture resource whose data format is being retrieved.
        virtual TextureFormat GetTextureFormat(HGraphicsResource hTextureResource) = 0;

        /// Retrieves the data of the given texture resource.
        ///
        /// @param hTextureResource [in] A handle to the texture resource whose data is being retrieved.
        virtual bool GetTextureData(HGraphicsResource hTextureResource, void* pDataOut) = 0;


        /// Creates a material resource.
        ///
        /// @remarks
        ///     Materials are immutable which means however they are defined by this method is how the material is defined for it's life. If a
        ///     change is required, the material must be deleted and re-created.
        virtual HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc) = 0;

        /// Sets the value of a material resource's input variable.
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y, float z) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, float x, float y, float z, float w) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y, int z) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, int x, int y, int z, int w) = 0;
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture) = 0;



        /// Creates a mesh resource.
        virtual HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc) = 0;

        /// Sets the default material to use for the given material slot on the given mesh.
        virtual void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource) = 0;

        /// Sets the value of a mesh resource's material input variable.
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y, float z) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y, int z) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w) = 0;
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture) = 0;


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
        virtual void SetObjectTransform(HGraphicsObject hObject, const glm::vec4 &position, const glm::quat &rotation, const glm::vec4 &scale) = 0;

        /// Sets the position of the given object.
        virtual void SetObjectPosition(HGraphicsObject hObject, const glm::vec4 &position) = 0;

        /// Sets the rotation of the given object.
        virtual void SetObjectRotation(HGraphicsObject hObject, const glm::quat &rotation) = 0;

        /// Sets the scale of the given object.
        virtual void SetObjectScale(HGraphicsObject hObject, const glm::vec4 &scale) = 0;


        /// Creates a mesh object.
        virtual HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const glm::vec4 &position = glm::vec4(0, 0, 0, 1), const glm::quat &rotation = glm::quat(), const glm::vec4 &scale = glm::vec4(1, 1, 1, 1)) = 0;

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

#if defined(_WIN32)
        /// Creates a render target where the result is output to the given window.
        virtual HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags = 0) = 0;

        /// Retrieves the render target for the given window, if any.
        ///
        /// @remarks
        ///     If the given window does not have a render target associated with it, 0 will be returned.
        virtual HGraphicsRenderTarget GetRenderTargetByWindow(HWND hWnd) const = 0;
#endif

        /// Creates a render target where the result is output to the given texture.
        virtual HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, AAType aaType, unsigned int aaQuality, uint32_t flags = 0) = 0;

        /// Deletes the given render target.
        virtual void DeleteRenderTarget(HGraphicsRenderTarget hRT) = 0;


        /// Sets the viewport for the given render target.
        virtual void SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height) = 0;

        /// Retrieves the viewport for the given render target.
        virtual void GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const = 0;


        /// Sets the clear color for the given render target.
        virtual void SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GT::Color &color) = 0;

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
        virtual void SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const glm::mat4 &projection, const glm::mat4 &view) = 0;

        /// Sets the projection matrix to use when rendering the given render target.
        virtual void SetRenderTargetProjection(HGraphicsRenderTarget hRT, const glm::mat4 &projection) = 0;

        /// Sets the view matrix to use when rendering the given render target.
        virtual void SetRenderTargetView(HGraphicsRenderTarget hRT, const glm::mat4 &view) = 0;

        /// Retrieves the projection and view matrices to use when rendering the given render target.
        ///
        /// @remarks
        ///     If the render target is not valid, \c projectionOut and \c viewOut will be left unmodified.
        virtual void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, glm::mat4 &projectionOut, glm::mat4 &viewOut) const = 0;


        /// Sets the GUI surface that should be rendered to the given render target.
        ///
        /// @remarks
        ///     The surface must belong to the GUI that was passed to the constructor of the graphics world.
        ///     @par
        ///     Setting the surface to NULL or 0 will remove the surface from the render target. This is the default, and should
        ///     be done if the surface is deleted from the GUI.
        virtual void SetRenderTargetGUIElement(HGraphicsRenderTarget hRT, drgui_element* pElement) = 0;

        /// Retrieves the surface that is being rendered to the given render target.
        virtual drgui_element* GetRenderTargetGUIElement(HGraphicsRenderTarget hRT) const = 0;

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


    private:

        /// A reference to the GUI context.
        drgui_context* m_pGUI;

        /// Padding.
#if UINTPTR_MAX == UINT32_MAX
        uint32_t m_padding0;
        uint32_t m_padding1;
#endif


    private:    // No copying.
        GraphicsWorld(const GraphicsWorld &);
        GraphicsWorld & operator=(const GraphicsWorld &);
    };
}

#endif // !__GT_GraphicsWorld_hpp_
