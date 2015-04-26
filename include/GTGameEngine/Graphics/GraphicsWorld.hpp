// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GraphicsWorld_hpp_
#define __GT_GraphicsWorld_hpp_

#include "GraphicsTypes.hpp"
#include "GraphicsWorldObject.hpp"
#include "../Math.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
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
        GraphicsWorld(GraphicsInterface &graphicsInterface);

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
        void Render(mat4 projection, mat4 view, HFramebuffer hFramebuffer, const GraphicsViewport &viewport);


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
        GraphicsWorldObject* CreateMesh(HGraphicsWorldMeshDescriptor hDescriptor, vec4 position = vec4(0.0f, 0.0f, 0.0f, 1.0f), quat rotation = quat::identity, vec4 scale = vec4(1.0f, 1.0f, 1.0f, 1.0f));


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
}

#endif // !__GT_GraphicsWorld_hpp_
