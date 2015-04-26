// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_ModelAsset_hpp_
#define __GT_ModelAsset_hpp_

#include "Asset.hpp"
#include "../Graphics/GraphicsTypes.hpp"

namespace GT
{
    /// The different supported animation types for meshes.
    enum class MeshAnimationType
    {
        None,
        Keyframe,
        Skeletal
    };

    /// Base class for model assets.
    class ModelAsset : public Asset
    {
    public:

        /// Constructor.
        ModelAsset(AssetType assetType);

        /// Destructor.
        virtual ~ModelAsset();


        /// @copydoc Asset::GetClass()
        AssetClass GetClass() const;


        /// Retrieves the number of meshes that make up the model.
        ///
        /// @return The number of meshes that make up the model.
        virtual unsigned int GetMeshCount() const = 0;



        ////////////////////////////////////////
        // Vertex Data

        /// Retrieves a pointer to the vertex data.
        ///
        /// @return A pointer to the vertex data.
        virtual const void* GetMeshVertexData(unsigned int meshIndex) const = 0;

        /// Retrieves the size of the buffer containing the vertex data.
        ///
        /// @return The size of the vertex data buffer.
        virtual size_t GetMeshVertexDataSize(unsigned int meshIndex) const = 0;

        /// Retrieves the stride of each vertex.
        ///
        /// @return The vertex stride.
        virtual size_t GetMeshVertexStride(unsigned int meshIndex) const = 0;

        /// Retrieves a pointer to the buffer that contains the layout of each vertex attribute.
        ///
        /// @param attribCountOut [out] A reference to the variable that receive the number of vertex attributes in the array.
        ///
        /// @return A pointer to the buffer that contains the layout of each vertex attribute.
        virtual const VertexAttribLayout* GetMeshVertexLayout(unsigned int meshIndex, unsigned int &attribCountOut) const = 0;


        ////////////////////////////////////////
        // Index Data

        /// Retrieves a pointer to the buffer containing the index data.
        ///
        /// @return A pointer to the buffer containing the index data.
        virtual const void* GetMeshIndexData(unsigned int meshIndex) const = 0;

        /// Retrieves the size of the buffer containing the index data.
        ///
        /// @return The size of the buffer containing the index data.
        virtual size_t GetMeshIndexDataSize(unsigned int meshIndex) const = 0;

        /// Retrieves the index count.
        ///
        /// @return The index count.
        virtual unsigned int GetMeshIndexCount(unsigned int meshIndex) const = 0;

        /// Retrieves the index format.
        ///
        /// @return The index format.
        virtual IndexFormat GetMeshIndexFormat(unsigned int meshIndex) const = 0;


        ////////////////////////////////////////
        // Animation Data

        /// Retrieves the animation type for the mesh at the given index.
        ///
        /// @param meshIndex [in] The index of the mesh whose animation type is being retrieved.
        ///
        /// @return The mesh animation type.
        virtual MeshAnimationType GetMeshAnimationType(unsigned int meshIndex) const = 0;

        /// Retrieves the number of vertices per keyframe for the mesh at the given index.
        ///
        /// @param meshIndex [in] The index of the mesh whose animation type is being retrieved.
        ///
        /// @return The number of vertices per key frame.
        ///
        /// @remarks
        ///     This should only ever be needed for Keyframe animation types.
        virtual unsigned int GetMeshVertexCountPerKeyframe(unsigned int meshIndex) const = 0;

        /// Retrieves the total number of keyframes.
        ///
        /// @param meshIndex [in] The index of the mesh whose animation key frame count is being retrieved.
        ///
        /// @return The total number of keyframes making up all of the keyframe animations.
        virtual unsigned int GetMeshTotalKeyframeCount(unsigned int meshIndex) const = 0;

        /// Retrieves the total numer of keyframe animation segments.
        ///
        /// @param meshIndex [in] The index of the mesh whose animation key frame count is being retrieved.
        ///
        /// @return The number of animation segments, or 0 if the mesh is not animated.
        virtual unsigned int GetMeshAnimationSegmentCount(unsigned int meshIndex) const = 0;
    };
}

#endif
