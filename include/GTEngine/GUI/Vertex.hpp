// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_Vertex
#define GT_GUI_Vertex

namespace GTGUI
{
    struct vec2
    {
        vec2(float _x, float _y)
            : x(_x), y(_y)
        {
        }

        float x;
        float y;
    };

    struct vec4
    {
        vec4(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }

        float x;
        float y;
        float z;
        float w;
    };


    /// Structure representing a vertex that will be used for drawing GUI geometry.
    struct Vertex
    {
        Vertex()
            : position(0.0f, 0.0f),
              texCoord(0.0f, 0.0f),
              colour(1.0f, 1.0f, 1.0f, 1.0f)
        {
        }

        vec2 position;
        vec2 texCoord;
        vec4 colour;
    };
}

#endif
