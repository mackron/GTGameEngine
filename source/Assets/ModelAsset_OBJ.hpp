// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ModelAsset_OBJ
#define GT_ModelAsset_OBJ

#include <GTEngine/Config.hpp>

#if defined(GT_BUILD_OBJ)
#include <GTEngine/Assets/ModelAsset.hpp>

namespace GT
{
    struct OBJMeshVertex
    {
        float position[3];
        float texcoord[2];
        float normal[3];
    };

    /// Model asset for OBJ models.
    class ModelAsset_OBJ : public ModelAsset
    {
    public:

        /// Constructor.
        ModelAsset_OBJ(const char* absolutePathOrIdentifier, AssetType assetType);

        /// Destructor
        ~ModelAsset_OBJ();


        /// @copydoc ModelAsset::Load()
        bool Load(const char* absolutePath, easyvfs_context* pVFS);


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

        /// The vertex data of the mesh.
        OBJMeshVertex* m_vertexData;

        /// The total number of vertices making up the vertex data.
        unsigned int m_vertexCount;

        /// The vertex layout.
        VertexAttribLayout m_vertexLayout[3];


        /// The index data.
        unsigned int* m_indexData;

        /// The index count.
        unsigned int m_indexCount;


        /// The material offset/count pair. This is temp until support for multiple materials is added to the OBJ loader.
        uint32_t m_materialOffsetCountPair[2];
    };
}

#endif

#endif
