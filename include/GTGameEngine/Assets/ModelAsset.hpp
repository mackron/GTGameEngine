// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_ModelAsset_hpp_
#define __GT_ModelAsset_hpp_

#include "Asset.hpp"
#include "../Rendering/RenderingTypes.hpp"

namespace GT
{
    /// Structure representing the layout of a vertex attribute in a model asset.
    struct VertexAttribLayout
    {
        GT::VertexAttribFormat attributeComponentType;      // Float, Signed Int, etc.
        unsigned int           attributeComponentCount;     // float = 1, float2 = 2, etc.
        unsigned int           attributeOffset;             // The attribute's offset.
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


        /// Retrieves a pointer to the vertex data.
        ///
        /// @return A pointer to the vertex data.
        virtual const void* GetVertexData() const = 0;

        /// Retrieves the size of the buffer containing the vertex data.
        ///
        /// @return The size of the vertex data buffer.
        virtual size_t GetVertexDataSize() const = 0;

        /// Retrieves the stride of each vertex.
        ///
        /// @return The vertex stride.
        virtual size_t GetVertexStride() const = 0;

        /// Retrieves a pointer to the buffer that contains the layout of each vertex attribute.
        ///
        /// @param attribCountOut [out] A reference to the variable that receive the number of vertex attributes in the array.
        ///
        /// @return A pointer to the buffer that contains the layout of each vertex attribute.
        virtual const VertexAttribLayout* GetVertexLayout(unsigned int &attribCountOut) const = 0;


        /// Retrieves a pointer to the buffer containing the index data.
        ///
        /// @return A pointer to the buffer containing the index data.
        virtual const void* GetIndexData() const = 0;

        /// Retrieves the size of the buffer containing the index data.
        ///
        /// @return The size of the buffer containing the index data.
        virtual size_t GetIndexDataSize() const = 0;

        /// Retrieves the index count.
        ///
        /// @return The index count.
        virtual unsigned int GetIndexCount() const = 0;

        /// Retrieves the index format.
        ///
        /// @return The index format.
        virtual IndexFormat GetIndexFormat() const = 0;
    };
}

#endif
