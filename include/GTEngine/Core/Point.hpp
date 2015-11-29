// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Point_hpp_
#define __GTLib_Point_hpp_

namespace GTLib
{
    template <typename T>
    class Point
    {
    public:
        
        Point()
            : x(0), y(0)
        {
        }
        
        Point(T _x, T _y)
            : x(_x), y(_y)
        {
        }
        
    public:
    
        /// The x and y position.
        T x, y;
    
    };
}


#endif
