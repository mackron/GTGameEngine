// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeComponentTypes_hpp_
#define __GT_SceneNodeComponentTypes_hpp_

#include <cstdint>

namespace GT
{
    typedef uint32_t SceneNodeComponentTypeID;


    // Scene Node Component Type Ranges
    //  0x00000000 - 0x0000FFFF : Reserved by the engine.
    //  0x00010000 - 0xFFFFFFFF : Custom

    static const SceneNodeComponentTypeID SceneNodeComponentType_Unknown  = 0x00000000;
    static const SceneNodeComponentTypeID SceneNodeComponentType_Graphics = 0x00000001;
}

#endif
