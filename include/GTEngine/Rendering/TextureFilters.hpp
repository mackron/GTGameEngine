// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_TextureFilters_hpp_
#define __GTEngine_Rendering_TextureFilters_hpp_

namespace GTEngine
{
    enum TextureFilter
    {
        TextureFilter_Nearest = 1,
        TextureFilter_Linear,

        TextureFilter_NearestNearest,
        TextureFilter_NearestLinear,
        TextureFilter_LinearNearest,
        TextureFilter_LinearLinear,

        TextureFilter_Point = TextureFilter_Nearest
    };
}

#endif