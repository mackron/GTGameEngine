// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTEngine/Assets/SoundAsset.hpp>

namespace GT
{
    SoundAsset::SoundAsset(const char* absolutePathOrIdentifier, AssetType assetType)
        : Asset(absolutePathOrIdentifier, assetType)
    {
    }

    SoundAsset::~SoundAsset()
    {
    }


    AssetClass SoundAsset::GetClass() const
    {
        return AssetClass_Sound;
    }
}
