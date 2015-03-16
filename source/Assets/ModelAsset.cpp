// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/ModelAsset.hpp>

namespace GT
{
    ModelAsset::ModelAsset(AssetType assetType)
        : Asset(assetType)
    {
    }

    ModelAsset::~ModelAsset()
    {
    }


    AssetClass ModelAsset::GetClass() const
    {
        return AssetClass_Model;
    }
}
