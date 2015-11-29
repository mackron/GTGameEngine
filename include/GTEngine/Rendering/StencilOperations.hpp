// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_StencilOperations
#define GT_Rendering_StencilOperations

namespace GT
{
    enum StencilOp
    {
        StencilOp_Keep,
        StencilOp_Zero,
        StencilOp_Replace,
        StencilOp_Increment,
        StencilOp_IncrementWrap,
        StencilOp_Decrement,
        StencilOp_DecrementWrap,
        StencilOp_Invert
    };
}

#endif