// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_TextureFilters
#define GT_Rendering_TextureFilters

namespace GT
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