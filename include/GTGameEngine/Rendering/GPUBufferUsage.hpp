// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUBufferUsage_hpp_
#define __GT_GE_Rendering_GPUBufferUsage_hpp_

namespace GT
{
    namespace GE
    {
        enum GPUBufferUsage
        {
            GPUBufferUsage_Default   = 0,
            GPUBufferUsage_Immutable = 1,
            GPUBufferUsage_Dynamic   = 2,
            GPUBufferUsage_Staging   = 3
        };
    }
}

#endif
