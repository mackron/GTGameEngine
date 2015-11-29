// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTEngine/Assets/ModelAsset.hpp>

namespace GT
{
    ModelAsset::ModelAsset(const char* absolutePathOrIdentifier, AssetType assetType)
        : Asset(absolutePathOrIdentifier, assetType)
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
