// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultAssetAllocator
#define GT_DefaultAssetAllocator

#include <GTGE/Config.hpp>

#if defined(GT_BUILD_DEFAULT_ASSETS)
#include <GTGE/Assets/AssetAllocator.hpp>

namespace GT
{
    /// The default asset allocator for assets that are supported natively by the engine.
    class DefaultAssetAllocator : public AssetAllocator
    {
    public:

        /// @copydoc AssetAllocator::GetAssetTypeByPath()
        AssetType GetAssetTypeByPath(const char* filePath) const;

        /// @copydoc AssetAllocator::IsAssetTypeSupported()
        bool IsAssetTypeSupported(AssetType assetType) const;

        /// @copydoc AssetAllocator::CreateAsset()
        Asset* CreateAsset(const char* absolutePathOrIdentifier, AssetType assetType);

        /// @copydoc AssetAllocator::DeleteAsset()
        void DeleteAsset(Asset* pAsset);
    };
}

#endif

#endif
