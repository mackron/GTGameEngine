// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleFunctionTypes_hpp_
#define __GTEngine_ParticleFunctionTypes_hpp_

namespace GTEngine
{
    /// Enumerator for each of the different particle functions.
    enum ParticleFunctionType
    {
        ParticleFunctionType_SizeOverTime             = 1,
        ParticleFunctionType_LinearVelocityOverTime   = 2,
        ParticleFunctionType_AngularVelocityOverTime  = 3,
    };
}

#endif