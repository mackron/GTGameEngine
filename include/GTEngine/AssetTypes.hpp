
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
        AssetType_ParticleEffect,
        
        // Special
        AssetType_Scene,
        AssetType_TextFile
    };
}

#endif