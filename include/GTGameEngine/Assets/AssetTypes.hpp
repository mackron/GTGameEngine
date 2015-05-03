// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_AssetTypes_hpp_
#define __GT_AssetTypes_hpp_

#include <cstdint>

namespace GT
{
    typedef uint32_t AssetClass;
    typedef uint32_t AssetType;


    // Asset Class Ranges
    //  0x00000000 - 0x000000FF : Reserved by the engine.
    //  0x00000100 - 0xFFFFFFFF : Custom

    static const AssetClass AssetClass_Unknown   = 0x00000000;
    static const AssetClass AssetClass_Image     = 0x00000001;
    static const AssetClass AssetClass_Model     = 0x00000002;
    static const AssetClass AssetClass_Material  = 0x00000003;
    static const AssetClass AssetClass_Sound     = 0x00000004;

    
    // Asset Type Ranges
    //  0x00000000 - 0x0000FFFF : Reserved by the engine
    //  0x00010000 - 0xFFFFFFFF : Custom

    // Special - 0x00000000 - 0x000000FF
    static const AssetType AssetType_Unknown     = 0x00000000;

    // Images - 0x00000100 - 0x000001FF
    static const AssetType AssetType_Image_GTIMG = 0x00000100;      //< The engine's native image format.
    static const AssetType AssetType_Image_PNG   = 0x00000101;
    static const AssetType AssetType_Image_TGA   = 0x00000102;
    static const AssetType AssetType_Image_JPG   = 0x00000103;
    static const AssetType AssetType_Image_PSD   = 0x00000104;

    // Models - 0x00000200 - 0x000002FF
    static const AssetType AssetType_Model_GTMDL = 0x00000200;      //< The engine's native model format.
    static const AssetType AssetType_Model_OBJ   = 0x00000201;
    static const AssetType AssetType_Model_MD2   = 0x00000202;
    static const AssetType AssetType_Model_OGEX  = 0x00000203;

    // Materials - 0x00000300 - 0x000003FF
    static const AssetType AssetType_Sound_GTMTL = 0x00000300;      //< The engine's native material format.
    static const AssetType AssetType_Sound_MTL   = 0x00000301;

    // Sounds - 0x00000400 - 0x000004FF
    static const AssetType AssetType_Sound_GTSND = 0x00000400;      //< The engine's native sound format.
    static const AssetType AssetType_Sound_WAV   = 0x00000401;
    static const AssetType AssetType_Sound_OGG   = 0x00000402;



    /// Structure used for mapping a file extension to an asset type.
    struct AssetExtensionDesc
    {
        char extension[32];
        AssetClass classification;
        AssetType  type;
    };
}

#endif
