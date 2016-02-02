// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ModelAsset_MD2
#define GT_ModelAsset_MD2

#include <GTGE/Config.hpp>

#if defined(GT_BUILD_MD2)
#include <GTGE/Assets/ModelAsset.hpp>
#include <GTGE/Core/Vector.hpp>

namespace GT
{
    struct MD2MeshVertex
    {
        float position[3];
        float texcoord[2];
        float normal[3];
    };


    /// MD2 implementation of the model asset.
    class ModelAsset_MD2 : public ModelAsset
    {
    public:

        /// Constructor.
        ModelAsset_MD2(const char* absolutePathOrIdentifier, AssetType assetType);

        /// Destructor.
        virtual ~ModelAsset_MD2();


        /// @copydoc ModelAsset::Load()
        bool Load(const char* absolutePath, drvfs_context* pContext);


        /// @copydoc ModelAsset::GetMeshCount()
        unsigned int GetMeshCount() const;


        ////////////////////////////////////////
        // Vertex Data

        /// @copydoc GetMeshVertexData()
        const void* GetMeshVertexData(unsigned int meshIndex) const;

        /// @copydoc GetMeshVertexDataSize()
        size_t GetMeshVertexDataSize(unsigned int meshIndex) const;

        /// @copydoc GetMeshVertexStride()
        size_t GetMeshVertexStride(unsigned int meshIndex) const;

        /// @copydoc GetMeshVertexLayout()
        const VertexAttribLayout* GetMeshVertexLayout(unsigned int meshIndex, unsigned int &attribCountOut) const;


        ////////////////////////////////////////
        // Index Data

        /// @copydoc GetMeshIndexData()
        const void* GetMeshIndexData(unsigned int meshIndex) const;

        /// @copydoc GetMeshIndexDataSize()
        size_t GetMeshIndexDataSize(unsigned int meshIndex) const;

        /// @copydoc GetMeshIndexCount()
        unsigned int GetMeshIndexCount(unsigned int meshIndex) const;

        /// @copydoc GetMeshIndexFormat()
        IndexFormat GetMeshIndexFormat(unsigned int meshIndex) const;


        ////////////////////////////////////////
        // Material Data

        /// @copydoc ModelAsset::GetMaterialCount()
        unsigned int GetMeshMaterialCount(unsigned int meshIndex) const;

        /// @copydoc ModelAsset::GetMaterialIndexOffsetCountPairs()
        const uint32_t* GetMeshMaterialIndexOffsetCountPairs(unsigned int meshIndex) const;

        /// @copydoc ModelAsset::GetMeshMaterialName()
        void GetMeshMaterialName(unsigned int meshIndex, unsigned int materialIndex, char* materialNameOut, unsigned int materialNameSizeInBytes) const;


        ////////////////////////////////////////
        // Animation Data

        /// @copydoc GetMeshAnimationType()
        MeshAnimationType GetMeshAnimationType(unsigned int meshIndex) const;

        /// @copydoc GetMeshVertexCountPerKeyframe()
        unsigned int GetMeshVertexCountPerKeyframe(unsigned int meshIndex) const;

        /// @copydoc GetMeshTotalKeyframeCount()
        unsigned int GetMeshTotalKeyframeCount(unsigned int meshIndex) const;

        /// @copydoc GetMeshAnimationSegmentCount()
        unsigned int GetMeshAnimationSegmentCount(unsigned int meshIndex) const;


    private:

        // There is only ever a single mesh in an MD2 file so we don't need to use a separate data structure in an array or anything.

        /// The vertex data of the mesh.
        MD2MeshVertex* m_vertexData;

        /// The total number of vertices making up the vertex data.
        unsigned int m_vertexCount;

        /// The number of vertices that make up a single frame.
        unsigned int m_vertexCountPerFrame;

        /// The vertex layout.
        VertexAttribLayout m_vertexLayout[3];


        /// The index data.
        unsigned int* m_indexData;

        /// The index count.
        unsigned int m_indexCount;


        /// The material offset/count pair. This is equal to 0/m_indexCount.
        uint32_t m_materialOffsetCountPair[2];


        /// The total number of keyframes.
        unsigned int m_keyframeCount;
    };
}

#endif // !GT_ModelAsset_MD2

#endif
