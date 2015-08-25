// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeComponentTypes
#define GT_SceneNodeComponentTypes

#include <cstdint>

namespace GT
{
    typedef uint32_t SceneNodeComponentTypeID;


    // Scene Node Component Type Ranges
    //  0x00000000 - 0x0000FFFF : Reserved by the engine.
    //  0x00010000 - 0xFFFFFFFF : Custom

    static const SceneNodeComponentTypeID SceneNodeComponentType_Unknown  = 0x00000000;
    static const SceneNodeComponentTypeID SceneNodeComponentType_Events   = 0x00000001;
    static const SceneNodeComponentTypeID SceneNodeComponentType_Script   = 0x00000002;
    static const SceneNodeComponentTypeID SceneNodeComponentType_Graphics = 0x00000003;
    static const SceneNodeComponentTypeID SceneNodeComponentType_Dynamics = 0x00000004;
}

#endif
