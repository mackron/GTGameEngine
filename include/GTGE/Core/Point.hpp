// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Point
#define GT_Point

namespace GT
{
    template <typename T>
    class gtPoint
    {
    public:
        
        gtPoint()
            : x(0), y(0)
        {
        }
        
        gtPoint(T _x, T _y)
            : x(_x), y(_y)
        {
        }
        
    public:
    
        /// The x and y position.
        T x, y;
    
    };
}


#endif
