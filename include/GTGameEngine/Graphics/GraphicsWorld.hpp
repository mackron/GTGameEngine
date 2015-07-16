// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsWorld_hpp_
#define __GT_GraphicsWorld_hpp_

#include "GraphicsTypes.hpp"
#include "GraphicsWorldObject.hpp"
#include "../Math.hpp"
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


    /// Base class for a graphics world. A graphics world is where rendering occurs.
    ///
    /// The actual rendering and resource creation is performed by derived classes. 
    class GraphicsWorld
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
        virtual HGraphicsResource CreateTextureResource(unsigned int width, unsigned int height, unsigned int depth, TextureFormat format, const void* pData) = 0;

        /// Creates a material resource.
        ///
        /// @remarks
        ///     Materials are immutable which means however they are defined by this method is how the material is defined for it's life. If a
        ///     change is required, the material must be deleted and re-created.
        virtual HGraphicsResource CreateMaterialResource() = 0;

        /// Creates a mesh resource.
        virtual HGraphicsResource CreateMeshResource() = 0;


        /// Deletes the given resource.
        ///
        /// @remarks
        ///     Use this to delete a resource of any type.
        virtual void DeleteResource(HGraphicsResource hResource) = 0;



        ////////////////////
        // Objects

        /// Creates a mesh object.
        virtual HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position = vec4(0, 0, 0, 1), const quat &rotation = quat::identity, const vec4 &scale = vec4(1, 1, 1, 1)) = 0;

        /// Deletes the given object.
        ///
        /// @remarks
        ///     Use this to delete an object of any type.
        virtual void DeleteObject(HGraphicsObject hObject) = 0;



        ////////////////////
        // Render Targets

        /// Creates a render target where the result is output to the given window.
#if defined(GT_PLATFORM_WINDOWS)
        virtual HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags = 0) = 0;
#endif

        /// Creates a render target where the result is output to the given texture.
        virtual HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags = 0) = 0;

        /// Deletes the given render target.
        virtual void DeleteRenderTarget(HGraphicsRenderTarget hRT) = 0;


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
        GUIContext &m_gui;


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
