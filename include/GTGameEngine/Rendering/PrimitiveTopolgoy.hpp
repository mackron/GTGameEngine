// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_PrimitiveTopology_hpp_
#define __GT_GE_Rendering_PrimitiveTopology_hpp_

namespace GT
{
    namespace GE
    {
        enum PrimitiveTopology
        {
            PrimitiveTopology_Point                  = 0,
            PrimitiveTopology_Line                   = 1,
            PrimitiveTopology_LineStrip              = 2,
            PrimitiveTopology_Triangle               = 3,
            PrimitiveTopology_TriangleStrip          = 4,
            PrimitiveTopology_Line_Adjacency         = 5,
            PrimitiveTopology_LineList_Adjacency     = 6,
            PrimitiveTopology_Triangle_Adjacency     = 7,
            PrimitiveTopology_TriangleList_Adjacency = 8
        };
    }
}

#endif
