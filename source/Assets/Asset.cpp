// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/Asset.hpp>

namespace GT
{
    Asset::Asset(AssetType type)
        : m_type(type)
    {
    }

    Asset::~Asset()
    {
    }


    AssetType Asset::GetType() const
    {
        return m_type;
    }
}
