// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/MaterialAsset.hpp>

namespace GT
{
    MaterialAsset::MaterialAsset(AssetType assetType)
        : Asset(assetType)
    {
    }

    MaterialAsset::~MaterialAsset()
    {
    }


    AssetClass MaterialAsset::GetClass() const
    {
        return AssetClass_Material;
    }
}
