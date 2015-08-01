// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_AssetAllocator_hpp_
#define __GT_AssetAllocator_hpp_

#include "AssetTypes.hpp"

namespace GT
{
    class Asset;

    /// Base class for creating and deleting asset objects.
    class AssetAllocator
    {
    public:

        /// Constructor.
        AssetAllocator();

        /// Destructor.
        virtual ~AssetAllocator();


        /// Retrieves the asset type by the given file path.
        ///
        /// @param filePath [in] The path of the file being checked.
        ///
        /// @return The asset type for the given path. Returns AssetType_Unknown if the allocator does not support that asset at the given path.
        virtual AssetType GetAssetTypeByPath(const char* filePath) const;

        /// Determines whether or not the given asset type is supported by this allocator.
        ///
        /// @param assetType [in] The asset type to check support for.
        ///
        /// @return True if the given asset type is supported by this allocator.
        virtual bool IsAssetTypeSupported(AssetType assetType) const;

        /// Creates an instantiation of an asset of the given type.
        ///
        /// @param assetType [in] The type of asset to create.
        ///
        /// @return A pointer to the new asset object.
        virtual Asset* CreateAsset(AssetType assetType);

        /// Deletes the given asset.
        ///
        /// @param pAsset [in] A pointer to the asset object to delete.
        virtual void DeleteAsset(Asset* pAsset);
    };
}

#endif
