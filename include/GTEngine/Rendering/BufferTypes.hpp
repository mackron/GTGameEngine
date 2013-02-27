// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_BufferTypes_hpp_
#define __GTEngine_Rendering_BufferTypes_hpp_

namespace GTEngine
{
    enum BufferTypes
    {
        BufferType_Colour  = (1 << 1),
        BufferType_Depth   = (1 << 2),
        BufferType_Stencil = (1 << 3)
    };
}

#endif