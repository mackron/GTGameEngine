// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/AssetAllocator.hpp>

namespace GT
{
    AssetAllocator::AssetAllocator()
    {
    }

    AssetAllocator::~AssetAllocator()
    {
    }


    AssetType AssetAllocator::GetAssetTypeByPath(const char*) const
    {
        return AssetType_Unknown;
    }

    bool AssetAllocator::IsAssetTypeSupported(AssetType) const
    {
        return false;
    }

    Asset* AssetAllocator::CreateAsset(AssetType)
    {
        return nullptr;
    }

    void AssetAllocator::DeleteAsset(Asset*)
    {
    }
}
