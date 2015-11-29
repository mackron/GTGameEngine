// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_BufferTypes
#define GT_Rendering_BufferTypes

namespace GT
{
    enum BufferTypes
    {
        BufferType_Colour  = (1 << 1),
        BufferType_Depth   = (1 << 2),
        BufferType_Stencil = (1 << 3)
    };
}

#endif