// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ParticleFunctionTypes
#define GT_ParticleFunctionTypes

namespace GT
{
    /// Enumerator for each of the different particle functions.
    enum ParticleFunctionType
    {
        ParticleFunctionType_SizeOverTime             = 1,
        ParticleFunctionType_LinearVelocityOverTime   = 2,
        ParticleFunctionType_AngularVelocityOverTime  = 3,
        ParticleFunctionType_ColourOverTime           = 4,
        ParticleFunctionType_AlphaOverTime            = 5,
    };
}

#endif