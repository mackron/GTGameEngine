// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_PrimitiveTopology_hpp_
#define __GT_GE_Rendering_PrimitiveTopology_hpp_

namespace GT
{
    enum GPUPrimitiveTopology
    {
        GPUPrimitiveTopology_Point                  = 0,
        GPUPrimitiveTopology_Line                   = 1,
        GPUPrimitiveTopology_LineStrip              = 2,
        GPUPrimitiveTopology_Triangle               = 3,
        GPUPrimitiveTopology_TriangleStrip          = 4,
        GPUPrimitiveTopology_Line_Adjacency         = 5,
        GPUPrimitiveTopology_LineList_Adjacency     = 6,
        GPUPrimitiveTopology_Triangle_Adjacency     = 7,
        GPUPrimitiveTopology_TriangleList_Adjacency = 8
    };
}

#endif
