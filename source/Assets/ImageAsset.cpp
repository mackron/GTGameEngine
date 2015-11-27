// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/ImageAsset.hpp>

namespace GT
{
    ImageAsset::ImageAsset(const char* absolutePathOrIdentifier, AssetType type)
        : Asset(absolutePathOrIdentifier, type)
    {
    }

    ImageAsset::~ImageAsset()
    {
    }


    AssetClass ImageAsset::GetClass() const
    {
        return AssetClass_Image;
    }
}
