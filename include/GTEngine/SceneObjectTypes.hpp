// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneObjectTypes_hpp_
#define __GTEngine_SceneObjectTypes_hpp_

namespace GTEngine
{
    enum SceneObjectType
    {
        SceneObjectType_Unknown,
        SceneObjectType_None,

        SceneObjectType_SceneNode,
        SceneObjectType_PhysicsObject,
        SceneObjectType_Particle,

        SceneObjectType_Custom
    };
}

#endif
