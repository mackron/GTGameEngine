// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_RendererFunctions_hpp_
#define __GTEngine_Rendering_RendererFunctions_hpp_

namespace GTEngine
{
    enum RendererFunction
    {
        RendererFunction_Never,
        RendererFunction_Less,
        RendererFunction_Equal,
        RendererFunction_LEqual,
        RendererFunction_Greater,
        RendererFunction_NotEqual,
        RendererFunction_GEqual,
        RendererFunction_Always,
    };
}

#endif