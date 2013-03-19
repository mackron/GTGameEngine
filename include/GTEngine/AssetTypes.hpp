// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_AssetTypes_hpp_
#define __GTEngine_AssetTypes_hpp_

namespace GTEngine
{
    /// Enumerator for the different types of assets.
    enum AssetType
    {
        AssetType_None,
        
        // Basic
        AssetType_Image,
        AssetType_Model,
        AssetType_Material,
        AssetType_Sound,
        AssetType_ParticleSystem,
        AssetType_Script,
        
        // Special
        AssetType_Scene,
        AssetType_Prefab,
        AssetType_TextFile
    };
}

#endif